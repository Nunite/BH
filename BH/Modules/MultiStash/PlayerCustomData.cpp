#include "PlayerCustomData.h"
#include "SavePlayerData.h" //Install_SavePlayerData()
#include "LoadPlayerData.h"
#include "UpdateClient.h"
#include "InfinityStash.h"
#include "../../D2Helpers.h"
#include "../../Patch.h"
#include "../../D2Ptrs.h"
#include "../../Constants.h"

// Convertion to 1.09
struct s_shifting {
	DWORD ptInventory;
	DWORD ptSpecificData;
	DWORD ptPYPlayerData;
	DWORD ptGame;
	DWORD ptClientGame;
	DWORD ptSkills;
	DWORD ptImage;
	DWORD ptFrame;
};
extern s_shifting shifting;
s_shifting shifting;

void freeMessage(sWinMessage* msg)
{
	msg->managed = 1;
	msg->unmanaged = 0;
	STORM_FreeWinMessage(msg);
}

void setImage(sDrawImageInfo* data, void* image) { ((void**)data)[shifting.ptImage / 4] = image; }//0x4 0x8 0x3C
void setFrame(sDrawImageInfo* data, DWORD frame) { ((DWORD*)data)[shifting.ptFrame / 4] = frame; }//0x8 0x44 0x40

PlayerData* __fastcall init_PlayerCustomData(DWORD p1, DWORD size, LPCSTR file, DWORD line, DWORD p5)
{
	LogMsg("init_PlayerCustomData\n");
	PlayerData* playerData = (PlayerData*)FOG_AllocMem(p1, size + sizeof(PYPlayerData), file, line, p5);
	ZeroMemory((BYTE*)playerData + shifting.ptPYPlayerData, sizeof(PYPlayerData));
	return playerData;
}

Stash* getStashFromItem(UnitAnyHM* ptChar, UnitAnyHM* ptItem)
{
	Stash* curStash = PCPY49->selfStash;
	UnitAnyHM* curItem;
	while (curStash)
	{
		if (curStash == PCPY49->currentStash)
			curItem = D2COMMON_GetItemFromInventoryHM(ptChar->ptInventory);
		else
			curItem = curStash->ptListItem;

		while (curItem)
		{
			if (D2COMMON_GetRealItem(curItem) == ptItem) return curStash;
			curItem = D2COMMON_GetNextItemFromInventoryHM(curItem);
		}
		curStash = curStash->nextStash;
	}

	curStash = PCPY49->sharedStash;
	while (curStash)
	{
		if (curStash == PCPY49->currentStash)
			curItem = D2COMMON_GetItemFromInventoryHM(ptChar->ptInventory);
		else curItem = curStash->ptListItem;
		while (curItem)
		{
			if (D2COMMON_GetRealItem(curItem) == ptItem) return curStash;
			curItem = D2COMMON_GetNextItemFromInventoryHM(curItem);
		}
		curStash = curStash->nextStash;
	}
	return NULL;
}

void __fastcall updateItem_111(UnitAnyHM* ptItem, UnitAnyHM* ptChar)
{
	if (ptChar->ptGame->isLODGame && (D2COMMON_GetItemPage(ptItem) == 4))
	{
		Stash* ptStash = getStashFromItem(ptChar, ptItem);
		if (ptStash)
			selectStash(ptChar, ptStash, ptStash->id == 0);
	}
}

void __declspec(naked) caller_updateItem_111()
{
	__asm
	{
		MOV ECX, ESI
		MOV EDX, EBP
		CALL updateItem_111
		POP EAX
		MOV EDX, DWORD PTR SS : [ESP + 0x18]
		PUSH EDX
		JMP EAX
	}
}

void __declspec(naked) caller_updateItemB_111()
{
	__asm
	{
		MOV EDX, EBP
		CALL updateItem_111
		POP EAX
		MOV EDX, DWORD PTR SS : [ESP + 0x18]
		PUSH EDX
		JMP EAX
	}
}


void __stdcall updateClientPlayerOnLoading(UnitAnyHM* ptChar)
{
	LogMsg("--- Start updateClientPlayerOnLoading ---\n");
	if (ptChar->ptGame->isLODGame)
	{
		//PCPY49->showSharedStash = openSharedStashOnLoading && active_sharedStash;
		//selectStash(ptChar, PCPY->showSharedStash ? PCPY->sharedStash : PCPY->selfStash, true);
		selectStash(ptChar,PCPY49->selfStash, true);  //共享的暂时先去掉
	}
	updateClient(ptChar, UC_SHARED_GOLD, PCPY49->sharedGold, 0, 0);
	LogMsg("End update client on loading.\n\n");
}

void __declspec(naked) caller_updateClientPlayerOnLoading()
{
	__asm
	{
		PUSH DWORD PTR SS : [ESP + 0x14]
		CALL updateClientPlayerOnLoading
		POP ECX
		POP EDI
		POP ESI
		POP EBP
		XOR EAX, EAX
		JMP ECX
	}
}

/**************************** FREE CUSTOM DATA ****************************/


void freeStash(Stash* ptStash)
{
	if (!ptStash) return;
	freeStash(ptStash->nextStash);
	free(ptStash->nextStash);//	D2FogMemDeAlloc(ptStash->nextStash,__FILE__,__LINE__,0);
	if (ptStash->name)
	{
		free(ptStash->name);//D2FogMemDeAlloc(ptStash->name,__FILE__,__LINE__,0);
		ptStash->name = NULL;
	}
	ptStash->nextStash = NULL;
}

void __fastcall free_PlayerCustomData(DWORD p1, PlayerData* playerData, LPCSTR file, DWORD line, DWORD p5)
{
	LogMsg("free_PlayerCustomData\n");
	PYPlayerData* ptPYPlayerData = (PYPlayerData*)((DWORD)playerData + shifting.ptPYPlayerData);

	freeStash(ptPYPlayerData->selfStash);
	free(ptPYPlayerData->selfStash);//D2FogMemDeAlloc(ptPYPlayerData->selfStash,__FILE__,__LINE__,0);
	ptPYPlayerData->selfStash = NULL;

	freeStash(ptPYPlayerData->sharedStash);
	free(ptPYPlayerData->sharedStash);//D2FogMemDeAlloc(ptPYPlayerData->sharedStash,__FILE__,__LINE__,0);
	ptPYPlayerData->sharedStash = NULL;

	FOG_FreeMem(p1, playerData, file, line, p5);
}

UnitAnyHM* __stdcall getNextItemToFree(UnitAnyHM* ptChar, UnitAnyHM* ptItem)
{
	UnitAnyHM* item = D2COMMON_GetNextItemFromInventoryHM(ptItem);
	if (item) return item;

	if (ptChar->nUnitType != UNIT_PLAYER) return NULL;
	if (!ptChar->ptPlayerData) return NULL;
	if (!PCPY49) return NULL;

	Stash* curStash = PCPY49->selfStash;
	while (curStash)
	{
		if (curStash->ptListItem)
		{
			item = curStash->ptListItem;
			curStash->ptListItem = NULL;
			return item;//->nUnitType == 4 ? item : NULL;
		}
		curStash = curStash->nextStash;
	}

	curStash = PCPY49->sharedStash;
	while (curStash)
	{
		if (curStash->ptListItem)
		{
			item = curStash->ptListItem;
			curStash->ptListItem = NULL;
			return item->nUnitType == 4 ? item : NULL;
		}
		curStash = curStash->nextStash;
	}

	return NULL;
}

void __declspec(naked) callerServer_getNextItemToFree_111()
{
	__asm
	{
		PUSH DWORD PTR SS : [ESP + 4]
		PUSH DWORD PTR SS : [ESP + 0x30]
		CALL getNextItemToFree
		RETN 4
	}
}

void __declspec(naked) callerClient_getNextItemToFree_111()
{
	__asm
	{
		PUSH DWORD PTR SS : [ESP + 4]
		PUSH EBX
		CALL getNextItemToFree
		RETN 4
	}
}

void Install_PlayerCustomData() {
	Install_SavePlayerData();
	Install_LoadPlayerData();
	Install_UpdateClient();

	LogMsg("Install_PlayerCustomData 开始\n");
	//D2Common.dll offset 0x309BE
	//init_PlayerCustomData
	Patch* patch1 = new Patch(Call, D2COMMON, { 0x309BE-1,0x309BE-1 }, (DWORD)init_PlayerCustomData, 5);
	patch1->Install();

	//D2Game.dll offset 9BB91
	//caller_updateItem_111
	Patch* patch2 = new Patch(Call, D2GAME, { 0x9BB91,0x9BB91 }, (DWORD)caller_updateItem_111, 5);
	patch2->Install();

	//D2Game.dll offset 9BBF1
	//caller_updateItemB_111
	Patch* patch3 = new Patch(Call, D2GAME, { 0x9BBF1,0x9BBF1 }, (DWORD)caller_updateItemB_111, 5);
	patch3->Install();

	//D2Game.dll offset 4BF12
	//caller_updateClientPlayerOnLoading
	Patch* patch4 = new Patch(Call, D2GAME, { 0x4BF12,0x4BF12 }, (DWORD)caller_updateClientPlayerOnLoading, 5);
	patch4->Install();

	//D2Common.dll offset 3093D-1
	//free_PlayerCustomData
	Patch* patch5 = new Patch(Call, D2COMMON, { 0x3093D - 1,0x3093D - 1 }, (DWORD)free_PlayerCustomData, 5);
	patch5->Install();

	//D2Game.dll offset E1162-1
	//callerServer_getNextItemToFree_111
	Patch* patch6 = new Patch(Call, D2GAME, { 0xE1162 - 1,0xE1162 - 1 }, (DWORD)callerServer_getNextItemToFree_111, 5);
	patch6->Install();

	//D2Client.dll offset A5C94-1
	//callerClient_getNextItemToFree_111
	Patch* patch7 = new Patch(Call, D2CLIENT, { 0xA5C94 - 1,0xA5C94 - 1 }, (DWORD)callerClient_getNextItemToFree_111, 5);
	patch7->Install();

	//D2Common.dll offset 21B23
	int patch1Addr = Patch::GetDllOffset(D2COMMON, 0x21B23);
	BYTE patch1Bytes[2] = { 0x0D , 0x07 };
	Patch::WriteBytes(patch1Addr, 2, patch1Bytes);

	LogMsg("Install_PlayerCustomData 结束\n");
}