#pragma once
#include "../../D2Structs.h"

BYTE* readExtendedSaveFile(char* name, DWORD* size);
int loadExtendedSaveFile(UnitAnyHM* ptChar, BYTE* data, DWORD maxSize);


void saveExtendedSaveFile(UnitAnyHM* ptChar, BYTE** data, DWORD* maxSize, DWORD* curSize);
void writeExtendedSaveFile(char* name, BYTE* data, DWORD size);
