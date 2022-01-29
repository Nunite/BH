#include "ChatColor.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"
#include "../../D2Intercepts.h"


using namespace Drawing;  //这个是Hook的namespace

static BOOL fDisabled = FALSE;
#pragma   comment(lib,"imm32.lib")   

D2EditBox* ChatColor::pD2WinEditBox;
//尝试做中文输入的补丁
Patch* InputLine1 = new Patch(Call, D2CLIENT, { 0x70C6C, 0xB254C }, (DWORD)InputLinePatch1_ASM, 5);
//{PatchCALL, DLLOFFSET2(D2CLIENT, 0x6FB20C6C, 0x6FB6254C), (DWORD)InputLinePatch1_ASM, 5, & fDefault},
Patch* InputLine2 = new Patch(Call, D2CLIENT, { 0x7037B, 0xB286B }, (DWORD)InputLinePatch2_ASM, 5);
//{ PatchCALL,   DLLOFFSET2(D2CLIENT, 0x6FB2037B, 0x6FB6186B),    (DWORD)InputLinePatch2_ASM,             5 ,   &fDefault },
Patch* UnicodeSupport1 = new Patch(Jump, D2LANG, { 0x82F0, 0x82B0 }, (DWORD)D2Lang_Unicode2WinPatch, 5);
//{PatchJMP, DLLOFFSET2(D2LANG, 0x6FC082F0, 0x6FC082B0), (DWORD)D2Lang_Unicode2WinPatch, 5, & fLocalizationSupport},
Patch* UnicodeSupport2 = new Patch(Jump, D2LANG, { 0x8320, 0x82E0 }, (DWORD)D2Lang_Win2UnicodePatch, 5);
//{PatchJMP,    DLLOFFSET2(D2LANG, 0x6FC08320, 0x6FC082E0),      (DWORD)D2Lang_Win2UnicodePatch,          5 ,   &fLocalizationSupport},
Patch* UnicodeSupport3 = new Patch(Call, D2WIN, { 0x183A0, 0xE850 }, (DWORD)ChannelEnterCharPatch, 5);
//{PatchCALL, DLLOFFSET2(D2WIN, 0x6F8F83A0, 0x6F8EE850), (DWORD)ChannelEnterCharPatch, 5, & fLocalizationSupport}, //注意InputLinePatch1_ASM必须结合这个，不然会引起部分堆栈错误(虽然不影响使用)
Patch* UnicodeSupport4 = new Patch(Call, BNCLIENT, { 0xFF5C, 0x1513C }, (DWORD)MultiByteFixPatch_ASM, 6);
//{ PatchCALL,   DLLOFFSET2(BNCLIENT,0x6FF2FF5C, 0x6FF3513C),     (DWORD)MultiByteFixPatch_ASM,            6 ,   &fLocalizationSupport }, // for /w *acc msg text

void ChatColor::Init() {
	
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

void ChatColor::OnGameJoin() {
	inGame = true;
}

void ChatColor::OnGameExit() {
	inGame = false;
	if (pD2WinEditBox && !D2CLIENT_GetUIState(UI_CHAT_CONSOLE)) {
		*(DWORD*)p_D2CLIENT_LastChatMsg = 0;
		wchar_t* p = wcscpy(p_D2CLIENT_LastChatMsg, D2WIN_GetEditBoxTextHM(pD2WinEditBox));
		*p_D2CLIENT_ChatTextLength = wcslen(p);
		D2WIN_DestroyEditBoxHM(pD2WinEditBox);
		pD2WinEditBox = NULL;
		*p_D2WIN_FocusedControl = NULL;
	}
}

void ChatColor::OnLoad() {
	InputLine1->Install();
	InputLine2->Install();
	UnicodeSupport1->Install();
	UnicodeSupport2->Install();
	UnicodeSupport3->Install();
	UnicodeSupport4->Install();
	LoadConfig();
}

void ChatColor::OnUnload()
{
	InputLine1->Remove();
	InputLine2->Remove();
	UnicodeSupport1->Remove();
	UnicodeSupport2->Remove();
	UnicodeSupport3->Remove();
	UnicodeSupport4->Remove();
}

void ChatColor::LoadConfig() {
	whisperColors.clear();

	BH::config->ReadAssoc("Whisper Color", whisperColors);
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


void ToggleIMEInput(BOOL fEnable) {

	static HIMC hIMC = NULL;
	if (fEnable) {
		if (fDisabled) {
			ImmAssociateContext(D2GFX_GetHwnd(), hIMC);
			fDisabled = FALSE;
		}
	}
	else {
		if (fDisabled == FALSE) {
			hIMC = ImmAssociateContext(D2GFX_GetHwnd(), NULL);
			fDisabled = TRUE;
		}
	}

}

void ChatColor::OnLoop()
{
	CheckD2WinEditBox();
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
		static DWORD dws[] = { 0x0D, 0 };//所用字体
		ChatColor::pD2WinEditBox = D2WIN_CreateEditBoxHM(0x83,
			Hook::GetScreenHeight() - 58, Hook::GetScreenWidth() - 266,
			0x2D,
			0, 0, 0, 0, 0,
			sizeof(dws), &dws
		);
		//FOCUSECONTROL = pD2WinEditBox;
		*p_D2WIN_FocusedControlHM = ChatColor::pD2WinEditBox;  //这个比较关键，不然获取不到输入焦点
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

	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后   
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
	if (InGame == FALSE && pc->flag != 4 && pc->flag != 10)return;
	//if (pc->flag != 4 && pc->flag != 10)return;

	char szTemp[300];
	char* pStr = pc->txt + strlen(pc->txt) + 1;
	char* ptemp = szTemp;
	char* p = pStr;

	//先还原成gb2312
	while (*p) {
		if (((*p) & 0xFC) == 0xC0) {//汉字
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

	//重新转换utf8
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