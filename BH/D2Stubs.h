#pragma once
#include <Windows.h>

DWORD __fastcall D2CLIENT_GetUnitName_STUB(DWORD UnitAny);
DWORD __fastcall D2CLIENT_InitAutomapLayer(DWORD nLayerNo);
DWORD __fastcall TestPvpFlag_STUB(DWORD planum1, DWORD planum2, DWORD flagmask);
DWORD __fastcall D2CLIENT_GetLevelName_STUB(DWORD levelId);
DWORD __fastcall D2CLIENT_GetUIVar_STUB(DWORD varno);
CellFile* __fastcall D2CLIENT_LoadUiImage(CHAR* szPath);
DWORD __fastcall D2CLIENT_ClickParty_ASM(RosterUnit* RosterUnit, DWORD Mode);
void __fastcall D2CLIENT_PlaySound(int SoundNo);
int __fastcall ITEMS_GetItemPropertiesString_STUB(int nStatFilter2, UnitAny* pItem, wchar_t* strBuffer, int nBufferLen, int a5, int nLayer, int nStatFilter, int a8, int a9, wchar_t* strBuffer2);

DWORD __fastcall D2GameASM_D2LoadInventory_STUB(GameStruct* ptGame, UnitAnyHM* pChar, saveBitField* pdata, DWORD p2, DWORD maxSize, DWORD p4, DWORD* ptNbBytesRead);
DWORD __fastcall D2GameASM_SendPacket_STUB(void* ptNetClient, LPVOID pData, DWORD size);

UnitAnyHM* __fastcall D2GameASM_D2GetPlayerPet(UnitAnyHM* pPlayer, DWORD unk_7, DWORD unk_1, GameStruct* ptGame);
void* __fastcall D2ClientASM_LoadImage_STUB(const char* filename, DWORD filetype);
void __fastcall D2ClientASM_FreeImage_STUB(void* image);
UnitAnyHM* __fastcall D2GameASM_GetObject_STUB(GameStruct* ptGame, DWORD type, DWORD itemNum);