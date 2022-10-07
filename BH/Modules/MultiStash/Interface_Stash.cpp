#include "Interface_Stash.h"
#include "UpdateServer.h"
#include "PlayerCustomData.h"
#include "PlugYFiles.h"
#include "InfinityStash.h"
#include "../../D2Helpers.h"
#include "../../Patch.h"
#include "../../D2Ptrs.h"
#include "../../D2Structs.h"
#include "../../Constants.h"

static struct
{
	union {
		DWORD all;
		struct {
			DWORD previous : 1;
			DWORD next : 1;
			DWORD toggleToSharedStash : 1;
			DWORD previousIndex : 1;
			DWORD nextIndex : 1;
			DWORD putGold : 1;
			DWORD takeGold : 1;
			DWORD stashName : 1;
		};
	};
} isDownBtn;

int posXPreviousBtn = -1;
int posYPreviousBtn = -1;
int posWPreviousBtn = 32;
int posHPreviousBtn = 32;
int posXNextBtn = -1;
int posYNextBtn = -1;
int posWNextBtn = 32;
int posHNextBtn = 32;
int posXSharedBtn = -1;
int posYSharedBtn = -1;
int posWSharedBtn = 32;
int posHSharedBtn = 32;
int posXPreviousIndexBtn = -1;
int posYPreviousIndexBtn = -1;
int posWPreviousIndexBtn = 32;
int posHPreviousIndexBtn = 32;
int posXNextIndexBtn = -1;
int posYNextIndexBtn = -1;
int posWNextIndexBtn = 32;
int posHNextIndexBtn = 32;
int posXPutGoldBtn = -1;
int posYPutGoldBtn = -1;
int posWPutGoldBtn = 32;
int posHPutGoldBtn = 32;
int posXTakeGoldBtn = -1;
int posYTakeGoldBtn = -1;
int posWTakeGoldBtn = 32;
int posHTakeGoldBtn = 32;
int posXStashNameField = -1;
int posYStashNameField = -1;
int posWStashNameField = 175;
int posHStashNameField = 20;
int posXStashGoldField = -1;
int posYStashGoldField = -1;
int posWStashGoldField = 152;
int posHStashGoldField = 18;

DWORD PersonalNormalPageColor = White;
DWORD PersonalIndexPageColor = DarkGreen;
DWORD PersonalMainIndexPageColor = Yellow;
DWORD SharedNormalPageColor = Gold;
DWORD SharedIndexPageColor = Orange;
DWORD SharedMainIndexPageColor = Red;

//这里直接自定义位置算了
DWORD	getXPreviousBtn() { return *p_D2CLIENT_PanelOffsetX + posXPreviousBtn; }
DWORD	getYPreviousBtn() { return *p_D2CLIENT_ScreenSizeY - posYPreviousBtn; }
DWORD	getXNextBtn() { return *p_D2CLIENT_PanelOffsetX + posXNextBtn; }//?169:SEL_X(0x63, 0x63, 0xCF, 0xA0));}
DWORD	getYNextBtn() { return  *p_D2CLIENT_ScreenSizeY - posYNextBtn; }
DWORD	getXPreviousIndexBtn() { return *p_D2CLIENT_PanelOffsetX + posXPreviousIndexBtn; }//73:SEL_X(0x18, 0x60, 0x8F, 0x60));}
DWORD	getYPreviousIndexBtn() { return *p_D2CLIENT_ScreenSizeY - posYPreviousIndexBtn; }
DWORD	getXNextIndexBtn() { return *p_D2CLIENT_PanelOffsetX + posXNextIndexBtn; }//217:SEL_X(0x128, 0xC0, 0xEF, 0xC0));}
DWORD	getYNextIndexBtn() { return *p_D2CLIENT_ScreenSizeY - posYNextIndexBtn; }
DWORD	getXStashNameField() { return *p_D2CLIENT_PanelOffsetX + posXStashNameField; }
DWORD	getYStashNameField() { return *p_D2CLIENT_ScreenSizeY - posYStashNameField; }
DWORD	getXStashGoldField() { return *p_D2CLIENT_PanelOffsetX + posXStashGoldField; }
DWORD	getYStashGoldField() { return *p_D2CLIENT_ScreenSizeY - posYStashGoldField; }

bool isOnRect(DWORD x, DWORD y, DWORD x0, DWORD y0, DWORD l, DWORD h)
{
	return (x >= x0) && (x < x0 + l) && (y > y0 - h) && (y <= y0);
}

#define isOnButtonNextStash(x,y) isOnRect(x, y, getXNextBtn(), getYNextBtn(), posWNextBtn, posHNextBtn)
#define isOnButtonPreviousStash(x,y) isOnRect(x, y, getXPreviousBtn(), getYPreviousBtn(), posWPreviousBtn, posHPreviousBtn)
//#define isOnButtonToggleSharedStash(x,y) isOnRect(x, y, getXSharedBtn(), getYSharedBtn(), posWSharedBtn, posHSharedBtn)
#define isOnButtonNextIndexStash(x,y) isOnRect(x, y, getXNextIndexBtn(), getYNextIndexBtn(), posWNextIndexBtn, posHNextIndexBtn)
#define isOnButtonPreviousIndexStash(x,y) isOnRect(x, y, getXPreviousIndexBtn(), getYPreviousIndexBtn(), posWPreviousIndexBtn, posHPreviousIndexBtn)
//#define isOnButtonPutGold(x,y) isOnRect(x, y, getXPutGoldBtn(), getYPutGoldBtn(), posWPutGoldBtn, posHPutGoldBtn)
//#define isOnButtonTakeGold(x,y) isOnRect(x, y, getXTakeGoldBtn(), getYTakeGoldBtn(), posWTakeGoldBtn, posHTakeGoldBtn)
#define isOnStashNameField(x,y) isOnRect(x, y, getXStashNameField(), getYStashNameField(), posWStashNameField, posHStashNameField)
#define isOnStashGoldField(x,y) isOnRect(x, y, getXStashGoldField(), getYStashGoldField(), posWStashGoldField, posHStashGoldField)

void* __stdcall printBtns()
{
	//onRealm用于判断是否在战网
	//if (onRealm || !D2isLODGame()) return D2LoadBuySelBtn();

	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();

	if(!ptChar->ptGame->isLODGame)  return D2CLIENT_LoadBuySelBtn();

	sDrawImageInfo data;
	ZeroMemory(&data, sizeof(data));
	setImage(&data, stashBtnsImages);

	setFrame(&data, 0 + isDownBtn.previous);
	D2GFX_PrintImage(&data, getXPreviousBtn(), getYPreviousBtn(), -1, 5, 0);

	setFrame(&data, 2 + isDownBtn.next);
	D2GFX_PrintImage(&data, getXNextBtn(), getYNextBtn(), -1, 5, 0);

	//if (active_sharedStash)
	//{
	//	setFrame(&data, 4 + isDownBtn.toggleToSharedStash + (PCPY->showSharedStash ? 2 : 0));
	//	D2PrintImage(&data, getXSharedBtn(), getYSharedBtn(), -1, 5, 0);
	//}

	setFrame(&data, 8 + isDownBtn.previousIndex);
	D2GFX_PrintImage(&data, getXPreviousIndexBtn(), getYPreviousIndexBtn(), -1, 5, 0);

	setFrame(&data, 10 + isDownBtn.nextIndex);
	D2GFX_PrintImage(&data, getXNextIndexBtn(), getYNextIndexBtn(), -1, 5, 0);

	//if (active_sharedGold)
	//{
	//	setImage(&data, sharedGoldBtnsImages);
	//	setFrame(&data, 0 + isDownBtn.putGold);
	//	D2PrintImage(&data, getXPutGoldBtn(), getYPutGoldBtn(), -1, 5, 0);

	//	setFrame(&data, 2 + isDownBtn.takeGold);
	//	D2PrintImage(&data, getXTakeGoldBtn(), getYTakeGoldBtn(), -1, 5, 0);
	//}


	LPWSTR lpText;
	DWORD mx = *p_D2CLIENT_MouseX;
	DWORD my = *p_D2CLIENT_MouseY;

	D2WIN_SetTextSize(1);

	if (isOnButtonPreviousStash(mx, my)) {
		lpText = L"上一页";   //这里先不做多语言功能
		D2WIN_PrintPopup(lpText, getXPreviousBtn() + posWPreviousBtn / 2, getYPreviousBtn() - posHPreviousBtn, White, 1);

	}
	else if (isOnButtonNextStash(mx, my)) {
		lpText = L"下一页";
		D2WIN_PrintPopup(lpText, getXNextBtn() + posWNextBtn / 2, getYNextBtn() - posHNextBtn, White, 1);

	}
	//else if (isOnButtonToggleSharedStash(mx, my)) {
	//	if (active_sharedStash) {
	//		lpText = getLocalString(PCPY->showSharedStash ? STR_TOGGLE_TO_PERSONAL : STR_TOGGLE_TO_SHARED);
	//		D2WIN_PrintPopup(lpText, getXSharedBtn() + posWSharedBtn / 2, getYSharedBtn() - posHSharedBtn, White, 1);
	//	}
	//	else if (active_SharedStashInMultiPlayer == 1) {
	//		lpText = getLocalString(STR_TOGGLE_MULTI_DISABLED);
	//		D2WIN_PrintPopup(lpText, getXPreviousIndexBtn() + posWPreviousIndexBtn / 2, getYPreviousIndexBtn() - posHPreviousIndexBtn, White, 1);
	//	}

	//}
	else if (isOnButtonPreviousIndexStash(mx, my)) {
		lpText = L"上10页";
		D2WIN_PrintPopup(lpText, getXPreviousIndexBtn() + posWPreviousIndexBtn / 2, getYPreviousIndexBtn() - posHPreviousIndexBtn, White, 1);

	}
	else if (isOnButtonNextIndexStash(mx, my)) {
		lpText = L"下10页";
		D2WIN_PrintPopup(lpText, getXNextIndexBtn() + posWNextIndexBtn / 2, getYNextIndexBtn() - posHNextIndexBtn, White, 1);

	}
	//else if (active_sharedGold && isOnButtonPutGold(mx, my)) {
	//	lpText = getLocalString(STR_PUT_GOLD);
	//	D2WIN_PrintPopup(lpText, getXPutGoldBtn() + posWPutGoldBtn / 2, getYPutGoldBtn() - posHPutGoldBtn, White, 1);

	//}
	//else if (active_sharedGold && isOnButtonTakeGold(mx, my)) {
	//	lpText = getLocalString(STR_TAKE_GOLD);
	//	D2WIN_PrintPopup(lpText, getXTakeGoldBtn() + posWTakeGoldBtn / 2, getYTakeGoldBtn() - posHTakeGoldBtn, White, 1);
	//}

	return D2CLIENT_LoadBuySelBtn();
}

void __fastcall printPageNumber(LPWSTR maxGoldText, DWORD x, DWORD y, DWORD color, DWORD bfalse)
{
	//if (onRealm || !D2isLODGame())
	//{
	//	D2PrintString(maxGoldText, x, y, color, bfalse);
	//	return;
	//}

	DWORD mx = *p_D2CLIENT_MouseX;
	DWORD my = *p_D2CLIENT_MouseY;
	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();
	WCHAR popupText[0x400];

	if (!PCPY49->currentStash)
		D2WIN_DrawText(L"没有所选页面", x, y, White, bfalse);
	else
	{
		bool isShared = PCPY49->currentStash->isShared;
		bool isIndex = PCPY49->currentStash->isIndex;
		bool isMainIndex = PCPY49->currentStash->isMainIndex;
		DWORD currentId = PCPY49->currentStash->id + 1;

		WCHAR text[21];
		getCurrentStashName(text, 21, ptChar);

		// Replace character # by number page.
		int j = 0;
		for (int i = 0; text[i]; i++)
		{
			if (text[i] == L'#')
			{
				DWORD val = currentId;
				int nbDigits = 0;
				do
				{
					nbDigits++;
					val /= 10;
				} while (val);
				j += nbDigits;

				val = currentId;
				nbDigits = 0;
				do
				{
					nbDigits++;
					popupText[j - nbDigits] = (WCHAR)(val % 10 + 48);
					val /= 10;
				} while (val);
			}
			else
				popupText[j++] = text[i];
		}
		popupText[j] = NULL;

		// Check text length vs field name length
		int pixelLen = D2WIN_GetPixelLen(popupText);
		int len = wcslen(popupText);
		while (len > 0 && pixelLen > posWStashNameField - 5)
		{
			len--;
			popupText[len] = NULL;
			pixelLen = D2WIN_GetPixelLen(popupText);
		}

		DWORD color = isShared ? (isMainIndex ? SharedMainIndexPageColor : isIndex ? SharedIndexPageColor : SharedNormalPageColor) : (isMainIndex ? PersonalMainIndexPageColor : isIndex ? PersonalIndexPageColor : PersonalNormalPageColor);
		D2WIN_DrawText(popupText, x, y, color, bfalse);

		if (isOnStashNameField(mx, my))
		{
			_snwprintf(popupText, 0x400, L"指令 /tp X : 以对侧储藏箱之第 X 页替换当前页。\n指令 /sp X : 以本储藏箱之第 X 页替换当前页。\n指令 /dp : 删除本空页。\n指令 /ip : 在当前页后插入空页。\nShift + 点选 : 重新命名当前页 (%d)。\n点选 : 变更页面类型(普通页, 标签页, 主标签页)", currentId);
			D2WIN_PrintPopup(popupText, getXStashNameField() + posWStashNameField / 2, getYStashNameField() - posHStashNameField - 2, White, 1);
		}
	}

	//printGoldMaxPopup
	if (isOnStashGoldField(mx, my))
	{
		/*if (active_sharedGold)
		{
			_snwprintf(popupText, 0x400, L"%s\n%s: %u", maxGoldText, getLocalString(STR_SHARED_GOLD_QUANTITY), PCPY49->sharedGold);
			D2WIN_PrintPopup(popupText, getXStashGoldField() + posWStashGoldField / 2, getYStashGoldField() - posHStashGoldField - 2, White, 1);
		}
		else*/
			D2WIN_PrintPopup(maxGoldText, getXStashGoldField() + posWStashGoldField / 2, getYStashGoldField() - posHStashGoldField - 2, White, 1);
	}
}

DWORD __stdcall manageBtnDown(sWinMessage* msg)
{
	//if (onRealm || !D2isLODGame()) return 0;  //在线也可以吧？

	if (isOnButtonPreviousStash(msg->x, msg->y))
		isDownBtn.previous = 1;
	else if (isOnButtonNextStash(msg->x, msg->y))
		isDownBtn.next = 1;
	//else if (active_sharedStash && isOnButtonToggleSharedStash(msg->x, msg->y))
	//	isDownBtn.toggleToSharedStash = 1;
	else if (isOnButtonPreviousIndexStash(msg->x, msg->y))
		isDownBtn.previousIndex = 1;
	else if (isOnButtonNextIndexStash(msg->x, msg->y))
		isDownBtn.nextIndex = 1;
	//else if (active_sharedGold && isOnButtonPutGold(msg->x, msg->y))
	//	isDownBtn.putGold = 1;
	//else if (active_sharedGold && isOnButtonTakeGold(msg->x, msg->y))
	//	isDownBtn.takeGold = 1;
	else if (isOnStashNameField(msg->x, msg->y))
		isDownBtn.stashName = 1;
	else return 0;

	//D2PlaySound(4, 0, 0, 0, 0);  //播放声音也先不要吧，有空再弄
	freeMessage(msg);
	return 1;
}

void __declspec(naked) caller_manageBtnDown_111()
{
	__asm
	{
		PUSH EBP
		CALL manageBtnDown
		TEST EAX, EAX
		JE IS_NOT_ON_BUTTON
		POP EDX
		MOV EDX, DWORD PTR DS : [EDX + 0x10]
		MOV DWORD PTR DS : [EDX] , 1
		POP EDI
		POP ESI
		POP EBP
		POP EBX
		RETN 4
	IS_NOT_ON_BUTTON :
		JMP D2CLIENT_ClickOnStashButton
	}
}

DWORD __stdcall manageBtnUp(sWinMessage* msg)
{
	//if (onRealm || !D2isLODGame()) return 0;

	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();

	if (isOnButtonPreviousStash(msg->x, msg->y)) {
		LogMsg("push up left button previous\n");
		if (isDownBtn.previous)
			if (GetKeyState(VK_SHIFT) < 0)
				updateServer(US_SELECT_PREVIOUS2);
			else
				updateServer(US_SELECT_PREVIOUS);

	}
	else if (isOnButtonNextStash(msg->x, msg->y)) {
		LogMsg("push up left button next\n");
		if (isDownBtn.next)
			if (GetKeyState(VK_SHIFT) < 0)
				updateServer(US_SELECT_NEXT2);
			else
				updateServer(US_SELECT_NEXT);

	}
	//else if (active_sharedStash && isOnButtonToggleSharedStash(msg->x, msg->y)) {
	//	LogMsg("push up left button shared\n");
	//	if (isDownBtn.toggleToSharedStash)
	//		if (PCPY->showSharedStash)
	//			updateServer(US_SELECT_SELF);
	//		else
	//			updateServer(US_SELECT_SHARED);

	//}
	else if (isOnButtonPreviousIndexStash(msg->x, msg->y)) {
		LogMsg("select left button previous index\n");
		if (isDownBtn.previousIndex)
			if (GetKeyState(VK_SHIFT) < 0)
				updateServer(US_SELECT_PREVIOUS_INDEX2);
			else
				updateServer(US_SELECT_PREVIOUS_INDEX);

	}
	else if (isOnButtonNextIndexStash(msg->x, msg->y)) {
		LogMsg("push up left button next index\n");
		if (isDownBtn.nextIndex)
			if (GetKeyState(VK_SHIFT) < 0)
				updateServer(US_SELECT_NEXT_INDEX2);
			else
				updateServer(US_SELECT_NEXT_INDEX);

	}
	//else if (active_sharedGold && isOnButtonPutGold(msg->x, msg->y)) {
	//	LogMsg("push up left put gold\n");
	//	if (isDownBtn.putGold)
	//		updateServer(US_PUTGOLD);

	//}
	//else if (active_sharedGold && isOnButtonTakeGold(msg->x, msg->y)) {
	//	LogMsg("push up left take gold\n");
	//	if (isDownBtn.takeGold)
	//		updateServer(US_TAKEGOLD);

	//}
	//else if (isOnStashNameField(msg->x, msg->y)) {
	//	LogMsg("push up left Stash Name\n");
	//	if (isDownBtn.stashName && PCPY->currentStash)
	//	{
	//		if (GetKeyState(VK_SHIFT) < 0)
	//		{
	//			WCHAR text[21];
	//			getCurrentStashName(text, 21, ptChar);
	//			D2TogglePage(5, 0, 0);// Open command window
	//			_snwprintf(ptInputCommand, 25, L"/rp %s", text);
	//			InputCommandLen = wcslen(ptInputCommand);
	//		}
	//		else if (PCPY->currentStash->isMainIndex)
	//			updateServer(US_RESET_INDEX);
	//		else if (PCPY->currentStash->isIndex)
	//			updateServer(US_SET_MAIN_INDEX);
	//		else
	//			updateServer(US_SET_INDEX);
	//	}

	//}
	else return 0;

	return 1;
}

void __declspec(naked) caller_manageBtnUp_111()
{
	__asm
	{
		PUSH EBX
		CALL manageBtnUp
		MOV isDownBtn.all, 0
		TEST EAX, EAX
		JE IS_NOT_ON_BUTTON
		POP EDX
		MOV EDX, DWORD PTR DS : [EDX + 0x1A]
		MOV DWORD PTR DS : [EDX] , 0
		SUB EDX, 8
		MOV DWORD PTR DS : [EDX] , 0
		ADD EDX, 4
		MOV DWORD PTR DS : [EDX] , 0
		ADD EDX, 0x68
		MOV DWORD PTR DS : [EDX] , 0
		POP EDI
		POP ESI
		POP EBP
		POP EBX
		POP ECX
		RETN 4
	IS_NOT_ON_BUTTON:
		JMP D2CLIENT_ClickOnStashButton
	}
}

void __declspec(naked) initBtnsStates()
{
	__asm
	{
		MOV isDownBtn.all, 0
		ADD ESP, 0x104
		RETN
	}
}

static Stash* curStash = NULL;
static DWORD currentSawStash = 0;
UnitAnyHM* __stdcall getNextItemForSet(UnitAnyHM* ptItem)
{
	UnitAnyHM* item = ptItem ? D2COMMON_GetNextItemFromInventoryHM(ptItem) : NULL;
	if (item) return item;

	if (!curStash)
	{
		UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();
		switch (currentSawStash)
		{
		case 0: curStash = PCPY49->selfStash;
			//currentSawStash = displaySharedSetItemNameInGreen ? 1 : 2;
			currentSawStash = 1;
			break;
		case 1: curStash = PCPY49->sharedStash;
			currentSawStash = 2;
			break;
		default:return NULL;
		}
	}
	else {
		curStash = curStash->nextStash;
	}
	if (curStash)
	{
		item = curStash->ptListItem;
		if (item) return item;
	}
	return getNextItemForSet(item);
}

UnitAnyHM* __stdcall initGetNextItemForSet(Inventory* ptInventory)
{
	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();
	if (ptChar->nUnitType != UNIT_PLAYER) return NULL;
	if (!PCPY49) return NULL;

	curStash = NULL;
	currentSawStash = 0;
	UnitAnyHM* item = D2COMMON_GetItemFromInventoryHM(ptChar->ptInventory);
	if (item) return item;
	return getNextItemForSet(item);
}

void Install_InterfaceStash()
{
	Install_UpdateServer();
	Install_PlugYImagesFiles();  //这个是用来加载自定义图片的

	LogMsg("Install_InterfaceStash 开始\n");

	//D2Client.dll offset 99A56-1
	//printBtns
	Patch* patch1 = new Patch(Call, D2CLIENT, { 0x99A56 - 1,0x99A56 - 1 }, (DWORD)printBtns, 5);
	patch1->Install();

	//D2Client.dll offset 99A33-1
	//printPageNumber
	Patch* patch2 = new Patch(Call, D2CLIENT, { 0x99A33 - 1,0x99A33 - 1 }, (DWORD)printPageNumber, 5);
	patch2->Install();

	//D2Client.dll offset 9B8A6-1
	//caller_manageBtnDown_111
	Patch* patch3 = new Patch(Call, D2CLIENT, { 0x9B8A6 - 1,0x9B8A6 - 1 }, (DWORD)caller_manageBtnDown_111, 5);
	patch3->Install();

	//D2Client.dll offset 9B6D9-1
	//caller_manageBtnUp_111
	Patch* patch4 = new Patch(Call, D2CLIENT, { 0x9B6D9 - 1,0x9B6D9 - 1 }, (DWORD)caller_manageBtnUp_111, 5);
	patch4->Install();

	//D2Client.dll offset 8CC3A-1 6
	//initBtnsStates
	Patch* patch5 = new Patch(Call, D2CLIENT, { 0x8CC3A - 1,0x8CC3A - 1 }, (DWORD)initBtnsStates, 6);
	patch5->Install();

	//D2Client.dll offset 8D434-1
	//initGetNextItemForSet
	Patch* patch6 = new Patch(Call, D2CLIENT, { 0x8D434 - 1,0x8D434 - 1 }, (DWORD)initGetNextItemForSet, 5);
	patch6->Install();

	//D2Client.dll offset 8D4CB-1
	//getNextItemForSet
	Patch* patch7 = new Patch(Call, D2CLIENT, { 0x8D4CB - 1,0x8D4CB - 1 }, (DWORD)getNextItemForSet, 5);
	patch7->Install();
	
	LogMsg("Install_InterfaceStash 结束\n");
}
