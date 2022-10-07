#include <Windows.h>
#include "D2Ptrs.h"

DWORD __declspec(naked) __fastcall D2CLIENT_GetUnitName_STUB(DWORD UnitAny)
{
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_GetUnitName_I
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_InitAutomapLayer(DWORD nLayerNo)
{
	__asm
	{
		push eax;
		mov eax, ecx;
		call D2CLIENT_InitAutomapLayer_I;
		pop eax;
		ret;
	}
}

DWORD __declspec(naked) __fastcall TestPvpFlag_STUB(DWORD planum1, DWORD planum2, DWORD flagmask)
{
	__asm
	{
		push esi;
		push[esp + 8];
		mov esi, edx;
		mov edx, ecx;
		call D2CLIENT_TestPvpFlag_I;
		pop esi;
		ret 4;
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_GetLevelName_STUB(DWORD levelId)
{
	__asm
	{
		mov esi, ecx
		jmp D2CLIENT_GetLevelName_I
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_GetUIVar_STUB(DWORD varno)
{
	__asm
	{
		mov eax, ecx;
		jmp D2CLIENT_GetUiVar_I;
	}
}

__declspec(naked) CellFile* __fastcall D2CLIENT_LoadUiImage(CHAR* szPath)
{
	__asm
	{
		MOV EAX, ECX
		PUSH 0
		CALL D2CLIENT_LoadUiImage_I
		RETN
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_ClickParty_ASM(RosterUnit* RosterUnit, DWORD Mode)
{
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_ClickParty_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_PlaySound(int SoundNo)
{
	__asm
	{
		push ebx
		mov ebx, ecx
		push 0
		push 0
		push 0
		push 0
		call D2CLIENT_PlaySound_I
		pop ebx;
		ret
	}
}

__declspec (naked) int __fastcall ITEMS_GetItemPropertiesString_STUB(int nStatFilter2, UnitAny* pItem, wchar_t* strBuffer, int nBufferLen, int a5, int nLayer, int nStatFilter, int a8, int a9, wchar_t* strBuffer2)
{
	__asm
	{
		push edi
		push esi
		mov edi, [esp + 40]
		mov esi, [esp + 36]
		push[esp + 32]
		push[esp + 32]
		push[esp + 32]
		push[esp + 32]
		push[esp + 32]
		push[esp + 32]
		push edx
		call D2CLIENT_GetItemPropertiesString
		pop esi
		pop edi
		retn 32
	}
}

__declspec (naked) DWORD __fastcall D2GameASM_D2LoadInventory_STUB(GameStruct* ptGame, UnitAnyHM* pChar, saveBitField* pdata, DWORD p2, DWORD maxSize, DWORD p4, DWORD* ptNbBytesRead)
{
	__asm
	{
		MOV EAX, DWORD PTR SS : [ESP + 4]
		MOV DWORD PTR SS : [ESP + 4] , EDX
		JMP D2GAME_D2LoadInventory
	}
}

__declspec (naked) DWORD __fastcall D2GameASM_SendPacket_STUB(void* ptNetClient, LPVOID pData, DWORD size)
{
	__asm
	{
		POP EAX
		PUSH EDX
		PUSH EAX
		MOV EAX, ECX
		JMP D2GAME_SendPacket
	}
}

__declspec (naked) UnitAnyHM* __fastcall D2GameASM_D2GetPlayerPet(UnitAnyHM* pPlayer, DWORD unk_7, DWORD unk_1, GameStruct* ptGame)
{
	__asm
	{
		//pushad
		push ebp
		push ebx
		push edi
		push esi
		mov edi, [esp + 0x18]//第四个参数
		mov ebx, [esp + 0x14]//第三个参数
		mov esi, edx
		mov eax, ecx
		call D2GAME_GetPlayerPet
		pop esi
		pop edi
		pop ebx
		pop ebp
		retn 8
	}
}

__declspec (naked) void* __fastcall D2ClientASM_LoadImage_STUB(const char* filename, DWORD filetype)
{
	__asm
	{
		PUSH EDX
		MOV EAX, ECX
		CALL D2CLIENT_LoadImage
		RETN
	}
}

const char* D2FreeImage_FILE = __FILE__;
__declspec (naked) void __fastcall D2ClientASM_FreeImage_STUB(void* image)
{
	__asm
	{
		PUSH ESI
		MOV ESI, ECX
		PUSH ESI
		CALL D2CMP_10014
		TEST ESI, ESI
		JE END_D2Free
		PUSH 0
		PUSH __LINE__
		MOV EDX, D2FreeImage_FILE;  ASCII "C:\projects\D2\head\Diablo2\Source\D2Client\CORE\ARCHIVE.CPP"
		MOV ECX, ESI
		CALL FOG_MemDeAlloc
	END_D2Free :
		POP ESI
		RETN
	}
}

__declspec (naked) UnitAnyHM* __fastcall D2GameASM_GetObject_STUB(GameStruct* ptGame, DWORD type, DWORD itemNum)
{
	__asm
	{
		MOV EAX, EDX
		MOV EDX, DWORD PTR SS : [ESP + 4]
		CALL D2GAME_GetObject
		RETN 4
	}
}