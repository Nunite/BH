#pragma once
#include "../../D2Structs.h"


void selectStash(UnitAnyHM* ptChar, Stash* newStash, bool forceUpdate = false);
void setCurrentStashIndex(UnitAnyHM* ptChar, int index);
void selectPreviousStash(UnitAnyHM* ptChar);
void selectNextStash(UnitAnyHM* ptChar);
void selectPreviousIndexStash(UnitAnyHM* ptChar);
void selectNextIndexStash(UnitAnyHM* ptChar);
void selectPrevious2Stash(UnitAnyHM* ptChar);
void selectNext2Stash(UnitAnyHM* ptChar);
void selectPreviousIndex2Stash(UnitAnyHM* ptChar);
void selectNextIndex2Stash(UnitAnyHM* ptChar);
void insertStash(UnitAnyHM* ptChar);
bool deleteStash(UnitAnyHM* ptChar, bool isClient);
void swapStash(UnitAnyHM* ptChar, DWORD page, bool toggle);
void getCurrentStashName(WCHAR* buffer, DWORD maxSize, UnitAnyHM* ptChar);

void updateSelectedStashClient(UnitAnyHM* ptChar);
int changeToSelectedStash(UnitAnyHM* ptChar, Stash* newStash, DWORD bOnlyItems, DWORD bIsClient);
void setSelectedStashClient(DWORD stashId, DWORD stashFlags, DWORD flags, bool bOnlyItems);
void renameCurrentStash(UnitAnyHM* ptChar, char* name);

DWORD loadStashList(UnitAnyHM* ptChar, BYTE* data, DWORD maxSize, DWORD* curSize, bool isShared);
void saveStashList(UnitAnyHM* ptChar, Stash* ptStash, BYTE** data, DWORD* maxSize, DWORD* curSize);

void Install_MultiPageStash();