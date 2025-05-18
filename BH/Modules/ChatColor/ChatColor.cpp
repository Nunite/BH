#include "ChatColor.h"
#include "../ScreenInfo/ScreenInfo.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"
#include "../../D2Intercepts.h"
#include "../../D2Helpers.h"
#include "../ItemMover/ItemMover.h"
#include "../Item/Item.h"


#define INVENTORY_WIDTH  p_D2CLIENT_InventoryLayout->SlotWidth
#define INVENTORY_HEIGHT p_D2CLIENT_InventoryLayout->SlotHeight
#define INVENTORY_LEFT   p_D2CLIENT_InventoryLayout->Left
#define INVENTORY_RIGHT  p_D2CLIENT_InventoryLayout->Right
#define INVENTORY_TOP    p_D2CLIENT_InventoryLayout->Top
#define INVENTORY_BOTTOM p_D2CLIENT_InventoryLayout->Bottom

//�����Ȳ��ܷ�����Ƭ��stash
#define STASH_WIDTH  p_D2CLIENT_StashLayout->SlotWidth
#define STASH_HEIGHT p_D2CLIENT_StashLayout->SlotHeight
#define STASH_LEFT   p_D2CLIENT_StashLayout->Left
#define STASH_RIGHT  p_D2CLIENT_StashLayout->Right
#define STASH_TOP    p_D2CLIENT_StashLayout->Top
#define STASH_BOTTOM p_D2CLIENT_StashLayout->Bottom

#define CUBE_WIDTH  p_D2CLIENT_CubeLayout->SlotWidth
#define CUBE_HEIGHT p_D2CLIENT_CubeLayout->SlotHeight
#define CUBE_LEFT   p_D2CLIENT_CubeLayout->Left
#define CUBE_RIGHT  p_D2CLIENT_CubeLayout->Right
#define CUBE_TOP    p_D2CLIENT_CubeLayout->Top
#define CUBE_BOTTOM p_D2CLIENT_CubeLayout->Bottom

#define CELL_SIZE p_D2CLIENT_InventoryLayout->SlotPixelHeight

using namespace Drawing;  //�����Hook��namespace
 
D2EditBox* ChatColor::pD2WinEditBox;
DWORD ChatColor::dwPlayerId=0;
StatMonitor ChatColor::sMonitorStr[200] = {L'\0'};
std::map<std::string, Toggle> ChatColor::Toggles;

//��������������Ĳ���
Patch* InputLine1 = new Patch(Call, D2CLIENT, { 0x70C6C, 0xB254C }, (DWORD)InputLinePatch1_ASM, 5);
//{PatchCALL, DLLOFFSET2(D2CLIENT, 0x6FB20C6C, 0x6FB6254C), (DWORD)InputLinePatch1_ASM, 5, & fDefault},
Patch* InputLine2 = new Patch(Call, D2CLIENT, { 0x7037B, 0xB286B }, (DWORD)InputLinePatch2_ASM, 5);
//{ PatchCALL,   DLLOFFSET2(D2CLIENT, 0x6FB2037B, 0x6FB6186B),    (DWORD)InputLinePatch2_ASM,             5 ,   &fDefault },
Patch* UnicodeSupport1 = new Patch(Jump, D2LANG, { 0x82F0, 0x82B0 }, (DWORD)D2Lang_Unicode2WinPatch, 5);
//{PatchJMP, DLLOFFSET2(D2LANG, 0x6FC082F0, 0x6FC082B0), (DWORD)D2Lang_Unicode2WinPatch, 5, & fLocalizationSupport},
Patch* UnicodeSupport2 = new Patch(Jump, D2LANG, { 0x8320, 0x82E0 }, (DWORD)D2Lang_Win2UnicodePatch, 5);
//{PatchJMP,    DLLOFFSET2(D2LANG, 0x6FC08320, 0x6FC082E0),      (DWORD)D2Lang_Win2UnicodePatch,          5 ,   &fLocalizationSupport},
Patch* UnicodeSupport3 = new Patch(Call, D2WIN, { 0x183A0, 0xE850 }, (DWORD)ChannelEnterCharPatch, 5);
//{PatchCALL, DLLOFFSET2(D2WIN, 0x6F8F83A0, 0x6F8EE850), (DWORD)ChannelEnterCharPatch, 5, & fLocalizationSupport}, //ע��InputLinePatch1_ASM�������������Ȼ�����𲿷ֶ�ջ����(��Ȼ��Ӱ��ʹ��)
Patch* UnicodeSupport4 = new Patch(Call, BNCLIENT, { 0xFF5C, 0x1513C }, (DWORD)MultiByteFixPatch_ASM, 6);
//{ PatchCALL,   DLLOFFSET2(BNCLIENT,0x6FF2FF5C, 0x6FF3513C),     (DWORD)MultiByteFixPatch_ASM,            6 ,   &fLocalizationSupport }, // for /w *acc msg text
//�������⻷״̬���Ժ��ٷֿ����ɡ��ȶ�д��ChatColor.cpp����
Patch* Monitor1 = new Patch(Call, D2CLIENT, { 0xADEB1, 0xB254C }, (DWORD)RecvCommand_A7_Patch_ASM, 9);
//{PatchCALL, DLLOFFSET2(D2CLIENT, 0x6FB5DEB1, 0x6FB34391), (DWORD)RecvCommand_A7_Patch_ASM, 9, & fDefault},//����״̬1
Patch* Monitor2 = new Patch(Call, D2CLIENT, { 0xADD74, 0xB254C }, (DWORD)RecvCommand_A8_Patch_ASM, 9);
//{ PatchCALL,   DLLOFFSET2(D2CLIENT , 0x6FB5DD74, 0x6FB34254),   (DWORD)RecvCommand_A8_Patch_ASM,        9 ,   &fDefault },//����״̬2
Patch* Monitor3 = new Patch(Call, D2CLIENT, { 0xADD31, 0xB254C }, (DWORD)RecvCommand_A9_Patch_ASM, 9);
//{ PatchCALL,   DLLOFFSET2(D2CLIENT , 0x6FB5DD31, 0x6FB34211),   (DWORD)RecvCommand_A9_Patch_ASM,        9 ,   &fDefault },//����״̬
//��Ѫ����
Patch* showLifeMana1 = new Patch(Call, D2CLIENT, { 0x276B5, 0xB254C }, (DWORD)ShowLifePatch_ASM, 5);
//{PatchCALL, DLLOFFSET2(D2CLIENT, 0x6FAD76B5, 0x6FB1D765), (DWORD)ShowLifePatch_ASM, 5, & fDefault},
Patch* showLifeMana2 = new Patch(Call, D2CLIENT, { 0x27767, 0xB254C }, (DWORD)ShowManaPatch_ASM, 5);
//{ PatchCALL,   DLLOFFSET2(D2CLIENT, 0x6FAD7767, 0x6FB1D817),    (DWORD)ShowManaPatch_ASM,           5 ,   &fDefault },
//Ӷ��/����ͷ��ȼ���ʾ���л��������ʾ
Patch* petHead = new Patch(Call, D2CLIENT, { 0x5B582, 0xB254C }, (DWORD)DrawPetHeadPath_ASM, 7);
//{PatchCALL, DLLOFFSET2(D2CLIENT, 0x6FB0B582, 0x6FB39662), (DWORD)DrawPetHeadPath_ASM, 7, & fDefault},
Patch* partyHead = new Patch(Call, D2CLIENT, { 0x5BBE0, 0xB254C }, (DWORD)DrawPartyHeadPath_ASM, 6);
//{ PatchCALL,   DLLOFFSET2(D2CLIENT, 0x6FB0BBE0, 0x6FB39F90),    (DWORD)DrawPartyHeadPath_ASM,          6 ,   &fDefault },
// ��HM��ֲ���߻���ֵ
Patch* DBase = new Patch(Call, D2CLIENT, { 0x8FDAD, 0x8FDAD }, (DWORD)StrcatDefenseStringPatch_ASM, 6);
//{PatchCALL, DLLOFFSET2(D2CLIENT, 0x6FB3FDAD, 0x6FB422FD), (DWORD)StrcatDefenseStringPatch_ASM, 6, & fDefault},//ƴ����ֵ�ַ���

void ChatColor::Init() {
	BH::inGameOnce = false;
}

void Print(DWORD color, char* format, ...) {
	va_list vaArgs;
	va_start(vaArgs, format);
	int len = _vscprintf(format, vaArgs) + 1;
	char* str = new char[len];
	vsprintf_s(str, len, format, vaArgs);
	va_end(vaArgs);

	wchar_t* wstr = new wchar_t[len];
	MultiByteToWideChar(CODE_PAGE, 0, str, -1, wstr, len);

	//by zyl
	for (DWORD i = 0; i < wcslen(wstr); i++)
	{
		if ((wstr[i] >= 0xFF || wstr[i] == 0x79) && wstr[i + 1] == L'c')
		{
			//if (name[i + 2] >= L'0' && name[i + 2] <= L':')
			//{
			wstr[i] = L'\377';
			//}
		};
	}

	D2CLIENT_PrintGameString(wstr, color);
	delete[] wstr;

	delete[] str;
}

DWORD gameTimer;
DWORD startExperience;
int startLevel;
unsigned int MercProtectSec;  //Ӷ���������

void ChatColor::OnGameJoin() {
	inGame = true;
	gameTimer = GetTickCount();
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	startExperience = (int)D2COMMON_GetUnitStat(pUnit, STAT_EXP, 0);
	startLevel = (int)D2COMMON_GetUnitStat(pUnit, STAT_LEVEL, 0);
}

void ChatColor::OnGameExit() {
	inGame = false;
}

void ChatColor::OnLoad() {
	InputLine1->Install();
	InputLine2->Install();
	UnicodeSupport1->Install();
	UnicodeSupport2->Install();
	UnicodeSupport3->Install();
	UnicodeSupport4->Install();
	Monitor1->Install();
	Monitor2->Install();
	Monitor3->Install();
	showLifeMana1->Install();
	showLifeMana2->Install();
	petHead->Install();
	partyHead->Install();
	DBase->Install();
	LoadConfig();
	//״̬д�������
	DWORD statNo[] = {   //״̬id
		1,2,9,11,19,21,28,55,58,60,
		62,95,61,10,16,20,26,30,31,32,
		38,51,88,94,101,117,120,128,129,130,
		131,132,133,134,135,136,137,139,140,144,
		145,153,157,158,159,177,8,14,33,34,
		37,40,41,42,45,48,49,148,149,151,
		161,162,3
	};
	DWORD color[] = {
		1,1,1,1,1,1,1,1,1,1,
		1,1,1,3,3,3,3,3,3,3,
		3,3,3,3,3,3,3,3,3,3,
		3,3,3,3,3,3,3,3,3,3,
		3,3,3,3,3,3,3,3,3,3,
		3,3,3,3,3,3,3,3,3,3,
		3,3,3
	};
	LPCSTR desc[] = {
		"凍結","判斷","傷害增加","傷害減少","詛咒","眩暈","聖光光環","傷害吸收","生命偷取","衰弱",
		"傷害減抗","聖戰士","魔法抵抗","防禦裝備","防禦強化","魔法裝備","抵抗","傷害減免","防禦","戰鬥命中",
		"魔法飛濺","戰鬥指揮","恢復生命","冰凍","神聖之盾","撞擊","野蠻狂暴","裝備修復","戰鬥傷害","火焰傷害",
		"冰凍傷害","閃電傷害","魔法傷害","毒素傷害","最小傷害","最大傷害","增強傷害","防禦態勢","攻擊態勢","準確",
		"魔法準確","魔影屏障","速度倍增","恢復之光","傷害轉換","傷害反彈","活力","電光裝備","火焰光環","冰凍",
		"閃電光環","祝福準確","集中","專注光環","嘲諷","奇思","淨化光環","榮耀之盾","橡木智慧","狂熱裝備",
		"橡木智慧","榮耀之盾","抵抗破除"
	};

	int statNoSize = sizeof(statNo) / sizeof(DWORD);  //��̬�����С

	for (DWORD n = 0; n < statNoSize; n++) {
		ChatColor::sMonitorStr[n].dwStatNo = statNo[n];
		ChatColor::sMonitorStr[n].dwColor = color[n];
		//ChatColor::sMonitorStr[0].fCountDown = TRUE;  //����ʱ,�����ʱ���У�����ʱ��Ĳ�������֪������
		MyMultiByteToWideChar(CP_ACP, 0, desc[n], -1, sMonitorStr[n].wszDesc[0], 30);
	}
	//������ʾѪ��,�Զ���
	*p_D2CLIENT_ShowLifeStr = TRUE;
	*p_D2CLIENT_ShowManaStr = TRUE;
	gameTimer = GetTickCount();
}

void ChatColor::OnUnload()
{
	InputLine1->Remove();
	InputLine2->Remove();
	UnicodeSupport1->Remove();
	UnicodeSupport2->Remove();
	UnicodeSupport3->Remove();
	UnicodeSupport4->Remove();
	Monitor1->Remove();
	Monitor2->Remove();
	Monitor3->Remove();
	showLifeMana1->Remove();
	showLifeMana2->Remove();
	petHead->Remove();
	partyHead->Remove();
	DBase->Remove();
}

void ChatColor::LoadConfig() {
	whisperColors.clear();

	BH::config->ReadAssoc("Whisper Color", whisperColors);

	//BH::config->ReadToggle("Merc Protect", "None", true, Toggles["Merc Protect"]);  //Ӷ������
	BH::config->ReadToggle("Merc Boring", "None", true, Toggles["Merc Boring"]);  //Ӷ���²�
	BH::config->ReadToggle("Rune Number", "None", false, Toggles["Rune Number"]);  //符文编号，默认关闭
	BH::config->ReadToggle("Show Money", "None", false, Toggles["Show Money"]);  //̰ģʽ,ĬϹر
	BH::config->ReadToggle("Death Back", "None", false, Toggles["Death Back"]);  //س,ĬϹر

	//BH::config->ReadInt("Merc Protect Sec", MercProtectSec, 3);   //Ĭ3
}

void ChatColor::OnChatPacketRecv(BYTE* packet, bool* block) {
	if (packet[1] == 0x0F && inGame) {
		unsigned int event_id = *(unsigned short int*)&packet[4];

		if (event_id == 4) {
			const char* from = (const char*)&packet[28];
			unsigned int fromLen = strlen(from);

			const char* message = (const char*)&packet[28 + fromLen + 1];
			unsigned int messageLen = strlen(message);

			bool replace = false;
			int color = 0;
			if (whisperColors.find(from) != whisperColors.end()) {
				replace = true;
				color = whisperColors[from];
			}

			if (replace) {
				*block = true;

				Print(color, "%s | %s", from, message);
			}
		}
	}
}

void ChatColor::OnDraw()
{
	DrawMonitorInfo();
}


void ChatColor::OnLoop()
{
	if (pD2WinEditBox && !D2CLIENT_GetUIState(UI_CHAT_CONSOLE)) {
		/**(DWORD*)p_D2CLIENT_LastChatMsg = 0;
		wchar_t* p = wcscpy(p_D2CLIENT_LastChatMsg, D2WIN_GetEditBoxTextHM(pD2WinEditBox));
		*p_D2CLIENT_ChatTextLength = wcslen(p);*/
		D2WIN_DestroyEditBoxHM(pD2WinEditBox);
		pD2WinEditBox = NULL;
		*p_D2WIN_FocusedControl = NULL;
	}
	CheckD2WinEditBox();


	if (BH::inGameOnce == false) {
		BH::inGameOnce = true;
		//*p_D2CLIENT_AutomapOn = TRUE;  //�Զ�������ͼ
		//D2CLIENT_ShowMap();  //Զͼ,��������������Զ�������ͼ
		(*p_D2CLIENT_AutomapPos).x += 32;  //��ͼ���ĵ�ƫ��...
		//(*p_D2CLIENT_AutomapPos).y += 0;
		dwPlayerId = D2CLIENT_GetPlayerUnit()->dwUnitId;

	}
}

void ResetMonitor() {

	for (int i = 0; i < 200; i++) {

		if ((int)(ChatColor::sMonitorStr[i].dwStatNo) <= 0)break;
		ChatColor::sMonitorStr[i].fEnable = 0;

	}

}

void ChatColor::OnEnd() 
{
	BH::inGameOnce = false;
	ResetMonitor();  //����״̬
}

void CheckD2WinEditBox()
{
	if (ChatColor::pD2WinEditBox) {
		if (D2CLIENT_GetUIState(UI_CHAT_CONSOLE)) {
			if (*(DWORD*)p_D2CLIENT_LastChatMsg != 0xAD5AFFFF) {
				D2WIN_DestroyEditBoxHM(ChatColor::pD2WinEditBox);
				ChatColor::pD2WinEditBox = NULL;
				*p_D2WIN_FocusedControl = NULL;
			}
			D2WIN_SetTextFont(1);
		}
	}
}

BOOL __cdecl InputLinePatch(BYTE keycode)
{
	if (keycode != VK_ESCAPE) {
		if (keycode != VK_RETURN) return TRUE;
		if (ChatColor::pD2WinEditBox) {
			wcscpy(p_D2CLIENT_LastChatMsg, D2WIN_GetEditBoxTextHM(ChatColor::pD2WinEditBox));
		}
	}
	return FALSE;
}

void __declspec(naked) InputLinePatch1_ASM()
{
	__asm {
		mov ebx, eax
		push[edi + 8]
		call InputLinePatch
		test eax, eax
		pop eax
		jz quitcode
		add dword ptr[esp], 0x427 // add to the point where function returns
		quitcode:
		ret
	}
}


BOOL __stdcall EditBoxCallBack(D2EditBox* pEditBox, DWORD dwLength, char* pChar)
{
	return TRUE;
}


BOOL __cdecl InitD2EditBox()
{
	//if (fMyChatOn) return FALSE;
	if (!D2CLIENT_GetUIState(UI_CHAT_CONSOLE)) return FALSE;
	if (!ChatColor::pD2WinEditBox) {
		static DWORD dws[] = { 0x0D, 0 };//��������
		ChatColor::pD2WinEditBox = D2WIN_CreateEditBoxHM(0x83,
			Hook::GetScreenHeight() - 58, Hook::GetScreenWidth() - 266,
			0x2D,
			0, 0, 0, 0, 0,
			sizeof(dws), &dws
		);
		//FOCUSECONTROL = pD2WinEditBox;
		*p_D2WIN_FocusedControlHM = ChatColor::pD2WinEditBox;  //����ȽϹؼ�����Ȼ��ȡ�������뽹��
		if (ChatColor::pD2WinEditBox) {
			D2WIN_SetEditBoxProcHM(ChatColor::pD2WinEditBox, &EditBoxCallBack);
			D2WIN_SetEditBoxTextHM(ChatColor::pD2WinEditBox, p_D2CLIENT_LastChatMsg);
		}
		*(DWORD*)p_D2CLIENT_LastChatMsg = 0xAD5AFFFF;
	}
	if (ChatColor::pD2WinEditBox)
		(ChatColor::pD2WinEditBox->fnCallBack)(ChatColor::pD2WinEditBox);
	return TRUE;
}

void __declspec(naked) InputLinePatch2_ASM()
{
	__asm {
		call InitD2EditBox
		test eax, eax
		jz quitcode
		add dword ptr[esp], 0x154
	quitcode:
		mov ecx, p_D2CLIENT_LastChatMsg
		ret
	}
}

LPCSTR __fastcall D2Lang_Unicode2WinPatch(LPSTR lpWinStr, LPWSTR lpUnicodeStr, DWORD dwBufSize)
{
	WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, lpWinStr, dwBufSize, NULL, NULL);
	return lpWinStr;
}


void UnicodeFix2(LPWSTR lpText)
{
	if (lpText) {
		size_t LEN = wcslen(lpText);
		for (size_t i = 0; i < LEN; i++)
		{
			if (lpText[i] == 0xf8f5) // Unicode 'y'
				lpText[i] = 0xff; // Ansi 'y'
		}
	}
}

int MyMultiByteToWideChar(
	UINT CodePage,         // code page
	DWORD dwFlags,         // character-type options
	LPCSTR lpMultiByteStr, // string to map
	int cbMultiByte,       // number of bytes in string
	LPWSTR lpWideCharStr,  // wide-character buffer
	int cchWideChar        // size of buffer
)
{
	int r = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	UnicodeFix2(lpWideCharStr);
	return r;
}

LPWSTR __fastcall D2Lang_Win2UnicodePatch(LPWSTR lpUnicodeStr, LPCSTR lpWinStr, DWORD dwBufSize)
{
	MyMultiByteToWideChar(CP_ACP, 0, lpWinStr, -1, lpUnicodeStr, dwBufSize);
	return lpUnicodeStr;
}

void UnicodeFix(LPWSTR lpText, DWORD* dwPos)
{
	DWORD len, i, j;
	WORD  char1, char2;
	WORD  uchar;
	BYTE  mbchar[2];
	DWORD	dwOldPos;
	DWORD	dwNewPos;

	if (!lpText) return;
	len = wcslen(lpText);
	if (dwPos) dwOldPos = *dwPos;
	else dwOldPos = 0;
	if (len >= 2) {
		dwNewPos = -1;
		for (i = 0, j = 0; i < len; i++) {
			char1 = lpText[i];
			char2 = lpText[i + 1];
			if (i == dwOldPos) {
				dwNewPos = j;
			}
			//a non-english character
			if (!HIBYTE(char1) && (char1) & 0x80 && char1 != 0xff && LOBYTE(char2)) {
				mbchar[0] = LOBYTE(char1);
				mbchar[1] = LOBYTE(char2);
				//if (fLocalizationSupport == 3) GB2GBK((char*)mbchar);
				if (MultiByteToWideChar(CP_ACP, 0, (char*)mbchar, sizeof(mbchar), (LPWSTR)&uchar, 1)) {
					//GbToBig5(&uchar);
					lpText[j++] = uchar;
					i++;
					if (i == dwOldPos) {
						dwNewPos = j - 1;
					}
				}
				else {
					lpText[j++] = char1;
				}
			}
			else {
				lpText[j++] = char1;
			}
		}
		if (dwNewPos == (DWORD)-1) {
			dwNewPos = j;
		}
		lpText[j] = 0;
	}
	else {
		dwNewPos = dwOldPos;
	}
	if (dwPos) *dwPos = dwNewPos;
	return;
}

void EditDeleteSelect(D2EditBox* hWnd)
{
	DWORD dwStart;
	DWORD len, i, j;

	if (hWnd->dwSelectStart == (DWORD)-1) return;
	if (hWnd->dwSelectStart == hWnd->dwSelectEnd) return;
	if (hWnd->dwSelectStart > hWnd->dwSelectEnd) {
		dwStart = hWnd->dwSelectEnd;
		len = hWnd->dwSelectStart - dwStart;
	}
	else {
		dwStart = hWnd->dwSelectStart;
		len = hWnd->dwSelectEnd - dwStart;
	}
	hWnd->dwSelectStart = (DWORD)-1;
	hWnd->dwSelectEnd = 0;
	i = dwStart;
	j = dwStart + len;
	while (1) {
		if (!hWnd->wszText[j]) break;
		hWnd->wszText[i++] = hWnd->wszText[j++];
	}
	hWnd->wszText[i] = 0;
	if (hWnd->dwCursorPos > dwStart && hWnd->dwCursorPos < dwStart + len) {
		hWnd->dwCursorPos = dwStart;
	}
	else if (hWnd->dwCursorPos >= dwStart + len) {
		hWnd->dwCursorPos -= len;
	}
	return;
}

DWORD __fastcall ChannelEnterCharPatch(D2EditBox *hWnd, BYTE bKeyCode)
{
	DWORD	ret;
	LPWSTR	lpText;
	DWORD len;
	WORD  char1;
	WORD  uchar;
	BYTE	mbchar[2];
	DWORD	dwPos;
	HGLOBAL hGlobal;
	LPSTR  lpStr;
	static BYTE lastcode = 0;
	
	lpText = (LPWSTR)(hWnd->wszText);
	len = wcslen(lpText);
	if (!len) memset( hWnd->wszText, 0, sizeof(hWnd->wszText) );
	if (hWnd->dwCursorPos > len) hWnd->dwCursorPos = len;
	if (hWnd->dwCursorPos > 0) {
		char1 = lastcode;
		if (!HIBYTE(char1) && (char1) & 0x80 && char1 != 0xff) {
			mbchar[0] = LOBYTE(char1);
			mbchar[1] = bKeyCode;
			lastcode = 0;
			//if( fLocalizationSupport==3 ) GB2GBK((char *)mbchar);
			if (MultiByteToWideChar(CP_ACP, 0, (char *)mbchar, sizeof(mbchar), (LPWSTR)&uchar, 1)) {
				lpText[hWnd->dwCursorPos-1] = uchar;
				return TRUE;
			}
		}
		
	}
	if (bKeyCode == KEY_CODE_PASTE) {
		ret = D2WIN_AddEditBoxChar(hWnd, bKeyCode);
		dwPos = hWnd->dwCursorPos;
		UnicodeFix(lpText, &dwPos);
		hWnd->dwCursorPos = dwPos;
		return ret;
	} else if (bKeyCode == KEY_CODE_COPY || bKeyCode == KEY_CODE_CUT) {
		DWORD dwStart;
		DWORD n;

		if (hWnd->dwSelectStart == (DWORD)-1) return FALSE;
		if (hWnd->dwSelectStart == hWnd->dwSelectEnd) return FALSE;
		if (hWnd->dwSelectStart > hWnd->dwSelectEnd) {
			dwStart = hWnd->dwSelectEnd;
			len = hWnd->dwSelectStart - dwStart;
		} else {
			dwStart = hWnd->dwSelectStart;
			len = hWnd->dwSelectEnd - dwStart;
		}
		if (!OpenClipboard(NULL)) return FALSE;
		EmptyClipboard();
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, (len+1) * 2); 
		if (hGlobal && (lpStr = (LPSTR)GlobalLock(hGlobal))) {
			n = WideCharToMultiByte(CP_ACP, 0, lpText + dwStart, len, lpStr, (len+1)*2, NULL, NULL);
			lpStr[n] = 0;
			GlobalUnlock(hGlobal);
			SetClipboardData(CF_TEXT, hGlobal);
		}
		CloseClipboard();
		if (hGlobal) GlobalFree(hGlobal);
		if (bKeyCode == KEY_CODE_CUT) EditDeleteSelect(hWnd);
		return TRUE;
	}
	if (bKeyCode >= ' ') {

		if ((bKeyCode & 0x80) && (bKeyCode != 0xff)) {
			lastcode = bKeyCode;
			ret = D2WIN_AddEditBoxChar(hWnd, 33);
		}else{
			ret = D2WIN_AddEditBoxChar(hWnd, bKeyCode);
		}
		UnicodeFix(hWnd->wszText, &hWnd->dwCursorPos);

		return ret;
	}
	ret = D2WIN_AddEditBoxChar(hWnd, bKeyCode);
	return ret;
}

void UnicodeToUTF_8(char* pOut, wchar_t* pText) {

	// ע�� WCHAR�ߵ��ֵ�˳��,���ֽ���ǰ�����ֽ��ں�   
	char* pchar = (char*)pText;
	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
	return;

}

void __stdcall MultiByteFix(LPSTR str)
{
	struct tagChat {
		DWORD flag0;
		DWORD flag;
		DWORD _1[5];
		char txt[100];
	};
	tagChat* pc = (tagChat*)str;
	if (InGame == FALSE && pc->flag != 4 && pc->flag != 10 && pc->flag != 6)return;
	//if (pc->flag != 4 && pc->flag != 10)return;

	char szTemp[300];
	char* pStr = pc->txt + strlen(pc->txt) + 1;
	char* ptemp = szTemp;
	char* p = pStr;

	//�Ȼ�ԭ��gb2312
	while (*p) {
		if (((*p) & 0xFC) == 0xC0) {//����
			*ptemp = ((p[0] & 0x03) << 6) + (p[1] & 0x3F);
			p++;
		}
		else {
			*ptemp = *p;
		}
		p++;
		ptemp++;
	}
	*ptemp = '\0';

	//����ת��utf8
	ptemp = szTemp;
	char buf[4];
	wchar_t pbuffer = L'\0';
	while (*ptemp) {
		if (*ptemp > 0) {
			*pStr++ = *ptemp++;
		}
		else {
			memset(buf, 0, 4);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ptemp, 2, &pbuffer, 1);
			UnicodeToUTF_8(buf, &pbuffer);
			*pStr++ = buf[0];
			*pStr++ = buf[1];
			*pStr++ = buf[2];
			ptemp++;
			ptemp++;
		}
	}
	*pStr = '\0';
}

void __declspec(naked) MultiByteFixPatch_ASM()
{
	__asm {
		mov eax, [p_BNCLIENT_BnChatMessage];
		mov[eax], ebp;
		push ebp;
		call MultiByteFix;
		ret;
	}
}


//������״̬������

void DrawMonitorInfo() {

	//if (tStateMonitorToggle.isOn == 0) return;  //Ĭ�Ͽ���
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	if (!pUnit) return;

	wchar_t wszTemp[512];
	memset(wszTemp, 0, sizeof(wszTemp));

	int xpos = Hook::GetScreenWidth() - 10;
	int ypos = Hook::GetScreenHeight() - 130;

	DWORD dwTimer = GetTickCount();
	DWORD dwOldFone = D2WIN_SetTextFont(8);
	for (int i = 0; i < 200; i++) {

		if ((int)(ChatColor::sMonitorStr[i].dwStatNo) <= 0)break;

		if (ChatColor::sMonitorStr[i].fEnable) {

			//SkillsTxt* test1 = &(*p_D2COMMON_sgptDataTable)->pSkillsTxt[146];
			//SkillsTxt* test2 = &(*p_D2COMMON_sgptDataTable)->pSkillsTxt[147];
			//SkillsTxt* test3 = &(*p_D2COMMON_sgptDataTable)->pSkillsTxt[155];
			//SkillsTxt* test4 = &(*p_D2COMMON_sgptDataTable)->pSkillsTxt[149];
			//SkillDescTxt* test1 =  &(*p_D2COMMON_sgptDataTable)->pSkillDescTxt[155];

			DWORD secs = (dwTimer - ChatColor::sMonitorStr[i].dwTimer) / 1000;
			if (ChatColor::sMonitorStr[i].fCountDown) {
				secs = ChatColor::sMonitorStr[i].dwTimer - secs;  //����ʱ
			}
			//wsprintfW(wszTemp, L"%s: %.2d:%.2d:%.2d", ChatColor::sMonitorStr[i].wszDesc[0], secs / 3600, (secs / 60) % 60, secs % 60);
			//wsprintfW(wszTemp, L"%s: %.2d:%.2d", ChatColor::sMonitorStr[i].wszDesc[0], (secs / 60) % 60, secs % 60);
			DWORD width, fileNo;
			D2WIN_GetTextWidthFileNo(wszTemp, &width, &fileNo);  //�����BH��ȡ������ķ���,��HM�е㲻һ��
			D2WIN_DrawText(wszTemp, xpos - width, ypos, ChatColor::sMonitorStr[i].dwColor, 0);
			ypos = ypos - 15;

		}

	}
	D2WIN_SetTextFont(dwOldFone);
}



void __stdcall SetState(DWORD dwStateNo, BOOL fSet) {

	//if (tStateMonitorToggle.isOn == 0) return;
	for (int i = 0; i < 200; i++) {
		if ((int)(ChatColor::sMonitorStr[i].dwStatNo) <= 0)break;
		if (ChatColor::sMonitorStr[i].dwStatNo == dwStateNo) {
			ChatColor::sMonitorStr[i].fEnable = fSet;
			ChatColor::sMonitorStr[i].dwTimer = GetTickCount();
			break;
		}
	}

}

void __declspec(naked) RecvCommand_A7_Patch_ASM()
{
	__asm {

		mov esi, ecx
		movzx edx, byte ptr[esi + 1]
		mov ecx, dword ptr[esi + 2]

		cmp edx, 0
		jne  org
		cmp ecx, [ChatColor::dwPlayerId]
		jne org
		movzx eax, byte ptr[esi + 06]

		push ecx
		push edx
		push esi

		push 1
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
		org :
		ret

	}

}

void __declspec(naked) RecvCommand_A8_Patch_ASM()
{
	__asm {

		mov esi, ecx
		movzx edx, byte ptr[esi + 1]
		mov ecx, dword ptr[esi + 2]

		cmp edx, 0
		jne  org
		cmp ecx, [ChatColor::dwPlayerId]
		jne org
		movzx eax, byte ptr[esi + 07]

		push ecx
		push edx
		push esi

		push 1
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
		org :
		ret

	}

}



void __declspec(naked) RecvCommand_A9_Patch_ASM()
{
	__asm {

		mov esi, ecx
		movzx edx, byte ptr[esi + 1]
		mov ecx, dword ptr[esi + 2]

		cmp edx, 0
		jne  org
		cmp ecx, [ChatColor::dwPlayerId]
		jne org
		movzx eax, byte ptr[esi + 06]

		push ecx
		push edx
		push esi

		push 0
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
		org :
		ret

	}

}

wchar_t* __cdecl wsprintfW2(wchar_t* dest, char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int len = wvsprintf((char*)dest, fmt, va);
	for (int i = len; i >= 0; i--) {
		dest[i] = ((char*)dest)[i];
	}
	return dest;
}

//long long ExpByLevel[] = {
//	0,
//	500,
//	1500,
//	3750,
//	7875,
//	14175,
//	22680,
//	32886,
//	44396,
//	57715,
//	72144,
//	90180,
//	112725,
//	140906,
//	176132,
//	220165,
//	275207,
//	344008,
//	430010,
//	537513,
//	671891,
//	839864,
//	1049830,
//	1312287,
//	1640359,
//	2050449,
//	2563061,
//	3203826,
//	3902260,
//	4663553,
//	5493363,
//	6397855,
//	7383752,
//	8458379,
//	9629723,
//	10906488,
//	12298162,
//	13815086,
//	15468534,
//	17270791,
//	19235252,
//	21376515,
//	23710491,
//	26254525,
//	29027522,
//	32050088,
//	35344686,
//	38935798,
//	42850109,
//	47116709,
//	51767302,
//	56836449,
//	62361819,
//	68384473,
//	74949165,
//	82104680,
//	89904191,
//	98405658,
//	107672256,
//	117772849,
//	128782495,
//	140783010,
//	153863570,
//	168121381,
//	183662396,
//	200602101,
//	219066380,
//	239192444,
//	261129853,
//	285041630,
//	311105466,
//	339515048,
//	370481492,
//	404234916,
//	441026148,
//	481128591,
//	524840254,
//	572485967,
//	624419793,
//	681027665,
//	742730244,
//	809986056,
//	883294891,
//	963201521,
//	1050299747,
//	1145236814,
//	1248718217,
//	1361512946,
//	1484459201,
//	1618470619,
//	1764543065,
//	1923762030,
//	2097310703,
//	2286478756,
//	2492671933,
//	2717422497,
//	2962400612,
//	3229426756,
//	3520485254,
//	3837739017,
//	9999999999
//};

void drawExperienceInfo() {
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	if (!pUnit) return;

	int nTime = ((GetTickCount() - gameTimer) / 1000);
	DWORD cExp = (DWORD)D2COMMON_GetUnitStat(pUnit, STAT_EXP, 0);
	if (startExperience == 0) { startExperience = cExp; }

	int cLevel = (int)D2COMMON_GetUnitStat(pUnit, STAT_LEVEL, 0);
	if (startLevel == 0) { startLevel = cLevel; }

	char sExp[255] = { 0 };
	double oldPctExp = ((double)startExperience - ExpByLevel[startLevel - 1]) / (ExpByLevel[startLevel] - ExpByLevel[startLevel - 1]) * 100.0;
	double pExp = ((double)cExp - ExpByLevel[cLevel - 1]) / (ExpByLevel[cLevel] - ExpByLevel[cLevel - 1]) * 100.0;
	double expGainPct = pExp - oldPctExp;
	if (cLevel > startLevel) {
		expGainPct = (100 - oldPctExp) + pExp + ((cLevel - startLevel) - 1) * 100;
	}
	double expPerSecond = nTime > 0 ? (cExp - startExperience) / (double)nTime : 0;
	char* unit = "";
	if (expPerSecond > 1E9) {
		expPerSecond /= 1E9;
		unit = "B";
	}
	else if (expPerSecond > 1E6) {
		expPerSecond /= 1E6;
		unit = "M";
	}
	else if (expPerSecond > 1E3) {
		expPerSecond /= 1E3;
		unit = "K";
	}

	sprintf_s(sExp, "%00.2f%% (%s%00.2f%%) [%s%.2f%s/s]", pExp, expGainPct >= 0 ? "+" : "", expGainPct, expPerSecond >= 0 ? "+" : "", expPerSecond, unit);

	Texthook::Draw((*p_D2CLIENT_ScreenSizeX / 2) - 200, *p_D2CLIENT_ScreenSizeY - 60, Center, 6, White, "%s", sExp);
}

void drawRuneNum() {
	// 禁用符文编号显示功能
	return;
	
	/*
	//ԸĻһ
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	if (!pUnit) return;
	
	if (Item::viewingUnit) return;   //ǲ鿴ҼʱͲʾ 

	if (!(D2CLIENT_GetUIState(UI_INVENTORY) || D2CLIENT_GetUIState(UI_STASH) || D2CLIENT_GetUIState(UI_CUBE) || D2CLIENT_GetUIState(UI_NPCSHOP)))
		return;

	for (UnitAny* pItem = pUnit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
		int left = 0;
		int top = 0;
		if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY) {
			left = INVENTORY_LEFT;
			top = INVENTORY_TOP;
			//ɫʾ
			//if (!D2CLIENT_GetUIState(UI_INVENTORY)&& !D2CLIENT_GetUIState(UI_STASH)) continue;
		}
		else if (pItem->pItemData->ItemLocation == STORAGE_STASH) {
			left = STASH_LEFT;
			top = STASH_TOP;
			if (!D2CLIENT_GetUIState(UI_STASH)) continue;
		}
		else if (pItem->pItemData->ItemLocation == STORAGE_CUBE) {
			left = CUBE_LEFT;
			top = CUBE_TOP;
			if (!D2CLIENT_GetUIState(UI_CUBE)) continue;
		}

		if (code[0] == 'r' && code[1] >= '0' && code[1] <= '3' && code[2] >= '0' && code[2] <= '9') {
			char numCode[2];
			numCode[0] = code[1];
			numCode[1] = code[2];
			Texthook::Draw(left + 1 + CELL_SIZE * pItem->pObjectPath->dwPosX, top + 20 + CELL_SIZE * pItem->pObjectPath->dwPosY, None, 6, White, "%d", stoi(numCode));
		}
	}
	*/
}


void DrawDefaultFontText(wchar_t* wStr, int xpos, int ypos, DWORD dwColor, int div, DWORD dwAlign)
{
	D2WIN_DrawText(wStr, xpos - (D2WIN_GetTextPixelLen(wStr) >> div), ypos, dwColor, dwAlign);//dwAlign:����ʱ�������� 1���� 0 ����
}

char* szOrbPattern = "%d/%d (%d%%)";
DWORD __stdcall ShowLifeWithMyPattern(DWORD callBack, int min, int max) {

	if (ChatColor::Toggles["Death Back"].state&&min<=0)   //����д�������������س�,��Ҫ�ǽ��ȥʬ����ֻḴ���bug��
	{
		PrintText(Red, "死亡回城~");
		SendMessage(D2GFX_GetHwnd(), WM_KEYDOWN, VK_ESCAPE, 0);
		Sleep(100);  //������Է�ֹ��η���
	}
	wchar_t wszTemp[64];
	int iPercent = 100 * min / max;
	wsprintfW2(wszTemp, szOrbPattern, min, max, iPercent);
	DrawDefaultFontText(wszTemp, 75, Hook::GetScreenHeight() - 95, 0);
	return callBack;

}

DWORD __stdcall ShowManaWithMyPattern(DWORD callBack, int min, int max) {

	wchar_t wszTemp[64];
	int iPercent = 100 * min / max;
	wsprintfW2(wszTemp, szOrbPattern, min, max, iPercent);
	DrawDefaultFontText(wszTemp, Hook::GetScreenWidth() - 65, Hook::GetScreenHeight() - 95, 0);

	//��δ���д�������Ҫ�ǲ������Ʒ�����Ը���ס��Ŀǰ����֪��D2��Draw�����ȼ����⣩
	//���￪ʼ��������ʣ������
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	if (!pUnit) return callBack;

	DWORD col1 = 0;
	DWORD col2 = 0;
	DWORD col3 = 0;
	DWORD col4 = 0;
	//������Ʒλ��,16����,pObjectPath.x
	//12,13,14,15
	//8,9,10,11
	//4,5,6,7
	//0,1,2,3
	for (UnitAny* pItem = pUnit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		if (pItem->pItemData->ItemLocation == STORAGE_NULL && pItem->pItemData->NodePage == NODEPAGE_BELTSLOTS) {
			DWORD beltPos = pItem->pObjectPath->dwPosX;
			if (beltPos == 0 || beltPos == 4 || beltPos == 8 || beltPos == 12) {
				col1 += 1;
			}
			else if (beltPos == 1 || beltPos == 5 || beltPos == 9 || beltPos == 13) {
				col2 += 1;
			}
			else if (beltPos == 2 || beltPos == 6 || beltPos == 10 || beltPos == 14) {
				col3 += 1;
			}
			else if (beltPos == 3 || beltPos == 7 || beltPos == 11 || beltPos == 15) {
				col4 += 1;
			}
		}
	}
	//��ʼ������ʣ������
	DWORD beltOffsetX = 31;
	Texthook::Draw(*p_D2CLIENT_ScreenSizeX / 2 + 40, *p_D2CLIENT_ScreenSizeY - 33, None, 0, White, "%d", col1);
	Texthook::Draw(*p_D2CLIENT_ScreenSizeX / 2 + 40 + beltOffsetX, *p_D2CLIENT_ScreenSizeY - 33, None, 0, White, "%d", col2);
	Texthook::Draw(*p_D2CLIENT_ScreenSizeX / 2 + 40 + beltOffsetX * 2, *p_D2CLIENT_ScreenSizeY - 33, None, 0, White, "%d", col3);
	Texthook::Draw(*p_D2CLIENT_ScreenSizeX / 2 + 40 + beltOffsetX * 3, *p_D2CLIENT_ScreenSizeY - 33, None, 0, White, "%d", col4);

	//�ȼ�������ʾ�Ƶ�����
	if (ScreenInfo::Toggles["Experience Meter"].state) {
		drawExperienceInfo();
	}

	//��ʾ�������֣��ټӸ�����
	if(ChatColor::Toggles["Rune Number"].state)
		drawRuneNum();

	return callBack;

}
BYTE showOrbs = 2;  //��ʾ�Լ��ĸ�ʽ 
void __declspec(naked) ShowLifePatch_ASM()
{
	__asm {
		cmp [showOrbs], 2
		je showme
		mov ecx, 0x00001045
		ret
		showme :
		call ShowLifeWithMyPattern
		push eax
		add dword ptr[esp], 0x4E
		ret
	}
}

void __declspec(naked) ShowManaPatch_ASM()
{
	__asm {
		cmp [showOrbs], 2
		je showme
		mov ecx, 0x00001046
		ret
		showme :
		call ShowManaWithMyPattern
		push eax
		add dword ptr[esp], 0x5B
		ret
	}
}

//������ʱ�Ȳ����ô��룬vc++��̫��^^
void AutoToBelt()
{
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return;

	//"hp", "mp", "rv"
		//ѭ�����ұ��������ҩ
	for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY || pItem->pItemData->ItemLocation == STORAGE_CUBE) {   //ֻȡ�����ͺ��������
			char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
			if (code[0] == 'h' && code[1] == 'p') {
				DWORD itemId = pItem->dwUnitId;  //��ҩ
				//��һ�������֪���ǲ����������ݰ�
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
			if (code[0] == 'm' && code[1] == 'p') {
				DWORD itemId = pItem->dwUnitId;  //��ҩ
				//��һ�������֪���ǲ����������ݰ�
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
			if (code[0] == 'r' && code[1] == 'v') {
				DWORD itemId = pItem->dwUnitId;  //��ҩ
				//��һ�������֪���ǲ����������ݰ�
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
		}
	}
}

//Ӷ���Զ���ҩ,ֻ֧�������ϵ�ҩ
void AutoMercDrink(double perHP) {
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return;
	if (perHP > 65) return;  //����65����ֵҲֱ����������
	//"hp", "mp", "rv"
		//ѭ�����ұ��������ҩ
	DWORD itemId = 0;
	DWORD itemId_hp = 0;   //��ҩ
	DWORD itemId_rv = 0;   //��ҩ
	char* code = "NULL";
	for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		if (pItem->pItemData->ItemLocation == STORAGE_NULL && pItem->pItemData->NodePage == NODEPAGE_BELTSLOTS) {   //ֻ�����������
			code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
			if (code[0] == 'h' && code[1] == 'p') {
				itemId_hp = pItem->dwUnitId; //��ҩ
			}
			if (code[0] == 'r' && code[1] == 'v') {
				itemId_rv = pItem->dwUnitId; //��ҩ
			}
			if(itemId_hp !=0 && itemId_rv !=0) break;  //���ҵ��˾�ֱ���ж�ѭ��
		}
	}
	if (itemId_hp == 0 && itemId_rv==0 && perHP <= 35) {
		if (ChatColor::Toggles["Merc Boring"].state)
			PrintText(Yellow, "�㶼ûҩ�����������һؼң���%.0f%%", perHP);
	}
	else {
		if (perHP > 35 && perHP <= 65) {
			if (itemId_hp == 0 && itemId_rv!=0) {
				itemId = itemId_rv;
				if (ChatColor::Toggles["Merc Boring"].state)
					PrintText(Purple, "Ӷ����Ѫ��,�Ϻ�����,�ɱ�����%.0f%%", perHP);
			}
			else if(itemId_hp != 0){
				itemId = itemId_hp;
				if (ChatColor::Toggles["Merc Boring"].state)
					PrintText(Red, "Ӷ����Ѫ��,�������,�ɱ�����%.0f%%", perHP);
			}
		}
		else if (perHP <= 35) {
			if (itemId_rv != 0) {
				itemId = itemId_rv;
				if (ChatColor::Toggles["Merc Boring"].state)
					PrintText(Purple, "Ӷ����Ѫ��,�Ϻ�����,�ɱ�����%.0f%%", perHP);
			}
		}
	}
	if (itemId == 0) return;  //ûҩ�˻��ú�ֱ������
	BYTE PacketData[13] = { 0x26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	*reinterpret_cast<int*>(PacketData + 1) = itemId;
	*reinterpret_cast<int*>(PacketData + 5) = 1;  //�Ƿ��Ӷ����ҩ,1�Ǹ���0�ǲ���
	D2NET_SendPacket(13, 0, PacketData);
	//���껹Ҫ����ȥ
	Task::Enqueue([=]()->void {
		Sleep(1000);  //ͣ1�����Կ�
		AutoToBelt();
	});
}

//ͷ�����
void __fastcall DrawPetHeadPath(int xpos, UnitAny* pUnit) {
	
	wchar_t wszTemp[512];
	wsprintfW(wszTemp, L"%d", D2COMMON_GetUnitStat(pUnit, STAT_LEVEL, 0));
	//swprintf(wszTemp, L"%d��%f/%f", D2COMMON_GetUnitStat(pUnit, STAT_LEVEL, 0), hp,maxhp);
		
	//D2WIN_DrawText(1, wszTemp, xpos + 5, 57, 0);
	DWORD dwOldFone = D2WIN_SetTextFont(1);   //��������
	D2WIN_DrawText(wszTemp,xpos+5,57,White,0);
	//D2WIN_DrawText(wszTemp, xpos + 700, 570, White, 0);
	D2WIN_SetTextFont(dwOldFone);
	bool test1 = ChatColor::Toggles["Merc Protect"].state;
	if (test1) {
		//������Ӷ���Զ���ҩ
		DWORD mHP = D2COMMON_GetUnitStat(pUnit, STAT_HP, 0);
		DWORD mMAXHP = D2COMMON_GetUnitStat(pUnit, STAT_MAXHP, 0);
		//if (mHP > 0x8000) {  //���˵��merc��Ѫ�����˱仯,��ʱ�Ȳ������ȥ�ж� by zyl 20220422
			double maxhp = (double)(mMAXHP >> 8);  //���Ӷ��ȡ������300����֪��Ϊʲô
			if (maxhp > 128) maxhp = 128;   //����������Ϊ128���Կ��ˡ�
			double hp = (double)(mHP >> 8);
			double perHP = (hp / maxhp) * 100.0;
			if (perHP > 100) perHP = 100;  //�����ҩ��ʱ���п��ܻ��г���100�����
			if ((GetTickCount() - mercLastTime) < (MercProtectSec*1000)) {  //�����ȹ̶�3���
				//PrintText(White, "�ϴθպȹ�ҩ���Ȳ���");
				return;  //�ϴκ�ҩʱ������3���Ȳ���
			}
			if (perHP < mercLastHP)
			{
				//PrintText(White, "Ӷ��Ѫ���٣�%.0f%%��%.0f%%", perHP, mercLastHP);
				//��ʼ��ҩ
				AutoMercDrink(perHP);
				mercLastTime = GetTickCount();  //��¼һ�º�ҩ��ʱ��
			}
			mercLastHP = perHP;
		//}
	}
}

void __declspec(naked) DrawPetHeadPath_ASM()
{
	//ecx  xpos
	//eax  ypos
	//ebx  pPet
	__asm {
		push esi

		mov edx, ebx
		call DrawPetHeadPath

		pop esi
		//org
		mov[esp + 0x56], 0
		ret
	}
}


void __fastcall DrawPartyHeadPath(int xpos, RosterUnit* pRosterUnit) {

	wchar_t wszTemp[512];

	//if (tShowPartyLevel.isOn) {
		wsprintfW(wszTemp, L"%d", pRosterUnit->wLevel);
		//DrawD2Text(1, wszTemp, xpos + 5, 57, 0);
		DWORD dwOldFone = D2WIN_SetTextFont(1);   //��������
		D2WIN_DrawText(wszTemp, xpos + 5, 57, White, 0);
		D2WIN_SetTextFont(dwOldFone);
	//}

	//if (tShowPartyPosition.isOn) {  //�����ſ��������Ȳ���
	//	wsprintfW(wszTemp, L"%d", pRosterUnit->dwLevelNo);
	//	DrawCenterText(1, wszTemp, xpos + 20, 15, 4, 1, 1);
	//}
}

void __declspec(naked) DrawPartyHeadPath_ASM()
{
	//[ebx]  xpos
	//eax  ypos
	//[esp+0C]  pRosterUnit
	__asm {
		mov ecx, dword ptr[ebx]
		mov edx, dword ptr[esp + 0xC]

		push ebx
		push edi
		push eax

		call DrawPartyHeadPath

		pop eax
		pop edi
		pop ebx

		mov ecx, dword ptr[ebx]
		mov edx, dword ptr[esp + 0xC]
		ret
	}
}

wchar_t* wcsrcat(wchar_t* dest, wchar_t* src)
{
	memmove(dest + wcslen(src), dest, (wcslen(dest) + 1) * sizeof(wchar_t));
	memcpy(dest, src, wcslen(src) * sizeof(wchar_t));
	return dest;
}

wchar_t* __fastcall StrcatDefenseStringPatch(wchar_t* wszStr) {

	//ƴ�����ַ����ĵط�,��һ������ƴ�ӻس�
	//����ETH��Ʒ Bug��ף����bug�����������ķ�bug���ֵ��С������Ϊ�Ƿ�bug
	//if (tShowItemVariableProp.isOn) {

		UnitAny* pUnit = *p_D2CLIENT_SelectedInvItem;
		if (pUnit) {

			ItemTxt* pItemTxt = (ItemTxt*)D2COMMON_GetItemText(pUnit->dwTxtFileNo);
			if (pItemTxt && pItemTxt->dwMinDef > 0) {

				int mDef = D2COMMON_GetBaseStatSigned(pUnit, 31, 0);
				int mMinDef = pItemTxt->dwMinDef;
				int mMaxDef = pItemTxt->dwMaxDef;
				wchar_t wszPrep[256] = { L"\0" };
				wchar_t wszTemp[512] = { L"\0" };

				if (D2COMMON_CheckItemFlag(pUnit, ITEM_ETHEREAL, 0, "?")) {
					mMinDef = (int)(mMinDef * 1.5);
					mMaxDef = (int)(mMaxDef * 1.5);

					if (mDef > mMaxDef && pUnit->pItemData->dwQuality == 2 && D2COMMON_CheckItemFlag(pUnit, ITEM_HASSOCKETS, 0, "?")) {
						//��ͨ��Ʒ ,���ף�������������������ΪETH BUG
						mMinDef = (int)(mMinDef * 1.5);
						mMaxDef = (int)(mMaxDef * 1.5);
						wcscpy(wszPrep, L"(Bug)");
					}

				}

				if (mDef > mMaxDef) {// 如果超过最大说明有附加ED或者自动max+1
					wsprintfW(wszTemp, L"%s(最小: %d, 最大: %d, 當前: %d+%d)\n", wszPrep, mMinDef, mMaxDef, mMaxDef, mDef - mMaxDef);
				}
				else {
					wsprintfW(wszTemp, L"%s(最小: %d, 最大: %d, 當前: %d)\n", wszPrep, mMinDef, mMaxDef, mDef);
				}
				wcsrcat(wszStr, wszTemp);
			}

		}

	//}
	return wszStr;
}

void __declspec(naked) StrcatDefenseStringPatch_ASM()
{
	__asm {
		push ebx

		mov ecx, edi
		call StrcatDefenseStringPatch

		pop ebx

		mov edx, dword ptr[esp + 0x1C]
		mov ecx, eax
		ret
	}
}