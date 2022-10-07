#include "ExtendedSaveFile.h"
#include "PlayerCustomData.h"
#include "InfinityStash.h"
#include "../../D2Ptrs.h"
#include "../../D2Helpers.h"

#define FILE_VERSION 0x3130			//"01"
#define FILE_EXTENDED 0x4D545343	//"CSTM"


BYTE* readExtendedSaveFile(char* name, DWORD* size)
{
	char filename[512];
	BYTE* data;
	d2_assert(!name, "Bad file name (NULL)", __FILE__, __LINE__);
	FOG_GetSavePath(filename, 512 - 5);
	strncat(filename, name, 512 - strlen(filename) - 5);
	strcat(filename, ".d2x");

	LogMsg("Extended file to read : %s\n", filename);

	FILE* file = fopen(filename, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		*size = ftell(file);
		fseek(file, 0, SEEK_SET);
		data = (BYTE*)FOG_MemAlloc(*size, __FILE__, __LINE__, 0);
		DWORD nbRead = fread(data, 1, *size, file);
		fclose(file);
		d2_assert(nbRead != *size, "nbRead from extented save file != size", __FILE__, __LINE__);
	}
	else {
		LogMsg("Can't open extented save file in mode \"rb\" (is not an error if it's a new player)\n");
		DWORD maxSize = 100;
		data = (BYTE*)FOG_MemAlloc(maxSize, __FILE__, __LINE__, 0);
		*size = 14;
		*((DWORD*)&data[0]) = FILE_EXTENDED; //"CSTM"
		*((WORD*)&data[4]) = FILE_VERSION;
		//*((DWORD*)&data[6]) = (BYTE)(nbPlayersCommandByDefault == 0 ? 0 : nbPlayersCommandByDefault - 1);
		*((DWORD*)&data[10]) = 0;// number of stash

		//TCustomDll* currentDll = customDlls;
		//while (currentDll)
		//{
		//	currentDll->initExtendedSaveFile(&data, &maxSize, size);
		//	currentDll = currentDll->nextDll;
		//}
	}
	return data;
}

int loadExtendedSaveFile(UnitAnyHM* ptChar, BYTE* data, DWORD maxSize)
{
	if (!ptChar || !PCPY49 || !data) return 0;

	LogMsg("Load extended file\n");

	DWORD curSize = 0;

	if (*(DWORD*)&data[curSize] != FILE_EXTENDED)
	{
		LogMsg("loadExtendedSaveFile -> bad header\n");
		return 9;
	}
	curSize += 4;

	if (*(WORD*)&data[curSize] != FILE_VERSION)
	{
		LogMsg("loadExtendedSaveFile -> bad file version\n");
		return 9;
	}
	curSize += 2;
	//nbPlayersCommand = (*(BYTE*)&data[curSize]) + 1;  //这个应该用于存储默认多少倍pp的值
	curSize += 1;
	curSize += 3;

	int ret = loadStashList(ptChar, data, maxSize, &curSize, false);

	//TCustomDll* currentDll = customDlls;
	//while (!ret && currentDll)
	//{
	//	ret = currentDll->loadExtendedSaveFile(ptChar, data, maxSize, &curSize);
	//	currentDll = currentDll->nextDll;
	//}


	PCPY49->selfStashIsOpened = true;
	return ret;
}

void saveExtendedSaveFile(UnitAnyHM* ptChar, BYTE** data, DWORD* maxSize, DWORD* curSize)
{
	*(DWORD*)(*data + *curSize) = FILE_EXTENDED; //"CSTM"
	*curSize += 4;
	*(WORD*)(*data + *curSize) = FILE_VERSION;
	*curSize += 2;
	//*(DWORD*)(*data + *curSize) = (BYTE)(nbPlayersCommand == 0 ? 0 : nbPlayersCommand - 1);  //这个在PlugY中可能只是用于保存设置的pp数,方便在加载存档的时候直接设置pp数
	*(DWORD*)(*data + *curSize) = (BYTE)0;
	*curSize += 4;

	saveStashList(ptChar, PCPY49->selfStash, data, maxSize, curSize);

}

void writeExtendedSaveFile(char* name, BYTE* data, DWORD size)
{
	char szTempName[MAX_PATH];
	char szSaveName[MAX_PATH];

	//Get temporary savefile name.
	FOG_GetSavePath(szTempName, MAX_PATH);
	strcat(szTempName, name);
	strcat(szTempName, ".d2~");
	LogMsg("Extended temporary file for saving : %s\n", szTempName);

	//Write data in savefile.
	FILE* customSaveFile = fopen(szTempName, "wb+");
	fwrite(data, size, 1, customSaveFile);
	fclose(customSaveFile);

	//Get real savefile name.
	FOG_GetSavePath(szSaveName, MAX_PATH);
	strcat(szSaveName, name);
	strcat(szSaveName, ".d2x");
	LogMsg("Extended file for saving : %s\n", szSaveName);

	//	if (!MoveFileEx(szTempName, szSaveName, MOVEFILE_WRITE_THROUGH|MOVEFILE_REPLACE_EXISTING))
	DeleteFile(szSaveName);
	if (!MoveFile(szTempName, szSaveName))
		LogMsg("Could not create the extended save file.");
}
