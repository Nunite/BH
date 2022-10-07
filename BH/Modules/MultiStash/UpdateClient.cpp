/*=================================================================
	File created by Yohann NICOLAS.
	Add support 1.13d by L'Autour.
	Add support 1.14d by haxifix.

	Updating client.

=================================================================*/

#include "UpdateClient.h"
#include "infinityStash.h"
#include "PlayerCustomData.h"
#include "../../Patch.h"
#include "../../D2Helpers.h"
//#include "../../D2Structs.h"
#include "../../D2Ptrs.h"

void updateSharedGold(DWORD goldAmount)
{
	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();
	LogMsg("SharedGold = %d\n", goldAmount);
	PCPY49->sharedGold = goldAmount;
}


void updateClient(UnitAnyHM* ptChar, DWORD mFunc, DWORD p1, DWORD p2, DWORD p3)
{
	void* ptNetClient;
	DataPacket packet;

	// Intialize the packet to send to client
	ZeroMemory(&packet, sizeof(DataPacket));
	packet.mType = 0x9D;
	packet.mFunc = (BYTE)mFunc;
	packet.mSize = sizeof(DataPacket);
	packet.mPlayerID = ptChar->nMemoryPool;  //这个是unitId
	packet.mParam1 = p1;
	packet.mParam2 = p2;
	packet.mParam3 = p3;

	//ptNetClient = D2GetClient(ptChar, __FILE__, __LINE__);
	ptNetClient = ptChar->ptPlayerData->ptNetClient;

	// Send packet to client for remove placed skills
	D2GameASM_SendPacket_STUB(ptNetClient, &packet, sizeof(DataPacket));  //先到这里吧
}

void updateClient(UnitAnyHM* ptChar, DWORD mFunc, char* msg)
{
	void* ptNetClient;
	DataPacket packet;

	// Intialize the packet to send to client
	ZeroMemory(&packet, sizeof(DataPacket));
	packet.mType = 0x9D;
	packet.mFunc = (BYTE)mFunc;
	packet.mSize = sizeof(DataPacket);
	packet.mPlayerID = ptChar->nMemoryPool;  //这个是unitId
	if (msg != NULL && strlen(msg) > 20)
		return;
	if (msg != NULL)
		strncpy((char*)&packet.mItemID, msg, 20);

	//ptNetClient = D2GetClient(ptChar, __FILE__, __LINE__);
	ptNetClient = ptChar->ptPlayerData->ptNetClient;

	// Send packet to client for remove placed skills
	D2GameASM_SendPacket_STUB(ptNetClient, &packet, sizeof(DataPacket));
}

DWORD __fastcall handleClientUpdate(DataPacket* packet)
{
	LogMsg("[CLIENT] Received custom message: %d with param: %08X , %08X , %08X\n", packet->mFunc, packet->mParam1, packet->mParam2, packet->mParam3);
	switch (packet->mFunc)
	{
	case UC_SELECT_STASH: setSelectedStashClient(packet->mParam1, packet->mParam2, packet->mParam3, (packet->mParam2 & 8) == 8); return 1;
	case UC_SHARED_GOLD: updateSharedGold(packet->mParam1); return 1;
	case UC_PAGE_NAME:
	{
		char pageName[21];
		strncpy(pageName, (char*)&packet->mItemID, 20);
		pageName[20] = NULL;
		renameCurrentStash((UnitAnyHM*)D2CLIENT_GetPlayerUnit(), pageName); return 1;
	}
	default: return 0;
	}
}

void __declspec(naked) caller_handleClientUpdate_111()
{
	__asm
	{
		LEA ECX, DWORD PTR SS : [ESP + 8]
		CALL handleClientUpdate
		POP EDI
		POP ESI
		MOV ESP, EBP
		POP EBP
		RETN
	}
}

void Install_UpdateClient()
{

	LogMsg("Install_UpdateClient 开始\n");
	//D2Client.dll offset AE896
	//caller_handleClientUpdate_111
	//这里比较特殊，得用patch去写了
	int patchAddr = Patch::GetDllOffset(D2CLIENT, 0xAE896);
	BYTE patchBytes[4] = { };
	patchBytes[1] = (DWORD)caller_handleClientUpdate_111-patchAddr+4;  //因为install没有JA类型的，所以直接这么做了
	Patch::WriteBytes(patchAddr, 1, patchBytes);
	

	LogMsg("Install_UpdateClient 结束\n");

}

/*================================= END OF FILE =================================*/