#include "LoadPlayerData.h"
#include "ExtendedSaveFile.h"
#include "../../D2Helpers.h"
#include "../../Patch.h"
#include "../../Constants.h"
#include "../../D2Ptrs.h"

enum {
	TS_SAVE_PERSONAL = 0,
	TS_SAVE_SHARED,
	TS_PUTGOLD,
	TS_TAKEGOLD
};

#pragma pack(1)
typedef struct {
	BYTE	packID;
	BYTE	packSize;
	DWORD	finalSize : 31;
	DWORD	isCustom : 1;
	BYTE	type;
	BYTE	data[0xFF];
} t_rcvMsg;
#pragma pack()

struct s_MPSaveFile
{
	DWORD clientID;
	int sizeExtended;
	int curExtended;
	BYTE* dataExtended;
	int sizeShared;
	int curShared;
	BYTE* dataShared;
	s_MPSaveFile* prev;
	s_MPSaveFile* next;
	bool completed;
};

static BYTE customPackID = 0x6C;
static s_MPSaveFile* receivedSaveFiles = NULL;

void freeCurrentCF(DWORD memoryPool, s_MPSaveFile** curSF)
{
	if (*curSF == NULL) return;
	FOG_FreeMem(memoryPool, (*curSF)->dataExtended, __FILE__, __LINE__, 0);
	FOG_FreeMem(memoryPool, (*curSF)->dataShared, __FILE__, __LINE__, 0);
	if ((*curSF)->next)
		(*curSF)->next->prev = (*curSF)->prev;
	if ((*curSF)->prev)
		(*curSF)->prev->next = (*curSF)->next;
	else
		receivedSaveFiles = (*curSF)->next;
	FOG_FreeMem(memoryPool, *curSF, __FILE__, __LINE__, 0);
	*curSF = NULL;
}

DWORD __stdcall LoadSPCustomData(UnitAnyHM* ptChar)
{
	DWORD size = 0;
	BYTE* data;

	LogMsg("--- Start LoadSPCustomData ---\n");
	//active_sharedStash = active_SharedStashInMultiPlayer > 0;
	bool active_sharedStash = false;  //这个先false
	LogMsg("active_sharedStash = %d : %d\n\n", active_sharedStash);

	if (!ptChar)
	{
		LogMsg("LoadSPCustomData : ptChar == NULL\n"); return 0x1B;
	}//Unknow failure
	if (ptChar->nUnitType != UNIT_PLAYER)
	{
		LogMsg("LoadSPCustomData : ptChar->nUnitType != UNIT_PLAYER\n"); return 0x1B;
	}//Unknow failure
	if (!ptChar->ptPlayerData)
	{
		LogMsg("LoadSPCustomData : PCPlayerData == NULL\n"); return 0x1B;
	}//Unknow failure
	if (!ptChar->ptGame)
	{
		LogMsg("LoadSPCustomData : PCGame == NULL\n"); return 0x1B;
	}//Unknow failure

	DWORD ret = 0;
	if (ptChar->ptGame->isLODGame)
	{
		LogMsg("is LOD Game\n");
		data = readExtendedSaveFile(ptChar->ptPlayerData->name, &size);
		ret = loadExtendedSaveFile(ptChar, data, size);
		FOG_MemDeAlloc(data, __FILE__, __LINE__, 0);
		if (!ret && active_sharedStash)  //共享仓库先跳过
		{
			//data = readSharedSaveFile(ptChar->ptPlayerData->name, &size);
			//ret = loadSharedSaveFile(ptChar, data, size);
			//FOG_MemDeAlloc(data, __FILE__, __LINE__, 0);
		}
	}
	else {
		LogMsg("is not LOD Game\n");
	}

	LogMsg("End LoadSPCustomData.\n\n");
	return ret;
}

void __declspec(naked) caller_LoadSPPlayerCustomData()
{
	__asm
	{
		MOV ESI, EAX
		TEST ESI, ESI
		JNZ JMP_LoadSPPlayerCustomData
		PUSH DWORD PTR SS : [ESP + 0x14]
		CALL LoadSPCustomData
		MOV ESI, EAX
		TEST ESI, ESI
		JNZ JMP_LoadSPPlayerCustomData
		RETN
	JMP_LoadSPPlayerCustomData :
		ADD DWORD PTR SS : [ESP] , 0x17
		RETN
	}
}

DWORD __stdcall LoadMPCustomData(UnitAnyHM* ptChar)
{
	LogMsg("Start LoadMPCustomData\n");
	//active_sharedStash = active_SharedStashInMultiPlayer == 2;
	bool active_sharedStash = false;  //共享仓库先去掉
	LogMsg("active_sharedStash = %d : %d\n\n", active_sharedStash);

	if (!ptChar) return NULL;
	if (!ptChar)
	{
		LogMsg("LoadMPCustomData : ptChar == NULL\n"); return 0x1B;
	}//Unknow failure
	if (ptChar->nUnitType != UNIT_PLAYER)
	{
		LogMsg("LoadMPCustomData : ptChar->nUnitType != UNIT_PLAYER\n"); return 0x1B;
	}//Unknow failure
	if (!ptChar->ptPlayerData)
	{
		LogMsg("LoadMPCustomData : PCPlayerData == NULL\n"); return 0x1B;
	}//Unknow failure

	//NetClient* ptClient = D2GetClient(ptChar, __FILE__, __LINE__);
	NetClient* ptClient = ptChar->ptPlayerData->ptNetClient;

	s_MPSaveFile* curSF = receivedSaveFiles;
	while (curSF && (ptClient->clientID != curSF->clientID))
		curSF = curSF->next;

	DWORD ret = 0;

	if (!curSF)
	{
		LogMsg("Server has received no data from extra save files of character %s\n", ptChar->ptPlayerData->name);
		ret = 0xE;//Unable to enter game, generic bad file
	}

	if (!ret && !curSF->completed)
	{
		LogMsg("Server hasn't receive all data from extra save files of character %s\n", ptChar->ptPlayerData->name);
		ret = 0xE;//Unable to enter game, generic bad file
	}

	if (!ret && !ptChar->ptGame)
	{
		LogMsg("LoadMPCustomData : PCGame == NULL\n");
		ret = 0x1B;//Unknow failure
	}

	if (ptChar->ptGame->isLODGame)
	{
		LogMsg("is LOD Game\n");
		if (!ret)
			ret = loadExtendedSaveFile(ptChar, curSF->dataExtended, curSF->sizeExtended);
		//if (!ret && active_sharedStash)
			//ret = loadSharedSaveFile(ptChar, curSF->dataShared, curSF->sizeShared);  //共享仓库先去掉
	}
	else {
		LogMsg("is not LOD Game\n");
	}

	freeCurrentCF(ptChar->ptGame->memoryPool, &curSF);

	LogMsg("--- End LoadMPCustomData. ---\n\n");

	return ret;
}

void __declspec(naked) caller_LoadMPPlayerCustomData_111()
{
	__asm
	{
		PUSH DWORD PTR SS : [EBX]
		CALL LoadMPCustomData
		TEST EAX, EAX
		JNZ JMP_LoadMPlayerCustomData
		CMP DWORD PTR DS : [EBX] , 0
		JNZ Continue_LoadMP
		ADD DWORD PTR SS : [ESP] , 0x21
	Continue_LoadMP :
		RETN
	JMP_LoadMPlayerCustomData :
		SUB DWORD PTR SS : [ESP] , 0x12
		RETN
	}
}

void sendData(BYTE* data, DWORD size, bool isShared)
{
	t_rcvMsg pack;
	//	d2_assert(size >= 0x40000000, "size of file > 0x40000000", __FILE__, __LINE__);

	pack.packID = customPackID;
	pack.finalSize = size;
	pack.isCustom = true;
	pack.type = isShared ? TS_SAVE_SHARED : TS_SAVE_PERSONAL;

	DWORD sended = 0;
	while (sended < size)
	{
		pack.packSize = (BYTE)(size - sended > 0xFE ? 0xFF : size - sended + 1);
		CopyMemory(pack.data, &data[sended], pack.packSize);
		LogMsg("Loading Send Packet: \ttype=%X\t finalSize=%X\t packSize=%02X\t data=%08X\n", pack.type, pack.finalSize, pack.packSize, pack.data);
		D2NET_SendToServer(pack.packSize + 7, 0, &pack);
		sended += pack.packSize - 1;
	}
	LogMsg("\n");
}

void __fastcall SendSaveFiles(char* ptPath, DWORD maxsize, char* name)
{
	FOG_GetSavePath(ptPath, maxsize);

	LogMsg("\n--- Start SendSaveFiles ---\n");
	bool active_sharedStash = false;  //共享先关掉
	LogMsg("active_sharedStash = %d : %d\n\n", active_sharedStash);

	// Send Extended Save File
	LogMsg("Send Extended Save File\n");
	DWORD sizeExtended = 0;
	BYTE* dataExtended = readExtendedSaveFile(name, &sizeExtended);
	sendData(dataExtended, sizeExtended, false);
	FOG_MemDeAlloc(dataExtended, __FILE__, __LINE__, 0);

	// Send Shared Save File
	//if (active_sharedStash)  //共享仓库先去掉
	//{
	//	LogMsg("Send Shared Save File\n");
	//	DWORD sizeShared = 0;
	//	BYTE* dataShared = readSharedSaveFile(name, &sizeShared);
	//	sendData(dataShared, sizeShared, true);
	//	D2FogMemDeAlloc(dataShared, __FILE__, __LINE__, 0);
	//}

	// Ending load
	LogMsg("End SendSaveFiles.\n\n");
}

void __declspec(naked) caller_SendSaveFiles_111()
{
	__asm
	{
		POP EAX
		PUSH DWORD PTR CS : [EAX + 0x01] //"name"
		PUSH EAX
		JMP SendSaveFiles
	}
}

DWORD __stdcall ReceiveSaveFiles(DWORD clientID, t_rcvMsg* msg)
{
	if ((msg->packID != customPackID) || !msg->isCustom) return msg->packID;

	LogMsg("Loading Receive Packet: clientID=%d\t type=%X\t finalSize=%X\t packSize=%02X\t data=%08X\n", clientID, msg->type, msg->finalSize, msg->packSize, msg->data);
	bool active_sharedStash = false;  //共享仓库先不做
	LogMsg("active_sharedStash = %d : %d\n\n", active_sharedStash);

	bool isShared;

	switch (msg->type)
	{
	case TS_SAVE_PERSONAL:	isShared = false; break;
	case TS_SAVE_SHARED:	isShared = true; break;
		//	case TS_PUTGOLD :		putGold(ptChar, (DWORD)msg->data); return 0;
		//	case TS_TAKEGOLD :		takeGold(ptChar, (DWORD)msg->data); return 0;
	default: return 0;//return msg->packID;
	}
	NetClient** ptClientTable = p_D2GAME_ClientTable;
	NetClient* ptClient = ptClientTable[clientID & 0xFF];

	s_MPSaveFile* curSF = receivedSaveFiles;
	while (curSF && (clientID != curSF->clientID))
		curSF = curSF->next;

	if (curSF && curSF->completed)
		freeCurrentCF(0 * ptClient->ptGame->memoryPool, &curSF);


	if (!curSF)
	{
		curSF = (s_MPSaveFile*)FOG_AllocMem(0 * ptClient->ptGame->memoryPool, sizeof(s_MPSaveFile), __FILE__, __LINE__, 0);
		ZeroMemory(curSF, sizeof(s_MPSaveFile));
		curSF->clientID = clientID;
		curSF->next = receivedSaveFiles;
		if (receivedSaveFiles) receivedSaveFiles->prev = curSF;
		receivedSaveFiles = curSF;
	}

	LogMsg("curSF = %08X\tcurSF->completed = %d\n", curSF, curSF->completed);


	DWORD size = msg->packSize - 1;
	if (isShared)
	{
		if (curSF->sizeShared == 0)
			curSF->sizeShared = msg->finalSize;
		d2_assert(curSF->sizeShared != msg->finalSize, "Size of shared file has change", __FILE__, __LINE__);

		if (!curSF->dataShared)
			curSF->dataShared = (BYTE*)FOG_AllocMem(0 * ptClient->ptGame->memoryPool, curSF->sizeShared, __FILE__, __LINE__, 0);//		d2_assert(!curSF->dataShared, "Can't malloc data", __FILE__, __LINE__);

		CopyMemory(&curSF->dataShared[curSF->curShared], msg->data, size);
		curSF->curShared += size;

	}
	else {

		if (curSF->sizeExtended == 0)
			curSF->sizeExtended = msg->finalSize;
		d2_assert(curSF->sizeExtended != msg->finalSize, "Size of extented file has change", __FILE__, __LINE__);

		if (!curSF->dataExtended)
			curSF->dataExtended = (BYTE*)FOG_AllocMem(0 * ptClient->ptGame->memoryPool, curSF->sizeExtended, __FILE__, __LINE__, 0);//		d2_assert(!curSF->dataExtended, "Can't malloc data", __FILE__, __LINE__);

		CopyMemory(&curSF->dataExtended[curSF->curExtended], msg->data, size);
		curSF->curExtended += size;
	}

	if (curSF->sizeExtended && (curSF->sizeExtended == curSF->curExtended) && curSF->sizeShared && (curSF->sizeShared == curSF->curShared))
	{
		curSF->completed = true;
		LogMsg("ReceiveSaveFiles completed.\n");
	}
	LogMsg("End Load Receive Packet\n");

	return msg->packID + 10;
}

void __declspec(naked) caller_ReceiveSaveFiles_111()
{
	__asm
	{
		PUSH ECX
		PUSH EDX
		LEA EBX, DWORD PTR DS : [EBP + 4]
		PUSH EBX		//Message
		MOV EBX, DWORD PTR SS : [EBP]
		PUSH EBX		//ID client
		CALL ReceiveSaveFiles
		POP EDX
		POP ECX
		RETN
	}
}

void Install_LoadPlayerData() {
	LogMsg("Install_LoadPlayerData 开始\n");
	//D2Game.dll offset 5638D
	//caller_LoadSPPlayerCustomData
	//PlugY抄过来的地址如果没有包含call都要减个1
	Patch* patch1 = new Patch(Call, D2GAME, { 0x5638D,0x5638D }, (DWORD)caller_LoadSPPlayerCustomData, 6);  //5+1，这个1是为了后面的一个字节为90
	patch1->Install();

	//D2Game.dll offset 56217
	//caller_LoadMPPlayerCustomData_111
	Patch* patch2 = new Patch(Call, D2GAME, { 0x56217,0x56217 }, (DWORD)caller_LoadMPPlayerCustomData_111, 5);
	patch2->Install();

	//D2Client.dll offset 1457C
	//caller_SendSaveFiles_111
	Patch* patch3 = new Patch(Call, D2CLIENT, { 0x1457C - 1,0x1457C - 1 }, (DWORD)caller_SendSaveFiles_111, 5);
	patch3->Install();

	//D2Game.dll offset CAF39
	//caller_ReceiveSaveFiles_111
	Patch* patch4 = new Patch(Call, D2GAME, { 0xCAF39,0xCAF39 }, (DWORD)caller_ReceiveSaveFiles_111, 7);
	patch4->Install();

	LogMsg("Install_LoadPlayerData 结束\n");
}