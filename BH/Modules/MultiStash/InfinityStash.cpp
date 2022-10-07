#include "InfinityStash.h"
#include "PlayerCustomData.h"
#include "UpdateClient.h"
#include "Interface_Stash.h"
#include "../../D2Ptrs.h"
#include "../../D2Helpers.h"
#include "../../Constants.h"


#define STASH_TAG 0x5453			//"ST"
#define JM_TAG 0x4D4A				//"JM"
DWORD maxSelfPages = -1;            //这个可能要设置成500吧
DWORD maxSharedPages = -1;          //这个可能要设置成500吧
DWORD nbPagesPerIndex = 10;
DWORD nbPagesPerIndex2 = 100;



UnitAnyHM* firstClassicStashItem(UnitAnyHM* ptChar)
{
	UnitAnyHM* ptItem = D2COMMON_GetItemFromInventoryHM(ptChar->ptInventory);
	while (ptItem)
	{
		if (D2COMMON_GetItemPage(D2COMMON_GetRealItem(ptItem)) == 4)
			return ptItem;
		ptItem = D2COMMON_GetNextItemFromInventoryHM(ptItem);
	}
	return NULL;
}

DWORD endStashList(UnitAnyHM* ptChar, Stash* ptStash)
{
	Stash* stash = ptStash;

	while (stash)
	{
		if (stash->ptListItem || ((stash == PCPY49->currentStash) && firstClassicStashItem(ptChar))) return 0;
		if (stash->name != NULL && stash->name[0]) return 0;
		stash = stash->nextStash;
	}
	return 1;
}

Stash* getLastStash(Stash* ptStash)
{
	if (!ptStash)
		return NULL;
	while (ptStash->nextStash)
		ptStash = ptStash->nextStash;
	return ptStash;
}

Stash* newStash(DWORD id)
{
	d2_assert(id == 0xFFFFFFFF, "Too much stash", __FILE__, __LINE__);

	Stash* stash = (Stash*)malloc(sizeof(Stash));//D2AllocMem(memPool, sizeof(Stash),__FILE__,__LINE__,0);
	d2_assert(!stash, "Error on stash allocation.", __FILE__, __LINE__);
	ZeroMemory(stash, sizeof(Stash));
	stash->id = id;

	return stash;
}

Stash* addStash(UnitAnyHM* ptChar, bool isShared, bool autoSetIndex, Stash* previous)
{
	previous = getLastStash(previous ? previous : isShared ? PCPY49->sharedStash : PCPY49->selfStash);
	if (previous)
		isShared = previous->isShared;

	Stash* stash = newStash(isShared ? PCPY49->nbSharedPages++ : PCPY49->nbSelfPages++);
	stash->isShared = isShared;
	stash->previousStash = previous;

	if (previous)
	{
		previous->nextStash = stash;
		if (autoSetIndex)
		{
			stash->isIndex = ((stash->id + 1) % nbPagesPerIndex) == 0;
			stash->isMainIndex = ((stash->id + 1) % nbPagesPerIndex2) == 0;
		}
	}
	else if (isShared)
	{
		PCPY49->sharedStash = stash;
		stash->isIndex = 1;
		stash->isMainIndex = 1;
	}
	else
	{
		PCPY49->selfStash = stash;
		stash->isIndex = 1;
		stash->isMainIndex = 1;
	}

	LogMsg("AddStash: stash->id=%d\tstash->isShared=%d\tnbSelf=%d\tnbShared=%d\n", stash->id, stash->isShared, PCPY49->nbSelfPages, PCPY49->nbSharedPages);
	return stash;
}

int changeToSelectedStash(UnitAnyHM* ptChar, Stash* newStash, DWORD bOnlyItems, DWORD bIsClient)
{
	if (!newStash) return 0;

	Stash* currentStash = PCPY49->currentStash;
	if (currentStash == newStash) return 0;

	LogMsg("changeToSelectedStash ID:%d\tshared:%d\tonlyItems:%d\tclient:%d\tcStash:%08X\tnStash:%08X\n", newStash->id, newStash->isShared, bOnlyItems, bIsClient, currentStash, newStash);

	d2_assert(currentStash && currentStash->ptListItem, "ERROR : currentStash isn't empty (ptListItem != NULL)", __FILE__, __LINE__);

	// Remove items from current page
	UnitAnyHM* ptNextItem;
	UnitAnyHM* ptItem = D2COMMON_GetItemFromInventoryHM(ptChar->ptInventory);
	while (ptItem)
	{
		ptNextItem = D2COMMON_GetNextItemFromInventoryHM(ptItem);
		if (D2COMMON_GetItemPage(ptItem) == 4)
		{
			BYTE tmp = ptItem->ptItemData->ItemData2;
			ptItem = D2COMMON_InvRemoveItem(ptChar->ptInventory, ptItem);
			ptItem->ptItemData->ItemData2 = tmp;
			if (currentStash)
			{
				ptItem->ptItemData->ptNextItem = currentStash->ptListItem;
				currentStash->ptListItem = ptItem;
			}
		}
		ptItem = ptNextItem;
	}

	// add items of new stash
	ptItem = newStash->ptListItem;
	while (ptItem)
	{
		D2COMMON_InvAddItem(ptChar->ptInventory, ptItem, ptItem->pItemPath->dwPosX, ptItem->pItemPath->dwPosY, 0xC, bIsClient, 4);
		ptItem = D2COMMON_GetNextItemFromInventoryHM(ptItem);
	}
	if (bOnlyItems)
		newStash->ptListItem = PCPY49->currentStash->ptListItem;
	else
		PCPY49->currentStash = newStash;
	PCPY49->currentStash->ptListItem = NULL;

	return 1;
}

DWORD loadStash(UnitAnyHM* ptChar, Stash* ptStash, BYTE data[], DWORD startSize, DWORD maxSize, DWORD* retSize)
{
	DWORD curSize = startSize;
	DWORD nbBytesRead;

	LogMsg("loadStash\n");

	*retSize = curSize;
	if (*(WORD*)&data[curSize] != STASH_TAG)//"ST"
	{
		LogMsg("loadStash -> Bad tag of stash of character %s : %04X\n", ptChar->ptPlayerData->name, *(WORD*)&data[curSize]);
		return 0x7;//Unable to enter game. Bad inventory data
	}
	curSize += 2;

	// Read flags.
	int len = strlen((char*)&data[curSize]);
	if (*(WORD*)&data[curSize + len + 1] != JM_TAG)
	{
		ptStash->flags = *(DWORD*)&data[curSize];
		curSize += 4;
	}

	// Read Name
//	if (strlen((char *)&data[curSize]) > 0xF)
//		*(char *)&data[curSize+0xF] = NULL;
	if (strlen((char*)&data[curSize]))
		ptStash->name = (char*)malloc(strlen((char*)&data[curSize]) + 1);//D2AllocMem(PCGame->memoryPool, strlen((char *)&data[curSize]),__FILE__,__LINE__,0);
	if (ptStash->name)
		strcpy(ptStash->name, (char*)&data[curSize]);
	curSize += strlen((char*)&data[curSize]) + 1;

	// Read inventory.
	DWORD ret = D2GameASM_D2LoadInventory_STUB(ptChar->ptGame, ptChar, (saveBitField*)&data[curSize], 0x60, maxSize - curSize, 0, &nbBytesRead);
	if (ret) LogMsg("loadStash -> D2LoadInventory failed\n");
	LogMsg("Stash loaded (%d : %s)\n", ptStash->id, ptStash->name);

	*retSize = curSize + nbBytesRead;
	return ret;
}

void autoSetIndex(Stash* firstStash)
{
	if (!firstStash)
		return;
	LogMsg("autoSetIndex\n");
	Stash* stash = firstStash;
	while (stash)
	{
		if (stash->isIndex || stash->isMainIndex)
			return;
		stash = stash->nextStash;
	}
	stash = firstStash;
	stash->isIndex = 1;
	stash->isMainIndex = 1;
	stash = stash->nextStash;
	while (stash)
	{
		stash->isIndex = ((stash->id + 1) % nbPagesPerIndex) == 0;
		stash->isMainIndex = ((stash->id + 1) % nbPagesPerIndex2) == 0;
		stash = stash->nextStash;
	}
}

DWORD loadStashList(UnitAnyHM* ptChar, BYTE* data, DWORD maxSize, DWORD* curSize, bool isShared)
{
	DWORD curStash = 0;
	Stash* newStash = NULL;

	DWORD nbStash = *(DWORD*)&data[*curSize];
	*curSize += 4;
	LogMsg("loadStashList nbStash=%d\n", nbStash);

	while (curStash < nbStash)
	{
		newStash = addStash(ptChar, isShared, false, newStash);
		changeToSelectedStash(ptChar, newStash, 0, false);
		DWORD ret = loadStash(ptChar, newStash, data, *curSize, 10000000, curSize);
		if (ret) return ret;
		curStash++;
	}

	if (!isShared)
	{
		if (PCPY49->selfStash)
			autoSetIndex(PCPY49->selfStash);
		else
		{
			newStash = addStash(ptChar, isShared, true, newStash);
			PCPY49->currentStash = newStash;
		}
	}

	if (isShared)
	{
		if (PCPY49->sharedStash)
			autoSetIndex(PCPY49->sharedStash);
		else
		{
			newStash = addStash(ptChar, isShared, true, newStash);
			if (!PCPY49->currentStash)
				PCPY49->currentStash = newStash;
		}
	}

	changeToSelectedStash(ptChar, isShared ? PCPY49->sharedStash : PCPY49->selfStash, 0, false);

	return 0;
}

#define DATA (*data + *curSize)
#define ADDDATA(T)		(T*)DATA;		*curSize += sizeof(T)
#define SETDATA(T,V)	*(T*)DATA = V;	*curSize += sizeof(T)

void saveStash(UnitAnyHM* ptChar, Stash* ptStash, BYTE** data, DWORD* maxSize, DWORD* curSize)
{
	//write "ST"
	SETDATA(WORD, STASH_TAG);

	//Write flags.
	SETDATA(DWORD, ptStash->flags);

	//save name
	if (ptStash->name)
	{
		int size = strlen(ptStash->name);
		if (size > 20) size = 20;
		strncpy((char*)DATA, ptStash->name, size);
		*curSize += size;
	}
	SETDATA(char, NULL);

	//Write "JM" of inventory
	SETDATA(WORD, JM_TAG);

	//Get position of counter of items in inventory
	WORD* ptNBItem = ADDDATA(WORD);
	*ptNBItem = 0;

	//Get first item
	UnitAnyHM* ptItem;
	if ((ptStash->id == PCPY49->currentStash->id) && (ptStash->isShared == PCPY49->currentStash->isShared))
		ptItem = D2COMMON_GetItemFromInventoryHM(ptChar->ptInventory);   //D2InventoryGetFirstItem(PCInventory)
	else
		ptItem = ptStash->ptListItem;

	//Write all items
	while (ptItem)
	{
		if (D2COMMON_GetItemPage(D2COMMON_GetRealItem(ptItem)) == 4)
		{
			int nbBytes = D2COMMON_SaveItem(D2COMMON_GetRealItem(ptItem), (saveBitField*)DATA, *maxSize - *curSize, 1, 1, 0);
			d2_assert(!nbBytes, "!\"Character has too many items\"", __FILE__, __LINE__);
			*curSize += nbBytes;
			(*ptNBItem)++;
		}
		ptItem = D2COMMON_GetNextItemFromInventoryHM(ptItem);
	}
}
//保存仓库页
void saveStashList(UnitAnyHM* ptChar, Stash* ptStash, BYTE** data, DWORD* maxSize, DWORD* curSize)
{
	DWORD curSizeNbStash = *curSize;
	*curSize += sizeof(DWORD);

	DWORD nbStash = 0;

	while (ptStash)
	{
		if (*curSize + 0x2000 > *maxSize)
		{
			BYTE* oldData = *data;
			*maxSize *= 2;
			*data = (BYTE*)FOG_AllocMem(ptChar->ptGame->memoryPool,*maxSize, __FILE__, __LINE__, 0);
			d2_assert(!*data, "Error : Memory allocation", __FILE__, __LINE__);
			CopyMemory(*data, oldData, *curSize);
			FOG_FreeMem(ptChar->ptGame->memoryPool, oldData, __FILE__, __LINE__, 0);
		}
		saveStash(ptChar, ptStash, data, maxSize, curSize);
		nbStash++;
		ptStash = endStashList(ptChar, ptStash->nextStash) ? NULL : ptStash->nextStash;
	}
	*(DWORD*)(*data + curSizeNbStash) = nbStash;
}

Stash* getStash(UnitAnyHM* ptChar, DWORD isShared, DWORD id)
{
	Stash* ptStash = isShared ? PCPY49->sharedStash : PCPY49->selfStash;

	while (ptStash)
	{
		if (ptStash->id == id) return ptStash;
		ptStash = ptStash->nextStash;
	}

	return NULL;
}

void setSelectedStashClient(DWORD stashId, DWORD stashFlags, DWORD flags, bool bOnlyItems)
{
	LogMsg("setSelectedStashClient ID:%d, stashFlags:%d, flags:%08X\n", stashId, stashFlags, flags);
	UnitAnyHM* ptChar = (UnitAnyHM*)D2CLIENT_GetPlayerUnit();
	Stash* newStash = getStash(ptChar, (stashFlags & 1) == 1, stashId);
	if (!newStash) do
		newStash = addStash(ptChar, (stashFlags & 1) == 1, false, newStash);
	while (newStash->id < stashId);
	newStash->flags = stashFlags;
	changeToSelectedStash(ptChar, newStash, bOnlyItems, 1);
	PCPY49->flags = flags;
}

void renameCurrentStash(UnitAnyHM* ptChar, char* name)
{
	LogMsg("renameCurrentStash : '%s'\n", name);
	Stash* stash = PCPY49->currentStash;
	int len = 0;
	if (name != NULL)
		len = strlen(name);
	if (stash->name)
		FOG_MemDeAlloc(stash->name, __FILE__, __LINE__, 0);
	if (len > 0)
	{
		stash->name = (char*)malloc(len + 1);//D2FogMemAlloc(len,__FILE__,__LINE__,0);
		strcpy(stash->name, name);
	}
	else
		stash->name = NULL;
}

/////// client
void updateSelectedStashClient(UnitAnyHM* ptChar)
{
	Stash* newStash = PCPY49->currentStash;
	if (!newStash)
		return;
	updateClient(ptChar, UC_SELECT_STASH, newStash->id, newStash->flags, PCPY49->flags);
	updateClient(ptChar, UC_PAGE_NAME, newStash->name);
}

void setCurrentStashIndex(UnitAnyHM* ptChar, int index)
{
	if (!PCPY49->currentStash)
		return;
	PCPY49->currentStash->isIndex = index >= 1;
	PCPY49->currentStash->isMainIndex = index == 2;
	updateSelectedStashClient(ptChar);
}

void selectStash(UnitAnyHM* ptChar, Stash* newStash, bool forceUpdate)
{
	if (!newStash)
		return;
	if (!forceUpdate && newStash == PCPY49->currentStash)
		return;
	LogMsg("selectStash ID:%d\tshared:%d\tforceUpdate:%d\n", newStash->id, newStash->isShared, forceUpdate);
	changeToSelectedStash(ptChar, newStash, 0, 0);
	updateSelectedStashClient(ptChar);
}

void selectPreviousStash(UnitAnyHM* ptChar)
{
	Stash* selStash = PCPY49->currentStash->previousStash;
	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void selectNextStash(UnitAnyHM* ptChar)
{
	Stash* selStash = PCPY49->currentStash;
	if (!selStash->isShared && (selStash->id >= maxSelfPages))	return;
	if (selStash->isShared && (selStash->id >= maxSharedPages)) return;

	selStash = selStash->nextStash ? selStash->nextStash : addStash(ptChar, PCPY49->showSharedStash, true, selStash);

	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void selectPreviousIndexStash(UnitAnyHM* ptChar)
{
	selectPreviousStash(ptChar);
	Stash* selStash = PCPY49->currentStash;
	while (selStash && !selStash->isIndex)
		selStash = selStash->previousStash;

	if (selStash == NULL)
	{
		selStash = PCPY49->currentStash;
		while (selStash->previousStash && ((selStash->id + 1) % nbPagesPerIndex != 0))
			selStash = selStash->previousStash;
	}

	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void selectNextIndexStash(UnitAnyHM* ptChar)
{
	selectNextStash(ptChar);
	Stash* selStash = PCPY49->currentStash;
	while (selStash && !selStash->isIndex)
		selStash = selStash->nextStash;

	if (selStash == NULL)
	{
		selStash = PCPY49->currentStash;
		while ((selStash->id + 1) % nbPagesPerIndex != 0)
		{
			if (!selStash->isShared && (selStash->id >= maxSelfPages))	break;
			if (selStash->isShared && (selStash->id >= maxSharedPages)) break;
			selStash = selStash->nextStash ? selStash->nextStash : addStash(ptChar, PCPY49->showSharedStash, true, selStash);
		}
	}
	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void selectPrevious2Stash(UnitAnyHM* ptChar)// Select first stash
{
	Stash* selStash = PCPY49->showSharedStash ? PCPY49->sharedStash : PCPY49->selfStash;
	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void selectNext2Stash(UnitAnyHM* ptChar)//select last stash
{
	Stash* selStash = PCPY49->showSharedStash ? PCPY49->sharedStash : PCPY49->selfStash;//PCPY->currentStash;
	Stash* lastStash = NULL;
	UnitAnyHM* currentStashItem = firstClassicStashItem(ptChar);
	while (selStash)
	{
		if (selStash->ptListItem || (selStash == PCPY49->currentStash) && currentStashItem) lastStash = selStash;
		selStash = selStash->nextStash;
	}
	if (!lastStash)
		lastStash = PCPY49->showSharedStash ? PCPY49->sharedStash : PCPY49->selfStash;
	if (lastStash != PCPY49->currentStash)
		selectStash(ptChar, lastStash);
}

void selectPreviousIndex2Stash(UnitAnyHM* ptChar)
{
	selectPreviousStash(ptChar);
	Stash* selStash = PCPY49->currentStash;
	while (selStash && !selStash->isMainIndex)
		selStash = selStash->previousStash;

	if (selStash == NULL)
	{
		selStash = PCPY49->currentStash;
		while (selStash->previousStash && ((selStash->id + 1) % nbPagesPerIndex2 != 0))
			selStash = selStash->previousStash;
	}

	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void selectNextIndex2Stash(UnitAnyHM* ptChar)
{
	selectNextStash(ptChar);
	Stash* selStash = PCPY49->currentStash;
	while (selStash && !selStash->isMainIndex)
		selStash = selStash->nextStash;

	if (selStash == NULL)
	{
		selStash = PCPY49->currentStash;
		while ((selStash->id + 1) % nbPagesPerIndex2 != 0)
		{
			if (!selStash->isShared && (selStash->id >= maxSelfPages))	break;
			if (selStash->isShared && (selStash->id >= maxSharedPages)) break;
			selStash = selStash->nextStash ? selStash->nextStash : addStash(ptChar, PCPY49->showSharedStash, true, selStash);
		}
	}
	if (selStash && (selStash != PCPY49->currentStash))
		selectStash(ptChar, selStash);
}

void insertStash(UnitAnyHM* ptChar)
{
	Stash* curStash = PCPY49->currentStash;
	Stash* stash = addStash(ptChar, curStash->isShared, false, curStash);
	while (stash->previousStash != curStash)
	{
		stash->flags = stash->previousStash->flags;
		stash->name = stash->previousStash->name;
		stash->ptListItem = stash->previousStash->ptListItem;
		stash = stash->previousStash;
	}
	stash->isIndex = 0;
	stash->isMainIndex = 0;
	stash->name = NULL;
	stash->ptListItem = NULL;
}

bool deleteStash(UnitAnyHM* ptChar, bool isClient)
{
	if (firstClassicStashItem(ptChar) != NULL)
		return false;
	//if (D2InventoryGetFirstItem())
	Stash* stash = PCPY49->currentStash;
	if (stash->nextStash == NULL)
	{
		stash->isIndex = 0;
		stash->isMainIndex = 0;
		stash->name = NULL;
		return true;
	}
	stash->flags = stash->nextStash->flags;
	stash->name = stash->nextStash->name;
	if (stash->nextStash->ptListItem != NULL)
	{
		LogMsg("deleteStash ID:%d\tshared:%d\tonlyItems:%d\tclient:%d\n", stash->id, stash->isShared, 1, isClient);
		changeToSelectedStash(ptChar, stash->nextStash, 1, isClient);
	}
	stash = stash->nextStash;
	while (stash->nextStash)
	{
		stash->flags = stash->nextStash->flags;
		stash->name = stash->nextStash->name;
		stash->ptListItem = stash->nextStash->ptListItem;
		stash = stash->nextStash;
	}
	stash->isIndex = 0;
	stash->isMainIndex = 0;
	stash->name = NULL;
	stash->ptListItem = NULL;
	return true;
}

void swapStash(UnitAnyHM* ptChar, Stash* curStash, Stash* swpStash)
{
	LogMsg("swapStash ID:%d\tshared:%d\tonlyItems:%d\tclient:%d\n", swpStash->id, swpStash->isShared, 1, 0);
	if (!ptChar || !curStash || !swpStash || curStash == swpStash)
		return;
	changeToSelectedStash(ptChar, swpStash, 1, 0);
	updateClient(ptChar, UC_SELECT_STASH, swpStash->id, swpStash->flags | 8, PCPY49->flags);
}

void swapStash(UnitAnyHM* ptChar, DWORD page, bool toggle)
{
	LogMsg("swap stash page = %u, toggle=%u\n", page, toggle);
	Stash* curStash = PCPY49->currentStash;
	Stash* swpStash = curStash->isShared == toggle ? PCPY49->selfStash : PCPY49->sharedStash;
	if (!swpStash)
		swpStash = addStash(ptChar, !curStash->isShared, true, swpStash);
	for (DWORD i = 0; i < page; i++)
	{
		LogMsg("swap stash : %i\n", i);
		if (swpStash->nextStash == NULL)
			addStash(ptChar, swpStash->isShared, false, swpStash);
		swpStash = swpStash->nextStash;
	}
	swapStash(ptChar, curStash, swpStash);
}

WCHAR* getDefaultStashName(UnitAnyHM* ptChar)
{
	return L"私人储藏页";
	/*if (!autoRenameStashPage)
		return getLocalString(PCPY->currentStash->isShared ? STR_SHARED_PAGE_NUMBER : STR_PERSONAL_PAGE_NUMBER);

	int onlyOneUnique = -1;
	int uniqueNameIndex = -1;
	int onlyOneSet = -1;
	int setNameIndex = -1;
	int onlyOneMisc = -1;
	int miscNameIndex = -1;

	Unit* ptItem = D2InventoryGetFirstItem(PCInventory);
	int nb = 0;
	while (ptItem)
	{
		if (D2ItemGetPage(ptItem) == 4)
		{
			if (onlyOneUnique != 0)
			{
				if (ptItem->ptItemData->quality != ITEMQUALITY_UNIQUE)
				{
					onlyOneUnique = 0;
					uniqueNameIndex = -1;
				}
				else
				{
					int uniqueId = D2GetUniqueID(ptItem);
					UniqueItemsBIN* uniqueBIN = SgptDataTables->uniqueItems + uniqueId;
					if (onlyOneUnique < 0)
					{
						onlyOneUnique = 1;
						uniqueNameIndex = uniqueBIN->uniqueNameId;
					}
					else if (setNameIndex != uniqueBIN->uniqueNameId)
					{
						onlyOneUnique = 0;
						uniqueNameIndex = -1;
					}
				}
			}

			if (onlyOneSet != 0)
			{
				if (ptItem->ptItemData->quality != ITEMQUALITY_SET)
				{
					onlyOneSet = 0;
					setNameIndex = -1;
				}
				else
				{
					int uniqueID = ptItem->ptItemData->uniqueID;
					SetItemsBIN* itemBIN = &SgptDataTables->setItems[uniqueID];
					SetsBIN* setBIN = &SgptDataTables->sets[itemBIN->setId];

					if (onlyOneSet < 0)
					{
						onlyOneSet = 1;
						setNameIndex = setBIN->setNameIndex;
					}
					else if (setNameIndex != setBIN->setNameIndex)
					{
						onlyOneSet = 0;
						setNameIndex = -1;
					}
				}
			}

			if (onlyOneMisc != 0)
			{
				if (ptItem->ptItemData->quality != ITEMQUALITY_NORMAL)
				{
					onlyOneMisc = 0;
					miscNameIndex = -1;
				}
				else
				{
					ItemsBIN* ptItemsBin = D2GetItemsBIN(ptItem->nTxtFileNo);
					if (onlyOneMisc < 0)
					{
						onlyOneMisc = 1;
						miscNameIndex = ptItemsBin->NameStr;
					}
					else if (miscNameIndex != ptItemsBin->NameStr)
					{
						onlyOneMisc = 0;
						miscNameIndex = -1;
					}
				}
			}
		}
		ptItem = D2UnitGetNextItem(ptItem);
	}

	if (onlyOneUnique == 1 && uniqueNameIndex >= 0)
		return StripGender(D2GetStringFromIndex(uniqueNameIndex));
	if (onlyOneSet == 1 && setNameIndex >= 0)
		return StripGender(D2GetStringFromIndex(setNameIndex));
	if (onlyOneMisc == 1 && miscNameIndex >= 0)
		return StripGender(D2GetStringFromIndex(miscNameIndex));

	return getLocalString(PCPY->currentStash->isShared ? STR_SHARED_PAGE_NUMBER : STR_PERSONAL_PAGE_NUMBER);*/
}

void getCurrentStashName(WCHAR* buffer, DWORD maxSize, UnitAnyHM* ptChar)
{
	if (PCPY49->currentStash->name && PCPY49->currentStash->name[0])
	{
		mbstowcs(buffer, PCPY49->currentStash->name, maxSize - 1);
	}
	else
	{
		wcsncpy(buffer, getDefaultStashName(ptChar), maxSize - 1);;
	}
	buffer[20] = NULL;
}

DWORD __stdcall carry1Limit(UnitAnyHM* ptChar, UnitAnyHM* ptItemParam, DWORD itemNum, BYTE page)
{
	//if (!ptChar) return 0;
	//UnitAnyHM* ptItem = ptItemParam ? ptItemParam : D2GameASM_GetObject_STUB(ptChar->ptGame, UNIT_ITEM, itemNum);
	//if ((page != 4) && (D2GetItemQuality(ptItem) == ITEMQUALITY_UNIQUE) && ptChar)
	//{
	//	int uniqueID = D2GetUniqueID(ptItem);
	//	if ((uniqueID >= 0) && (uniqueID < (int)SgptDataTables->nbUniqueItems))
	//	{
	//		UniqueItemsBIN* uniqueItems = SgptDataTables->uniqueItems + uniqueID;
	//		if (uniqueItems && (uniqueItems->carry1 == 1))
	//		{
	//			ItemsBIN* ptItemsBin = D2GetItemsBIN(ptItem->nTxtFileNo);
	//			Unit* ptFirstItem = D2InventoryGetFirstItem(PCInventory);
	//			if (ptItemsBin && ptFirstItem)
	//				return D2VerifIfNotCarry1(ptItem, ptItemsBin, ptFirstItem);
	//		}
	//	}
	//}
	return 0;
}

void __declspec(naked) caller_carry1Limit_111()
{
	__asm
	{
		PUSH DWORD PTR SS : [ESP + 0x08]//page
		PUSH 0//EDI
		PUSH DWORD PTR SS : [ESP + 0x0C]
		PUSH ESI//ptChar
		CALL carry1Limit
		TEST EAX, EAX
		JNZ	end_carry1Limit
		//JMP D2ItemSetPage  //这个暂时先注释，S5应该是来不及了。
	end_carry1Limit :
		ADD ESP, 0xC
		XOR EAX, EAX
		POP EDI
		POP EBX
		POP ESI
		POP EBP
		RETN 8
	}
}

void Install_MultiPageStash()
{
	Install_PlayerCustomData();
	Install_InterfaceStash();

	LogMsg("Patch D2Game for carry1 unique item. (MultiPageStash) 开始\n");

	//D2Game.dll offset CF1E3-1
	//caller_carry1Limit_111
	Patch* patch1 = new Patch(Call, D2GAME, { 0xCF1E3 - 1,0xCF1E3 - 1 }, (DWORD)caller_carry1Limit_111, 5);
	patch1->Install();

	LogMsg("Patch D2Game for carry1 unique item. (MultiPageStash) 结束\n");
}
