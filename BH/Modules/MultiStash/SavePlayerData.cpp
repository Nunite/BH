/*=================================================================
	File created by Yohann NICOLAS.
	Add support 1.13d by L'Autour.

    Save Player Custom Data.

=================================================================*/

#include <time.h>
#include <stdio.h>
#include "../../Patch.h"
#include "../../D2Ptrs.h"
#include "../../D2Helpers.h"
#include "PlayerCustomData.h"
#include "ExtendedSaveFile.h"


//自定义保存
void __stdcall SaveSPPlayerCustomData(UnitAnyHM* ptChar) {
	if (!ptChar->ptGame->isLODGame) return;  //非资料片就直接返回
	LogMsg("\n--- 开始保存人物存档 ---\n");
	//NetClient* ptClient = D2GetClient(ptChar, __FILE__, __LINE__);  //PlugY的这个方法对应的地址offset都是000000，貌似是说只有1.11的版本才支持？
	NetClient* ptClient = ptChar->ptPlayerData->ptNetClient;

	if (PCPY49->selfStashIsOpened) {
		DWORD curSizeExt = 0;
		DWORD maxSizeExt = 0x4000;
		BYTE* dataExt = (BYTE*)FOG_AllocMem(ptChar->ptGame->memoryPool, maxSizeExt, __FILE__, __LINE__, 0);
		d2_assert(!dataExt, "Error : Memory allocation Extended SaveFile", __FILE__, __LINE__);
		saveExtendedSaveFile(ptChar, &dataExt, &maxSizeExt, &curSizeExt);
		writeExtendedSaveFile(ptChar->ptPlayerData->name, dataExt, curSizeExt);
		FOG_FreeMem(ptChar->ptGame->memoryPool, dataExt, __FILE__, __LINE__, 0);
	}
	if (PCPY49->sharedStashIsOpened) {  //这里先不做，试试看先
		//DWORD curSizeShr = 0;
		//DWORD maxSizeShr = 0x4000;
		//BYTE* dataShr = (BYTE*)FOG_MemAlloc( maxSizeShr, __FILE__, __LINE__, 0);
		//d2_assert(!dataShr, "Error : Memory allocation Shared SaveFile", __FILE__, __LINE__);
		//saveSharedSaveFile(ptChar, &dataShr, &maxSizeShr, &curSizeShr);

		//writeSharedSaveFile(PCPlayerData->name, dataShr, curSizeShr, ptClient->isHardCoreGame);
		//FOG_FreeMem(dataShr, __FILE__, __LINE__, 0);
	}

	LogMsg("\n--- 结束保存人物存档 ---\n");
}

void __declspec(naked) caller_SaveSPPlayerCustomData_111()
{
	__asm
	{
			MOV EAX,DWORD PTR SS:[ESP+0x14]
			TEST EAX,EAX
			JE next
			PUSH DWORD PTR SS:[ESP+0x8]
			CALL SaveSPPlayerCustomData
		next:
			JMP D2GAME_SaveSPChar
	}
}

struct s_dataToSend
{
	s_dataToSend* next;
	int		clientID;
	int		sizeExtended;
	int		curExtended;
	BYTE* dataExtended;
	int		sizeShared;
	int		curShared;
	BYTE* dataShared;
	bool	init;
};
s_dataToSend* ptDataToSend = NULL;

void __stdcall SendSaveFilesToSave(UnitAnyHM* ptChar) {
	LogMsg("\n--- 开始 SendSaveFilesToSave ---\n");

	DWORD curSizeExt = 0;
	BYTE* dataExt = NULL;
	DWORD curSizeShr = 0;
	BYTE* dataShr = NULL;

	if (PCPY49->selfStashIsOpened)
	{
		DWORD maxSizeExt = 0x4000;
		dataExt = (BYTE*)FOG_AllocMem(ptChar->ptGame->memoryPool,maxSizeExt, __FILE__, __LINE__, 0);
		d2_assert(!dataExt, "Error : Memory allocation Extended SaveFile", __FILE__, __LINE__);
		saveExtendedSaveFile(ptChar, &dataExt, &maxSizeExt, &curSizeExt);
	}

	if (PCPY49->sharedStashIsOpened)  //共享的先放一下
	{
		//DWORD maxSizeShr = 0x4000;
		//dataShr = (BYTE*)D2AllocMem(PCGame->memoryPool, maxSizeShr, __FILE__, __LINE__, 0);
		//d2_assert(!dataShr, "Error : Memory allocation Shared SaveFile", __FILE__, __LINE__);
		//saveSharedSaveFile(ptChar, &dataShr, &maxSizeShr, &curSizeShr);
	}

	//NetClient* ptClient = D2GetClient(ptChar, __FILE__, __LINE__);
	NetClient* ptClient = ptChar->ptPlayerData->ptNetClient;
	s_dataToSend* dataToSend = ptDataToSend;
	while (dataToSend && (dataToSend->clientID != ptClient->clientID))
		dataToSend = dataToSend->next;
	if (!dataToSend)
	{
		dataToSend = (s_dataToSend*)FOG_AllocMem(ptChar->ptGame->memoryPool, sizeof(s_dataToSend), __FILE__, __LINE__, 0);
		ZeroMemory(dataToSend, sizeof(s_dataToSend));
		dataToSend->next = ptDataToSend;
		ptDataToSend = dataToSend;
	}

	//Sending savefiles
	dataToSend->clientID = ptClient->clientID;
	dataToSend->init = 1;
	dataToSend->sizeExtended = curSizeExt;
	dataToSend->curExtended = 0;
	dataToSend->dataExtended = dataExt;
	dataToSend->sizeShared = curSizeShr;
	dataToSend->curShared = 0;
	dataToSend->dataShared = dataShr;
	LogMsg("SendSaveFilesToSave : clientID=%d\t init=%d\t sizeExtended=%d\t curExtended=%d\t dataExtended=%d\t sizeShared=%d\t curShared=%d\t dataShared=%08X\n",
		dataToSend->clientID, dataToSend->init, dataToSend->sizeExtended, dataToSend->curExtended, dataToSend->dataExtended, dataToSend->sizeShared, dataToSend->curShared, dataToSend->dataShared);


	LogMsg("SendSaveFilesToSave : 结束\n\n");
}


void __declspec(naked) caller_SendSaveFilesToSave_111()
{
	__asm
	{
		PUSH DWORD PTR SS : [ESP + 0x2014]
		CALL SendSaveFilesToSave
		MOV EAX, DWORD PTR SS : [ESP + 0x8]
		TEST EAX, EAX
		RETN
	}
}

#define TC_SAVE_PERSONAL 0
#define TC_SAVE_SHARED 1

#pragma pack(1)
typedef struct {
	BYTE	packID;
	BYTE	packSize;
	BYTE	init;
	DWORD	finalSize : 31;
	DWORD	isCustom : 1;
	BYTE	type;
	BYTE	data[0xFF];
} t_rcvMsg;
#pragma pack()

struct s_MPSaveFile
{
	int		sizeExtended;
	int		curExtended;
	BYTE* dataExtended;
	int		sizeShared;
	int		curShared;
	BYTE* dataShared;
};
static s_MPSaveFile receivedSaveFiles;

static BYTE customPackID = 0xB2;


DWORD __stdcall ManageNextPacketToSend(NetClient* ptClient)
{
	LogMsg("ManageNextPacketToSend\n");
	s_dataToSend* dataToSend = ptDataToSend;
	while (dataToSend && (dataToSend->clientID != ptClient->clientID))
		dataToSend = dataToSend->next;
	if (!dataToSend) return 1;

	LogMsg("ManageNextPacketToSend : clientID=%d\t init=%d\t sizeExtended=%d\t curExtended=%d\t dataExtended=%08X\t sizeShared=%d\t curShared=%d\t dataShared=%08X\n",
		dataToSend->clientID, dataToSend->init, dataToSend->sizeExtended, dataToSend->curExtended, dataToSend->dataExtended, dataToSend->sizeShared, dataToSend->curShared, dataToSend->dataShared);

	if (dataToSend->sizeExtended && dataToSend->dataExtended && (dataToSend->curExtended < dataToSend->sizeExtended))
	{
		DWORD remainingData = dataToSend->sizeExtended - dataToSend->curExtended;
		t_rcvMsg* msg = (t_rcvMsg*)FOG_AllocMem(ptClient->ptGame->memoryPool, sizeof(t_rcvMsg), __FILE__, __LINE__, 0);
		msg->packID = customPackID;
		msg->init = dataToSend->init;
		msg->finalSize = dataToSend->sizeExtended;
		msg->type = TC_SAVE_PERSONAL;
		msg->isCustom = true;
		msg->packSize = remainingData > 0xFE ? 0xFF : (BYTE)remainingData + 1;
		CopyMemory(msg->data, &dataToSend->dataExtended[dataToSend->curExtended], msg->packSize);
		LogMsg("ManageNextPacketToSend : type=%d\t init=%d\t finalSize=%d\t packSize=%d\t data=%08X\n", msg->type, msg->init, msg->finalSize, msg->packSize, msg->data);
		D2NET_SendToClient(0, dataToSend->clientID, msg, msg->packSize + 7);
		dataToSend->init = false;
		dataToSend->curExtended += msg->packSize - 1;
		FOG_FreeMem(ptClient->ptGame->memoryPool,msg, __FILE__, __LINE__, 0);
		if (dataToSend->curExtended == dataToSend->sizeExtended)
		{
			dataToSend->sizeExtended = 0;
			dataToSend->curExtended = 0;
			FOG_FreeMem(ptClient->ptGame->memoryPool, dataToSend->dataExtended, __FILE__, __LINE__, 0);
			dataToSend->dataExtended = NULL;
		}
		LogMsg("ManageNextPacketToSend1 : End\n");
		return 0;//ManageNextPacketToSend(ptClient);//return 0;
	}

	if (dataToSend->sizeShared && dataToSend->dataShared && (dataToSend->curShared < dataToSend->sizeShared))
	{
		DWORD remainingData = dataToSend->sizeShared - dataToSend->curShared;
		t_rcvMsg* msg = (t_rcvMsg*)FOG_AllocMem(ptClient->ptGame->memoryPool, sizeof(t_rcvMsg), __FILE__, __LINE__, 0);
		msg->packID = customPackID;
		msg->init = dataToSend->init;
		msg->finalSize = dataToSend->sizeShared;
		msg->type = TC_SAVE_SHARED;
		msg->isCustom = true;
		msg->packSize = remainingData > 0xFE ? 0xFF : (BYTE)remainingData + 1;
		CopyMemory(msg->data, &dataToSend->dataShared[dataToSend->curShared], msg->packSize);
		LogMsg("ManageNextPacketToSend : type=%d\t init=%d\t finalSize=%d\t packSize=%d\t data=%08X\n", msg->type, msg->init, msg->finalSize, msg->packSize, msg->data);
		D2NET_SendToClient(0, dataToSend->clientID, msg, msg->packSize + 7);
		dataToSend->init = false;
		dataToSend->curShared += msg->packSize - 1;
		FOG_FreeMem(ptClient->ptGame->memoryPool, msg, __FILE__, __LINE__, 0);
		if (dataToSend->curShared == dataToSend->sizeShared)
		{
			dataToSend->sizeShared = 0;
			dataToSend->curShared = 0;
			FOG_FreeMem(ptClient->ptGame->memoryPool, dataToSend->dataShared, __FILE__, __LINE__, 0);
			dataToSend->dataShared = NULL;
		}
		LogMsg("ManageNextPacketToSend2 : End\n");
		return 0;//ManageNextPacketToSend(ptClient);//return 0;
	}

	LogMsg("ManageNextPacketToSend3 : End\n");
	return 1;
}

void __declspec(naked) caller_ManageNextPacketToSend()
{
	__asm
	{
		PUSH ESI
		CALL ManageNextPacketToSend
		TEST EAX, EAX
		JNZ end_caller_ManageNextPacketToSend
		XOR ECX, ECX
		RETN
	end_caller_ManageNextPacketToSend :
		MOV ECX, DWORD PTR DS : [ESI + 0x17C]
		RETN
	}
}

DWORD __stdcall ReceiveSaveFilesToSave(t_rcvMsg* msg)
{
	if ((msg->packID != customPackID) || !msg->isCustom) return 0;

	LogMsg("ReceiveSaveFilesToSave : Receive Packet - type=%d\t init=%d\t finalSize=%d\t packSize=%d\t data=%08X\n", msg->type, msg->init, msg->finalSize, msg->packSize, msg->data);

	bool isShared;

	if (msg->type == TC_SAVE_PERSONAL)
		isShared = false;
	else if (msg->type == TC_SAVE_SHARED)
		isShared = true;
	else return 0;


	if (msg->init)
	{
		FOG_MemDeAlloc(receivedSaveFiles.dataExtended, __FILE__, __LINE__, 0);
		FOG_MemDeAlloc(receivedSaveFiles.dataShared, __FILE__, __LINE__, 0);
		ZeroMemory(&receivedSaveFiles, sizeof(receivedSaveFiles));
	}

	LogMsg("ReceiveSaveFilesToSave : sizeExtended=%d\t curExtended=%d\t dataExtended=%08X\t sizeShared=%d\t curShared=%d\t dataShared=%08X\n", receivedSaveFiles.sizeExtended, receivedSaveFiles.curExtended, receivedSaveFiles.dataExtended, receivedSaveFiles.sizeShared, receivedSaveFiles.curShared, receivedSaveFiles.dataShared);

	DWORD size = msg->packSize - 1;
	if (isShared)
	{
		if (receivedSaveFiles.sizeShared == 0)
			receivedSaveFiles.sizeShared = msg->finalSize;
		//		d2_assert(receivedSaveFiles.sizeShared != msg->finalSize, "Size of shared file has change", __FILE__, __LINE__);

		if (!receivedSaveFiles.dataShared)
			receivedSaveFiles.dataShared = (BYTE*)FOG_MemAlloc(receivedSaveFiles.sizeShared, __FILE__, __LINE__, 0);

		CopyMemory(&receivedSaveFiles.dataShared[receivedSaveFiles.curShared], msg->data, size);
		receivedSaveFiles.curShared += size;

	}
	else {

		if (receivedSaveFiles.sizeExtended == 0)
			receivedSaveFiles.sizeExtended = msg->finalSize;
		//		d2_assert(receivedSaveFiles.sizeExtended != msg->finalSize, "Size of extented file has change", __FILE__, __LINE__);

		if (!receivedSaveFiles.dataExtended)
			receivedSaveFiles.dataExtended = (BYTE*)FOG_MemAlloc(receivedSaveFiles.sizeExtended, __FILE__, __LINE__, 0);

		CopyMemory(&receivedSaveFiles.dataExtended[receivedSaveFiles.curExtended], msg->data, size);
		receivedSaveFiles.curExtended += size;
	}
	LogMsg("ReceiveSaveFilesToSave : End\n");
	return 1;
}

void __declspec(naked) caller_ReceivedSaveFilesToSave_111()
{
	__asm
	{
		LEA EAX, DWORD PTR SS : [ESP + 0x10]
		PUSH EAX
		CALL ReceiveSaveFilesToSave
		TEST EAX, EAX
		JE continue_rcvFct
		ADD DWORD PTR SS : [ESP] , 0x3D
		RETN
	continue_rcvFct :
		MOVZX EAX, BYTE PTR SS : [ESP + 0x10]
		RETN
	}
}

void __stdcall SaveMPPlayerCustomData(BYTE* dataD2Savefile)
{
	LogMsg("SaveMPPlayerCustomData - Start.\n");

	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();
	//backupSaveFiles(PCPlayerData->name, (dataD2Savefile[0x24] & 4) == 4);
	bool active_sharedStash = false;  //这里暂时先不开启共享仓库
	//if (active_PlayerCustomData)
	//{
		if (receivedSaveFiles.sizeExtended && (receivedSaveFiles.sizeExtended == receivedSaveFiles.curExtended) && (!active_sharedStash || receivedSaveFiles.sizeShared && (receivedSaveFiles.sizeShared == receivedSaveFiles.curShared)))
		{
			LogMsg("SaveMPPlayerCustomData - Saving can start\n");
			if (ptChar)
			{
				writeExtendedSaveFile(ptChar->ptPlayerData->name, receivedSaveFiles.dataExtended, receivedSaveFiles.sizeExtended);
				FOG_MemDeAlloc(receivedSaveFiles.dataExtended, __FILE__, __LINE__, 0);
				//if (active_sharedStash)
				//{
				//	writeSharedSaveFile(PCPlayerData->name, receivedSaveFiles.dataShared, receivedSaveFiles.sizeShared, (dataD2Savefile[0x24] & 4) == 4);//6FBAB9A4-6FAA0000
				//	D2FogMemDeAlloc(receivedSaveFiles.dataShared, __FILE__, __LINE__, 0);
				//}
				ZeroMemory(&receivedSaveFiles, sizeof(receivedSaveFiles));
			}
			LogMsg("SaveMPPlayerCustomData - Receive Savefiles completed\n\n");
		}
		else {
			if (ptChar)
			{
				LogMsg("Connection error : save with client data.");
				SaveSPPlayerCustomData(ptChar);
			}
		}
	//}
	LogMsg("--- SaveMPPlayerCustomData End ---\n\n");
}

void __declspec(naked) caller_SaveMPPlayerCustomData_111()
{
	__asm
	{
		PUSH EAX
		PUSH ECX
		PUSH EAX
		CALL SaveMPPlayerCustomData
		POP ECX
		POP EAX
		CMP ECX, 0xAA55AA55
		RETN
	}
}

void Install_SavePlayerData()
{
	
	LogMsg("Install_SavePlayerData 开始\n");

	//Save single player custom data.
	//113c  D2Game.dll offset 543D1
	//caller_SaveSPPlayerCustomData_111   这个是保存单机存档？
	//PlugY抄过来的地址都要减个1，因为它没包含前面的这个call字节
	Patch* patch1 = new Patch(Call, D2GAME, { 0x543D1 - 1,0x543D1 - 1 }, (DWORD)caller_SaveSPPlayerCustomData_111, 5);
	patch1->Install();

	//113c D2Game.dll offset 53EAB
	//caller_SendSaveFilesToSave_111   这个是发送存档到存储？
	int patchAddr = Patch::GetDllOffset(D2GAME, 0x53EAB);  //第1个字节用90填充
	BYTE patchBytes[1] = { 0x90 };
	Patch::WriteBytes(patchAddr, 1, patchBytes);
	Patch* patch2 = new Patch(Call, D2GAME, { 0x53EAB+1,0x53EAB + 1 }, (DWORD)caller_SendSaveFilesToSave_111, 5);
	patch2->Install();

	//113c D2Game.dll offset 2D173
	//caller_ManageNextPacketToSend
	int patch1Addr = Patch::GetDllOffset(D2GAME, 0x2D173);  //第1个字节用90填充
	BYTE patch1Bytes[1] = { 0x90 };
	Patch::WriteBytes(patch1Addr, 1, patch1Bytes);
	Patch* patch3 = new Patch(Call, D2GAME, { 0x2D173 + 1,0x2D173 + 1 }, (DWORD)caller_ManageNextPacketToSend, 5);
	patch3->Install();

	//113c D2Client.dll offset 43946
	//caller_ReceivedSaveFilesToSave_111
	Patch* patch4 = new Patch(Call, D2CLIENT, { 0x43946,0x43946 }, (DWORD)caller_ReceivedSaveFilesToSave_111, 5);
	patch4->Install();

	//113c D2Client.dll offset AC572
	//caller_SaveMPPlayerCustomData_111
	Patch* patch5 = new Patch(Call, D2CLIENT, { 0xAC572,0xAC572}, (DWORD)caller_SaveMPPlayerCustomData_111, 6);  //用6是后面一个字节也用90填充
	patch5->Install();

	customPackID++;

	LogMsg("Install_SavePlayerData 结束\n");
}

/*================================= END OF FILE =================================*/