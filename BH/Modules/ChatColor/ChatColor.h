#pragma once
#include "../../D2Structs.h"
#include "../Module.h"
#include "../../Config.h"
#include "../../Common.h"

class ChatColor : public Module {
private:
	bool inGame;
	std::map<string, unsigned int> whisperColors;
public:
	static D2EditBox* pD2WinEditBox;
	ChatColor() : Module("Chat Color") {};

	void Init();

	void OnLoad();
	void OnUnload();
	void LoadConfig();
	void OnGameJoin();
	void OnGameExit();
	void OnChatPacketRecv(BYTE* packet, bool *block);

	void OnLoop();
};

#define		KEY_CODE_COPY			0x03
#define		KEY_CODE_PASTE			0x16
#define		KEY_CODE_CUT			0x18

void ToggleIMEInput(BOOL fEnable);

void CheckD2WinEditBox();
void InputLinePatch1_ASM();
void InputLinePatch2_ASM();

LPCSTR __fastcall D2Lang_Unicode2WinPatch(LPSTR lpWinStr, LPWSTR lpUnicodeStr, DWORD dwBufSize);
LPWSTR __fastcall D2Lang_Win2UnicodePatch(LPWSTR lpUnicodeStr, LPCSTR lpWinStr, DWORD dwBufSize);

DWORD __fastcall ChannelEnterCharPatch(D2EditBox* hWnd, BYTE bKeyCode);
void MultiByteFixPatch_ASM();
