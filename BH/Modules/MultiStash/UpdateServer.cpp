#include "UpdateServer.h"
#include "PlayerCustomData.h"
#include "InfinityStash.h"
#include "../../D2Helpers.h"
#include "../../Patch.h"


int renameIndex = 0;
char renameString[21];
DWORD PageSwap;


void updateServer(WORD p)
{
	/*if (!onRealm)
		D2SendToServer3(0x3A, p);*/  //这个好像是单机？
}

void savePlayers(UnitAnyHM* ptChar)
{
	//if (active_savegame)
		//D2SaveGame(PCGame);  //这个先不弄吧
}

void maxGold(UnitAnyHM* ptChar)
{
	/*LogMsg("maxGold\n");

	DWORD maxGold = D2GetMaxGold(ptChar);
	DWORD maxGoldBank = D2GetMaxGoldBank(ptChar);
	DWORD playerGold = D2GetPlayerStat(ptChar, STATS_GOLD, 0);
	DWORD playerGoldBank = D2GetPlayerStat(ptChar, STATS_GOLDBANK, 0);
	if ((playerGold < maxGold) || (playerGoldBank < maxGoldBank)) {
		D2AddPlayerStat(ptChar, STATS_GOLD, maxGold - playerGold, 0);
		D2AddPlayerStat(ptChar, STATS_GOLDBANK, maxGoldBank - playerGoldBank, 0);
	}
	else {
		D2AddPlayerStat(ptChar, STATS_GOLD, 100000, 0);
	}
	if (active_sharedGold)
	{
		PCPY->sharedGold = 0xFFFFFFFF;
		updateClient(ptChar, UC_SHARED_GOLD, PCPY->sharedGold, 0, 0);
	}*/
}

int __stdcall handleServerUpdate(UnitAnyHM* ptChar, WORD param)
{
	int type = param & 0xFF;
	DWORD arg = (param & 0xFF00) >> 8;
	LogMsg("Received custom message: type=%i, arg=%i\n", type, arg);
	switch (type)
	{
	case US_UNASSIGN_STR_POINT:	/*UnassignStrPoint(ptChar);*/ return 1;  //这些先不要，应该是洗点相关的
	case US_UNASSIGN_ENE_POINT:	/*UnassignEnePoint(ptChar);*/ return 1;
	case US_UNASSIGN_DEX_POINT:	/*UnassignDexPoint(ptChar);*/ return 1;
	case US_UNASSIGN_VIT_POINT:	/*UnassignVitPoint(ptChar);*/ return 1;
	case US_UNASSIGN_STR_POINTS:	/*UnassignStrPoints(ptChar);*/ return 1;
	case US_UNASSIGN_ENE_POINTS:	/*UnassignEnePoints(ptChar);*/ return 1;
	case US_UNASSIGN_DEX_POINTS:	/*UnassignDexPoints(ptChar);*/ return 1;
	case US_UNASSIGN_VIT_POINTS:	/*UnassignVitPoints(ptChar);*/ return 1;
	case US_UNASSIGN_SKILLS:		/*UnassignAllSkillsPoints(ptChar);*/ return 1;

	case US_SET_INDEX:				setCurrentStashIndex(ptChar, 1); return 1;
	case US_SET_MAIN_INDEX:			setCurrentStashIndex(ptChar, 2); return 1;
	case US_RESET_INDEX:			setCurrentStashIndex(ptChar, 0); return 1;
	case US_SELECT_PREVIOUS:		selectPreviousStash(ptChar); return 1;
	case US_SELECT_NEXT:			selectNextStash(ptChar); return 1;
	case US_SELECT_SELF:			/*if (active_sharedStash) toggleToSelfStash(ptChar);*/ return 1;
	case US_SELECT_SHARED:			/*if (active_sharedStash) toggleToSharedStash(ptChar);*/ return 1;
	case US_SELECT_PREVIOUS_INDEX:	selectPreviousIndexStash(ptChar); return 1;
	case US_SELECT_NEXT_INDEX:		selectNextIndexStash(ptChar); return 1;
	case US_SELECT_PREVIOUS2:		selectPrevious2Stash(ptChar); return 1;
	case US_SELECT_NEXT2:			selectNext2Stash(ptChar); return 1;
	case US_SELECT_PREVIOUS_INDEX2:	selectPreviousIndex2Stash(ptChar); return 1;
	case US_SELECT_NEXT_INDEX2:	selectNextIndex2Stash(ptChar); return 1;
	case US_INSERT_PAGE:			insertStash(ptChar); selectNextStash(ptChar); return 1;
	case US_DELETE_PAGE:			deleteStash(ptChar, false); return 1;

	case US_SAVE:					savePlayers(ptChar); return 1;

	case US_MAXGOLD:				maxGold(ptChar); return 1;
	case US_PUTGOLD:				/*putGold(ptChar, 0);*/ return 1;
	case US_TAKEGOLD:				/*takeGold(ptChar, 0);*/ return 1;
	case US_SWAP3:					PageSwap = arg << 24; return 1;
	case US_SWAP2:					PageSwap |= arg << 16; return 1;
	case US_SWAP1:					PageSwap |= arg << 8; return 1;
	case US_SWAP0:					swapStash(ptChar, PageSwap | arg, false); PageSwap = 0; return 1;
	case US_SWAP0_TOGGLE:			swapStash(ptChar, PageSwap | arg, true); PageSwap = 0; return 1;
	case US_RENAME:  //重命名的也不要
		//if (renameIndex == 0)
		//	ZeroMemory(renameString, sizeof(renameString));
		//if (arg != NULL && renameIndex < 15)
		//	renameString[renameIndex++] = (char)arg;
		//if (arg == NULL)
		//{
		//	renameString[renameIndex] = NULL;
		//	renameIndex = 0;
		//	LogMsg("Rename on Server : %s -> %s\n", PCPlayerData->name, renameString);
		//	strcpy(PCPlayerData->name, renameString);
		//	strcpy(PCPlayerData->ptNetClient->name, renameString);
		//}
		return 1;
	case US_PAGENAME:
		if (renameIndex == 0)
			ZeroMemory(renameString, sizeof(renameString));
		if (arg != NULL && renameIndex < 20)
			renameString[renameIndex++] = (char)arg;
		if (arg == NULL)
		{
			renameString[renameIndex] = NULL;
			renameIndex = 0;
			LogMsg("Rename current page on Server : %s -> %s\n", PCPY49->currentStash->name, renameString);
			renameCurrentStash(ptChar, renameString);
		}
		return 1;
	default:
		return 0;
	}
}


void __declspec(naked) caller_handleServerUpdate()
{
	__asm
	{
		PUSH ESI
		PUSH EBX
		CALL handleServerUpdate
		TEST EAX, EAX
		JNZ END_RCM
		MOV EAX, ESI
		AND EAX, 0xFF
		SHR ESI, 8
		MOV EDI, EAX
		RETN
	END_RCM :
		ADD ESP, 4
		POP EDI
		POP ESI
		XOR EAX, EAX
		POP EBX
		RETN 8
	}
}

void Install_UpdateServer()
{
	LogMsg("Install_UpdateServer 开始\n");

	//D2Game.dll offset CC983
	//caller_handleServerUpdate
	int patch1Addr = Patch::GetDllOffset(D2GAME, 0xCC983);
	BYTE patch1Bytes[1] = { 0x57  };
	Patch::WriteBytes(patch1Addr, 1, patch1Bytes);
	Patch* patch1 = new Patch(Call, D2GAME, { 0xCC983 + 1,0xCC983 + 1 }, (DWORD)caller_handleServerUpdate, 5);
	patch1->Install();
	
	LogMsg("Install_UpdateServer 结束\n");
}
