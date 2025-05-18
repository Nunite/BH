#include "ItemMover.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"
#include "../../D2Helpers.h"
#include "../Item/Item.h"
#include "../ChatColor/ChatColor.h"
#include "../ScreenInfo/ScreenInfo.h"
using namespace Drawing;


//用于检查分辨率有没有发生变化
int my_screenWidth = -1;
int my_screenHeight = -1;
// This module was inspired by the RedVex plugin "Item Mover", written by kaiks.
// Thanks to kaiks for sharing his code.

#define INVENTORY_WIDTH  inventoryLayout->SlotWidth
#define INVENTORY_HEIGHT inventoryLayout->SlotHeight
#define INVENTORY_LEFT   inventoryLayout->Left
#define INVENTORY_RIGHT  inventoryLayout->Right
#define INVENTORY_TOP    inventoryLayout->Top
#define INVENTORY_BOTTOM inventoryLayout->Bottom

#define STASH_WIDTH  stashLayout->SlotWidth
#define STASH_HEIGHT stashLayout->SlotHeight
#define STASH_LEFT   stashLayout->Left
#define STASH_RIGHT  stashLayout->Right
#define STASH_TOP    stashLayout->Top
#define STASH_BOTTOM stashLayout->Bottom

#define CUBE_WIDTH  cubeLayout->SlotWidth
#define CUBE_HEIGHT cubeLayout->SlotHeight
#define CUBE_LEFT   cubeLayout->Left
#define CUBE_RIGHT  cubeLayout->Right
#define CUBE_TOP    cubeLayout->Top
#define CUBE_BOTTOM cubeLayout->Bottom

#define CELL_SIZE inventoryLayout->SlotPixelHeight

std::string POTIONS[] = { "hp", "mp", "rv" , "yp"};

DWORD idBookId;
DWORD unidItemId;

bool ItemMover::Init() {
	BnetData* pData = (*p_D2LAUNCH_BnData);
	if (!pData) { return false; }
	int xpac = pData->nCharFlags & PLAYER_TYPE_EXPANSION;

	if (xpac) {
		stashLayout = p_D2CLIENT_StashLayout;
		StashItemIds = LODStashItemIds;
	}
	else {
		stashLayout = p_D2CLIENT_ClassicStashLayout;
		StashItemIds = ClassicStashItemIds;
	}
	inventoryLayout = p_D2CLIENT_InventoryLayout;
	cubeLayout = p_D2CLIENT_CubeLayout;

	if (!InventoryItemIds) {
		InventoryItemIds = new int[INVENTORY_WIDTH * INVENTORY_HEIGHT];
	}
	if (!StashItemIds) {
		StashItemIds = new int[STASH_WIDTH * STASH_HEIGHT];
	}
	if (!CubeItemIds) {
		CubeItemIds = new int[CUBE_WIDTH * CUBE_HEIGHT];
	}

	//PrintText(1, "Got positions: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
	//	INVENTORY_WIDTH,
	//	INVENTORY_HEIGHT,
	//	STASH_WIDTH,
	//	LOD_STASH_HEIGHT,
	//	CLASSIC_STASH_HEIGHT,
	//	CUBE_WIDTH,
	//	CUBE_HEIGHT,
	//	INVENTORY_LEFT,
	//	INVENTORY_TOP,
	//	STASH_LEFT,
	//	LOD_STASH_TOP,
	//	CLASSIC_STASH_TOP,
	//	CUBE_LEFT,
	//	CUBE_TOP,
	//	CELL_SIZE
	//);
	return true;
}

bool ItemMover::LoadInventory(UnitAny* unit, int xpac, int source, int sourceX, int sourceY, bool shiftState, bool ctrlState, int stashUI, int invUI) {
	bool returnValue = false;

	memset(InventoryItemIds, 0, INVENTORY_WIDTH * INVENTORY_HEIGHT * sizeof(int));
	memset(StashItemIds, 0, STASH_WIDTH * STASH_HEIGHT * sizeof(int));
	memset(CubeItemIds, 0, CUBE_WIDTH * CUBE_HEIGHT * sizeof(int));

	//if (!xpac) {
	//	for (int x = 0; x < STASH_WIDTH; x++) {
	//		for (int y = CLASSIC_STASH_HEIGHT; y < LOD_STASH_HEIGHT; y++) {
	//			StashItems[y * STASH_WIDTH + x] = (UnitAny*)NULL;
	//		}
	//	}
	//}

	unsigned int itemId = 0;
	BYTE itemXSize, itemYSize;
	bool cubeInInventory = false, cubeAnywhere = false;
	UnitAny* pTargetItem = NULL;
	for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		int* p;
		int width;
		if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY) {
			p = InventoryItemIds;
			width = INVENTORY_WIDTH;
		}
		else if (pItem->pItemData->ItemLocation == STORAGE_STASH) {
			p = StashItemIds;
			width = STASH_WIDTH;
		}
		else if (pItem->pItemData->ItemLocation == STORAGE_CUBE) {
			p = CubeItemIds;
			width = CUBE_WIDTH;
		}
		else {
			continue;
		}

		bool box = false;
		char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
		if (code[0] == 'b' && code[1] == 'o' && code[2] == 'x') {
			if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY) {
				cubeInInventory = true;
				cubeAnywhere = true;
			}
			if (pItem->pItemData->ItemLocation == STORAGE_STASH) {
				cubeAnywhere = true;
			}
			box = true;
		}

		int xStart = pItem->pObjectPath->dwPosX;
		int yStart = pItem->pObjectPath->dwPosY;
		ItemsTxt* pItemText = D2COMMON_GetItemText(pItem->dwTxtFileNo);
		BYTE xSize = pItemText->binvwidth;
		BYTE ySize = pItemText->binvheight;
		for (int x = xStart; x < xStart + xSize; x++) {
			for (int y = yStart; y < yStart + ySize; y++) {
				p[y * width + x] = pItem->dwUnitId;
				if (pItem->pItemData->ItemLocation == STORAGE_CUBE) {
					char info[MAX_PATH] = "";
					sprintf(info, "zyl CUBE: %d\r\n", p[y * width + x]);
					OutputDebugString(info);
				}

				// If you click to move the cube into itself, your character ends up in
				// the amusing (and apparently permanent) state where he has no visible
				// cube and yet is unable to pick one up. Logging out does not fix it.
				// So we disable all cube movements to be on the safe side.
				if (x == sourceX && y == sourceY && pItem->pItemData->ItemLocation == source && !box) {
					// This is the item we want to move
					itemId = pItem->dwUnitId;
					pTargetItem = pItem;
					itemXSize = xSize;
					itemYSize = ySize;
				}
			}
		}
	}

	int destination;
	if (ctrlState && shiftState && ((stashUI && cubeAnywhere) || (invUI && cubeInInventory)) && source != STORAGE_CUBE) {
		destination = STORAGE_CUBE;
	}
	else if (ctrlState) {
		destination = STORAGE_NULL;  // i.e. the ground
	}
	else if (source == STORAGE_STASH || source == STORAGE_CUBE) {
		destination = STORAGE_INVENTORY;
	}
	else if (source == STORAGE_INVENTORY && D2CLIENT_GetUIState(UI_STASH)) {
		destination = STORAGE_STASH;
	}
	else if (source == STORAGE_INVENTORY && D2CLIENT_GetUIState(UI_CUBE)) {
		destination = STORAGE_CUBE;
	}
	else {
		return false;
	}

	// Find a spot for the item in the destination container
	if (itemId > 0) {
		returnValue = FindDestination(unit, pTargetItem, xpac, destination, itemId, itemXSize, itemYSize);
	}

	FirstInit = true;
	return returnValue;
}

bool ItemMover::FindDestination(UnitAny* unit, UnitAny* pItem, int xpac, int destination, unsigned int itemId, BYTE xSize, BYTE ySize) {
	int* p;
	int width = 0, height = 0;
	if (destination == STORAGE_INVENTORY) {
		p = InventoryItemIds;
		width = INVENTORY_WIDTH;
		height = INVENTORY_HEIGHT;
	}
	else if (destination == STORAGE_STASH) {
		p = StashItemIds;
		width = STASH_WIDTH;
		height = STASH_HEIGHT;
	}
	else if (destination == STORAGE_CUBE) {
		p = CubeItemIds;
		width = CUBE_WIDTH;
		height = CUBE_HEIGHT;
	}

	bool found = false;
	int destX = 0, destY = 0, invItemId = 0;
	if (width) {
		bool first_y = true;
		// test once for stacked items
		ItemsTxt* pItemsTxt = D2COMMON_GetItemText(pItem->dwTxtFileNo);
		if (destination != STORAGE_NULL && pItemsTxt->bstackable)
		{
			int nQuantity = D2COMMON_GetUnitStat(pItem, STAT_AMMOQUANTITY, 0);
			for (UnitAny* pInvItem = unit->pInventory->pFirstItem; pInvItem; pInvItem = pInvItem->pItemData->pNextInvItem) {
				if (pInvItem->dwTxtFileNo == pItem->dwTxtFileNo)
				{
					if (pInvItem->pItemData->ItemLocation == destination)
					{
						int nTargetQuantity = D2COMMON_GetUnitStat(pInvItem, STAT_AMMOQUANTITY, 0);
						if (nQuantity + nTargetQuantity <= pItemsTxt->dwmaxstack)
						{
							destX = -1;
							destY = -1;
							invItemId = pInvItem->dwUnitId;
							found = true;
							break;
						}
						else if (nTargetQuantity != pItemsTxt->dwmaxstack)
						{
							invItemId = pInvItem->dwUnitId;
							break;
						}
					}
				}
			}
		}

		if (!found)
		{
			//bool found = false;
			//int destX = 0, destY = 0;
			//bool first_y = true;
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					bool abort = false;
					int vacancies = 0;
					for (int testx = x; testx < x + xSize && testx < width; testx++) {
						for (int testy = y; testy < y + ySize && testy < height; testy++) {
							if (p[testy * width + testx]) {
								abort = true;
								break;
							}
							else {
								vacancies++;
							}
						}
						if (abort) {
							break;
						}
					}
					if (vacancies == xSize * ySize) {
						// Found an empty spot that's big enough for the item
						found = true;
						destX = x;
						destY = y;
						break;
					}
					if (xSize == 1) {
						if (first_y) {
							if (x + 1 < width) {
								x++;
								y--;
								first_y = false;
							}
						}
						else {
							first_y = true;
							x--;
						}
					}
				} // end y loop
				if (found) {
					break;
				}
				if (xSize == 2 && x % 2 == 0 && x + 2 >= width) {
					x = 0;
				}
				else {
					x++;
				}
			} // end x loop
		}
	}
	else {
		found = true;
	}

	if (found) {
		Lock();
		if (ActivePacket.startTicks == 0) {
			ActivePacket.itemId = itemId;
			ActivePacket.x = destX;
			ActivePacket.y = destY;
			ActivePacket.startTicks = BHGetTickCount();
			ActivePacket.destination = destination;
			ActivePacket.itemTargetId = invItemId;
		}
		Unlock();
	}

	return found;
}

void ItemMover::StackItem() {
	BYTE PacketData[9] = { 0x21,0,0,0,0 };
	*reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
	*reinterpret_cast<int*>(PacketData + 5) = ActivePacket.itemTargetId;
	D2NET_SendPacket(9, 1, PacketData);
}

void ItemMover::PickUpItem() {
	BYTE PacketData[5] = { 0x19,0,0,0,0 };
	*reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
	D2NET_SendPacket(5, 1, PacketData);
}

void ItemMover::PutItemInContainer() {
	BYTE PacketData[17] = { 0x18,0,0,0,0 };
	*reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
	*reinterpret_cast<int*>(PacketData + 5) = ActivePacket.x;
	*reinterpret_cast<int*>(PacketData + 9) = ActivePacket.y;
	*reinterpret_cast<int*>(PacketData + 13) = ActivePacket.destination;
	D2NET_SendPacket(17, 1, PacketData);
}

void ItemMover::PutItemOnGround() {
	BYTE PacketData[5] = { 0x17,0,0,0,0 };
	*reinterpret_cast<int*>(PacketData + 1) = ActivePacket.itemId;
	D2NET_SendPacket(5, 1, PacketData);
}

void ItemMover::AutoToBelt()
{
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return;

	/*UnitAny* test1 = D2COMMON_GetBeltItem(unit->pInventory, 0);
	DWORD test2 = D2COMMON_GetFreeBeltSlot(unit->pInventory,test1, 0);*/

	//"hp", "mp", "rv"
		//循环查找背包里面的药
	for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY || pItem->pItemData->ItemLocation == STORAGE_CUBE) {   //只取背包和盒子里面的
			char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
			if (code[0] == 'h' && code[1] == 'p') {
				DWORD itemId = pItem->dwUnitId;  //红药
				//试一下这个不知道是不是填充的数据包
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
			if (code[0] == 'm' && code[1] == 'p') {
				DWORD itemId = pItem->dwUnitId;  //蓝药
				//试一下这个不知道是不是填充的数据包
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
			if (code[0] == 'r' && code[1] == 'v') {
				DWORD itemId = pItem->dwUnitId;  //紫药
				//试一下这个不知道是不是填充的数据包
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
			if (code[0] == 'y' && code[1] == 'p' && code[2] == 's') {
				DWORD itemId = pItem->dwUnitId;  //解毒药
				//试一下这个不知道是不是填充的数据包
				BYTE PacketData[5] = { 0x63, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = itemId;
				D2NET_SendPacket(5, 0, PacketData);
			}
		}
	}
}

void ItemMover::OnLeftClick(bool up, int x, int y, bool* block) {
	BnetData* pData = (*p_D2LAUNCH_BnData);
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	bool shiftState = ((GetKeyState(VK_LSHIFT) & 0x80) || (GetKeyState(VK_RSHIFT) & 0x80));

	if (up || !pData || !unit || !shiftState || D2CLIENT_GetCursorItem() > 0 || (!D2CLIENT_GetUIState(UI_INVENTORY) && !D2CLIENT_GetUIState(UI_STASH) && !D2CLIENT_GetUIState(UI_CUBE) && !D2CLIENT_GetUIState(UI_NPCSHOP)) ||
		!Init()) {
		return;
	}

	//Init();

	unidItemId = 0;
	idBookId = 0;

	int xpac = pData->nCharFlags & PLAYER_TYPE_EXPANSION;

	int mouseX, mouseY;

	for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
		char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
		if ((pItem->pItemData->dwFlags & ITEM_IDENTIFIED) <= 0) {
			int xStart = pItem->pObjectPath->dwPosX;
			int yStart = pItem->pObjectPath->dwPosY;
			BYTE xSize = D2COMMON_GetItemText(pItem->dwTxtFileNo)->binvwidth;
			BYTE ySize = D2COMMON_GetItemText(pItem->dwTxtFileNo)->binvheight;
			if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY) {
				mouseX = (*p_D2CLIENT_MouseX - INVENTORY_LEFT) / CELL_SIZE;
				mouseY = (*p_D2CLIENT_MouseY - INVENTORY_TOP) / CELL_SIZE;
			}
			else if (pItem->pItemData->ItemLocation == STORAGE_STASH) {
				mouseX = (*p_D2CLIENT_MouseX - STASH_LEFT) / CELL_SIZE;
				mouseY = (*p_D2CLIENT_MouseY - STASH_TOP) / CELL_SIZE;
			}
			else if (pItem->pItemData->ItemLocation == STORAGE_CUBE) {
				mouseX = (*p_D2CLIENT_MouseX - CUBE_LEFT) / CELL_SIZE;
				mouseY = (*p_D2CLIENT_MouseY - CUBE_TOP) / CELL_SIZE;
			}
			for (int x = xStart; x < xStart + xSize; x++) {
				for (int y = yStart; y < yStart + ySize; y++) {
					if (x == mouseX && y == mouseY) {
						if ((pItem->pItemData->ItemLocation == STORAGE_STASH && !D2CLIENT_GetUIState(UI_STASH)) || (pItem->pItemData->ItemLocation == STORAGE_CUBE && !D2CLIENT_GetUIState(UI_CUBE))) {
							return;
						}
						unidItemId = pItem->dwUnitId;
					}
				}
			}
		}

		BOOL bIsIdBook = code[0] == 'i' && code[1] == 'b' && code[2] == 'k';
		BOOL bIsUnlimitedIdBook = code[0] == 'r' && code[1] == 'i' && code[2] == 'd';
		BOOL bItemIsInInventory = pItem->pItemData->ItemLocation == STORAGE_INVENTORY;
		if (bItemIsInInventory && (bIsUnlimitedIdBook || (bIsIdBook && D2COMMON_GetUnitStat(pItem, STAT_AMMOQUANTITY, 0) > 0))) {
			idBookId = pItem->dwUnitId;
		}
		if (unidItemId > 0 && idBookId > 0) {
			BYTE PacketData[13] = { 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			*reinterpret_cast<int*>(PacketData + 1) = idBookId;
			*reinterpret_cast<WORD*>(PacketData + 5) = (WORD)unit->pPath->xPos;
			*reinterpret_cast<WORD*>(PacketData + 9) = (WORD)unit->pPath->yPos;
			D2NET_SendPacket(13, 0, PacketData);
			*block = true;
			return;
		}
	}
}

void ItemMover::OnRightClick(bool up, int x, int y, bool* block) {
	BnetData* pData = (*p_D2LAUNCH_BnData);
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	bool shiftState = ((GetKeyState(VK_LSHIFT) & 0x80) || (GetKeyState(VK_RSHIFT) & 0x80));
	bool ctrlState = ((GetKeyState(VK_LCONTROL) & 0x80) || (GetKeyState(VK_RCONTROL) & 0x80));
	if (up || !pData || !unit || !(shiftState || ctrlState) ||
		!Init()) {
		return;
	}

	//Init();

	//int xpac = pData->nCharFlags & PLAYER_TYPE_EXPANSION;

	//int inventoryRight = INVENTORY_LEFT + (CELL_SIZE * INVENTORY_WIDTH);
	//int inventoryBottom = INVENTORY_TOP + (CELL_SIZE * INVENTORY_HEIGHT);
	//int stashRight = STASH_LEFT + (CELL_SIZE * STASH_WIDTH);
	//int stashTop = xpac ? LOD_STASH_TOP : CLASSIC_STASH_TOP;
	//int stashHeight = xpac ? LOD_STASH_HEIGHT : CLASSIC_STASH_HEIGHT;
	//int stashBottom = stashTop + (CELL_SIZE * stashHeight);
	//int cubeRight = CUBE_LEFT + (CELL_SIZE * CUBE_WIDTH);
	//int cubeBottom = CUBE_TOP + (CELL_SIZE * CUBE_HEIGHT);

	int source, sourceX, sourceY;
	int invUI = D2CLIENT_GetUIState(UI_INVENTORY);
	int stashUI = D2CLIENT_GetUIState(UI_STASH);
	int cubeUI = D2CLIENT_GetUIState(UI_CUBE);
	if ((invUI || stashUI || cubeUI) && x >= INVENTORY_LEFT && x <= INVENTORY_RIGHT && y >= INVENTORY_TOP && y <= INVENTORY_BOTTOM) {
		source = STORAGE_INVENTORY;
		sourceX = (x - INVENTORY_LEFT) / CELL_SIZE;
		sourceY = (y - INVENTORY_TOP) / CELL_SIZE;
	}
	else if (stashUI && x >= STASH_LEFT && x <= STASH_RIGHT && y >= STASH_TOP && y <= STASH_BOTTOM) {
		source = STORAGE_STASH;
		sourceX = (x - STASH_LEFT) / CELL_SIZE;
		sourceY = (y - STASH_TOP) / CELL_SIZE;
	}
	else if (cubeUI && x >= CUBE_LEFT && x <= CUBE_RIGHT && y >= CUBE_TOP && y <= CUBE_BOTTOM) {
		source = STORAGE_CUBE;
		sourceX = (x - CUBE_LEFT) / CELL_SIZE;
		sourceY = (y - CUBE_TOP) / CELL_SIZE;
	}
	else {
		return;
	}

	bool moveItem = LoadInventory(unit, 0, source, sourceX, sourceY, shiftState, ctrlState, stashUI, invUI);
	if (moveItem) {
		PickUpItem();
	}
	*block = true;
}

void ItemMover::LoadConfig() {
	BH::config->ReadKey("Use TP Tome", "VK_N", TpKey);
	BH::config->ReadKey("Use TP Tome Back", "VK_BACK", TpBackKey);
	BH::config->ReadKey("Use Exit Game", "VK_NUMPADSUBTRACT", ExitGameKey);
	BH::config->ReadKey("Use Healing Potion", "VK_1", HealKey);
	BH::config->ReadKey("Use Mana Potion", "VK_2", ManaKey);
	BH::config->ReadKey("Use Rejuv Potion", "VK_3", JuvKey);
	BH::config->ReadKey("Use Antidote Potion", "VK_4", YpsKey);
	BH::config->ReadKey("Use Potion To Belt", "VK_OEM_3", BeltKey); //自动填充腰带

	BH::config->ReadInt("Low TP Warning", tp_warn_quantity);
	// BH::config->ReadToggle("Quick Cast", "None", false, ScreenInfo::Toggles["Quick Cast"]);
	// BH::config->ReadToggle("Skill Bar", "None", false, ScreenInfo::Toggles["Skill Bar"]);
	// BH::config->ReadToggle("Skill Bar Disable", "None", false, ScreenInfo::Toggles["Skill Bar Disable"]);
	// BH::config->ReadToggle("Buff Timers", "None", false, ScreenInfo::Toggles["Buff Timers"]);
	//BH::config->ReadInt("StashLeftFix", BH::stash_left_fix,0);
}

void ItemMover::OnLoad() {
	LoadConfig();
	AutoBackTown = false;
	Drawing::Texthook* colored_text;

	settingsTab = new Drawing::UITab("增强功能", BH::settingsUI);

	unsigned int x = 8;
	unsigned int y = 7;
	new Drawing::Texthook(settingsTab, x, y, "鼠标点击可自定义快捷键 (按esc清除快捷键)");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &TpKey, "快速开门:       ");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &TpBackKey, "快速回城:       ");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &ExitGameKey, "快速退出:       ");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &HealKey, "使用红药:       ");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &ManaKey, "使用蓝药:       ");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &JuvKey, "使用紫药:       ");
	new Drawing::Keyhook(settingsTab, x, (y += 15), &YpsKey, "使用解药:       ");  //解毒
	new Drawing::Keyhook(settingsTab, x, (y += 15), &BeltKey, "填充腰带:       ");
	int keyhook_x = 200;
	new Checkhook(settingsTab, 4, (y += 15), &ChatColor::Toggles["Merc Protect"].state, "佣兵保护");
	new Keyhook(settingsTab, keyhook_x, (y + 2), &ChatColor::Toggles["Merc Protect"].toggle, "");
	new Checkhook(settingsTab, 4, (y += 15), &ChatColor::Toggles["Merc Boring"].state, "佣兵吐槽");
	new Keyhook(settingsTab, keyhook_x, (y + 2), &ChatColor::Toggles["Merc Boring"].toggle, "");
	new Checkhook(settingsTab, 4, (y += 15), &ChatColor::Toggles["Rune Number"].state, "符文数字显示");
	new Keyhook(settingsTab, keyhook_x, (y + 2), &ChatColor::Toggles["Rune Number"].toggle, "");
	new Checkhook(settingsTab, 4, (y += 15), &ChatColor::Toggles["Show Money"].state, "贪婪模式");
	new Keyhook(settingsTab, keyhook_x, (y + 2), &ChatColor::Toggles["Show Money"].toggle, "");
	new Checkhook(settingsTab, 4, (y += 15), &ChatColor::Toggles["Death Back"].state, "死亡立即回城");
	new Keyhook(settingsTab, keyhook_x, (y + 2), &ChatColor::Toggles["Death Back"].toggle, "");
	// y += 15;
	// new Drawing::Checkhook(settingsTab, 4, y, &ScreenInfo::Toggles["Quick Cast"].state, "Quick Cast");
	// new Drawing::Keyhook(settingsTab, keyhook_x, y + 2, &ScreenInfo::Toggles["Quick Cast"].toggle, "");
	// y += 15;

	// new Drawing::Checkhook(settingsTab, 4, y, &ScreenInfo::Toggles["Skill Bar"].state, "Skill Bar");
	// new Drawing::Keyhook(settingsTab, keyhook_x, y + 2, &ScreenInfo::Toggles["Skill Bar"].toggle, "");
	// y += 15;

	// new Drawing::Checkhook(settingsTab, 4 + 15, y, &ScreenInfo::Toggles["Skill Bar Disable"].state, "Disable along Quick Cast");
	// new Drawing::Keyhook(settingsTab, keyhook_x, y + 2, &ScreenInfo::Toggles["Skill Bar Disable"].toggle, "");
	// y += 15;

	// new Drawing::Checkhook(settingsTab, 4, y, &ScreenInfo::Toggles["Buff Timers"].state, "Always show Buff Timers");
	// new Drawing::Keyhook(settingsTab, keyhook_x, y + 2, &ScreenInfo::Toggles["Buff Timers"].toggle, "");


	/*y += 15;
	y += 15;

	new Drawing::Texthook(settingsTab, x, (y), "物品快速移动说明");
	colored_text = new Drawing::Texthook(settingsTab, x, (y += 15),
		"Shift+左键 如果鉴定书在背包，就可以快速鉴定物品");
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(settingsTab, x, (y += 15),
		"Shift+右键 在背包和打开的箱子（或打开的盒子）之间移动");
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(settingsTab, x, (y += 15),
		"Ctrl+右键 把物品扔地上");
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(settingsTab, x, (y += 15),
		"Ctrl+shift+右键 移动物品到关闭着的盒子");
	colored_text->SetColor(Gold);*/

	colored_text = new Drawing::Texthook(settingsTab, x, (y += 15),
		"");
	colored_text->SetColor(Gold);

}


void ItemMover::OnKey(bool up, BYTE key, LPARAM lParam, bool* block) {
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	
	if (!unit)
		return;
	bool shiftState = ((GetKeyState(VK_LSHIFT) & 0x80) || (GetKeyState(VK_RSHIFT) & 0x80));
	//if (shiftState) return;  //按了shift就不处理,继续走下去,主要用于佣兵喝药

	if (!up && (key == HealKey || key == ManaKey || key == JuvKey || key == YpsKey)) {
		int idx = key == YpsKey ? 3 : key == JuvKey ? 2 : key == ManaKey ? 1 : 0;
		std::string startChars = POTIONS[idx];
		char minPotion = 127;
		DWORD minItemId = 0;
		bool isBelt = false;
		for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
			if (
				(!shiftState && pItem->pItemData->ItemLocation == STORAGE_INVENTORY) ||
				(pItem->pItemData->ItemLocation == STORAGE_NULL && pItem->pItemData->NodePage == NODEPAGE_BELTSLOTS)) {
				char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
				if (code[0] == startChars[0] && code[1] == startChars[1] && code[2] < minPotion) {
					minPotion = code[2];
					minItemId = pItem->dwUnitId;
					isBelt = pItem->pItemData->NodePage == NODEPAGE_BELTSLOTS;
					break;
				}
				//else if (code[0] == 'y'&& code[1] == 'p'&& code[2] == 's') {  //解毒药
				//	minItemId = pItem->dwUnitId;
				//	isBelt = pItem->pItemData->NodePage == NODEPAGE_BELTSLOTS;
				//}
			}
			//char *code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
			//if (code[0] == 'b' && code[1] == 'o' && code[2] == 'x') {
			//	// Hack to pick up cube to fix cube-in-cube problem
			//	BYTE PacketDataCube[5] = {0x19,0,0,0,0};
			//	*reinterpret_cast<int*>(PacketDataCube + 1) = pItem->dwUnitId;
			//	D2NET_SendPacket(5, 1, PacketDataCube);
			//	break;
			//}
		}
		
		if (minItemId > 0) {
			if (isBelt) {
				BYTE PacketData[13] = { 0x26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = minItemId;
				if (shiftState) {   //按shift
					if (ChatColor::Toggles["Merc Boring"].state)
						PrintText(White, "佣兵：听我说谢谢你~");
					*reinterpret_cast<int*>(PacketData + 5) = 1;  //是否给佣兵吃药,1是给，0是不给
				}
				D2NET_SendPacket(13, 0, PacketData);
			}
			else {
				//PrintText(1, "Sending packet %d, %d, %d", minItemId, unit->pPath->xPos, unit->pPath->yPos);
				BYTE PacketData[13] = { 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = minItemId;
				*reinterpret_cast<WORD*>(PacketData + 5) = (WORD)unit->pPath->xPos;
				*reinterpret_cast<WORD*>(PacketData + 9) = (WORD)unit->pPath->yPos;
				D2NET_SendPacket(13, 0, PacketData);
			}
			/**block = true;*/
		}
		Task::Enqueue([=]()->void {
			Sleep(1000);  //停1秒试试看
			AutoToBelt();
		});
		*block = true;  //设置了喝药快捷键，如果没有蓝就不喝红。
	}
	else if (!up && (key == TpKey || key == TpBackKey)) {
			AutoBackTown = false;
			if (key == TpBackKey) {
				AutoBackTown = true;
			}
			DWORD tpId = 0;
			int tp_quantity = 0;
			for (UnitAny* pItem = unit->pInventory->pFirstItem; pItem; pItem = pItem->pItemData->pNextInvItem) {
				if (pItem->pItemData->ItemLocation == STORAGE_INVENTORY) {
					char* code = D2COMMON_GetItemText(pItem->dwTxtFileNo)->szCode;
					if (code[0] == 'r' && code[1] == 't' && code[2] == 'p') {
						tpId = pItem->dwUnitId;
						break;
					}else if ((code[0] == 't' && code[1] == 'b' && code[2] == 'k')) {
						tp_quantity = D2COMMON_GetUnitStat(pItem, STAT_AMMOQUANTITY, 0);
						if (tp_quantity > 0) {
							tpId = pItem->dwUnitId;
							break;
						}
					}
				}
			}
			if (tpId > 0) {
				BYTE PacketData[13] = { 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				*reinterpret_cast<int*>(PacketData + 1) = tpId;
				*reinterpret_cast<WORD*>(PacketData + 5) = (WORD)unit->pPath->xPos;
				*reinterpret_cast<WORD*>(PacketData + 9) = (WORD)unit->pPath->yPos;
				if (tp_quantity < tp_warn_quantity) {
					PrintText(Red, "TP tome is running low!");
				}
				D2NET_SendPacket(13, 0, PacketData);
				*block = true;
			}
	}
	else if (!up && key == BeltKey) {  //自动填充腰带
			AutoToBelt();
			//*block = true;   //不让它继续向下触发按键事件
	}
	else if (!up && key == ExitGameKey) {  //快速退出游戏
		*p_D2CLIENT_ExitAppFlag = 0;
		SendMessage(D2GFX_GetHwnd(), WM_CLOSE, 0, 0);
		*block = true;   //不让它继续向下触发按键事件
	}
}

void ItemMover::OnGamePacketRecv(BYTE* packet, bool* block) {
	switch (packet[0])
	{
	case 0x3F:
	{
		// We get this packet after our cursor change. Will only ID if we found book and item previously. packet[1] = 0 guarantees the cursor is changing to "id ready" state.
		if (packet[1] == 0 && idBookId > 0 && unidItemId > 0) {
			BYTE PacketData[9] = { 0x27,0,0,0,0,0,0,0,0 };
			*reinterpret_cast<int*>(PacketData + 1) = unidItemId;
			*reinterpret_cast<int*>(PacketData + 5) = idBookId;
			D2NET_SendPacket(9, 0, PacketData);
			*block = true;
			// Reseting variables after we ID an item so the next ID works.
			unidItemId = 0;
			idBookId = 0;
		}
		break;
	}
	case 0x9c:
	{
		// We get this packet after placing an item in a container or on the ground
		if (FirstInit) {
			BYTE action = packet[1];
			unsigned int itemId = *(unsigned int*)&packet[4];
			Lock();
			if (itemId == ActivePacket.itemId) {
				//PrintText(1, "Placed item id %d", itemId);
				ActivePacket.itemId = 0;
				ActivePacket.itemTargetId = 0;
				ActivePacket.x = 0;
				ActivePacket.y = 0;
				ActivePacket.startTicks = 0;
				ActivePacket.destination = 0;
			}
			Unlock();
		}

		if ((*BH::MiscToggles2)["Advanced Item Display"].state) {
			bool success = true;
			ItemInfo item = {};
			ParseItem((unsigned char*)packet, &item, &success);
			//PrintText(1, "Item packet: %s, %s, %X, %d, %d", item.name.c_str(), item.code, item.attrs->flags, item.sockets, GetDefense(&item));
			if ((item.action == ITEM_ACTION_NEW_GROUND || item.action == ITEM_ACTION_OLD_GROUND) && success) {
				bool showOnMap = false;
				auto color = UNDEFINED_COLOR;

				for (vector<Rule*>::iterator it = MapRuleList.begin(); it != MapRuleList.end(); it++) {
					// skip map and notification if ping level requirement is not met
					int filterLevel = Item::GetFilterLevel();
					if (filterLevel != 0 && (*it)->action.pingLevel < filterLevel && (*it)->action.pingLevel != -1) continue;

					if ((*it)->Evaluate(NULL, &item)) {
						auto action_color = (*it)->action.notifyColor;
						// never overwrite color with an undefined color. never overwrite a defined color with dead color.
						if (action_color != UNDEFINED_COLOR && (action_color != DEAD_COLOR || color == UNDEFINED_COLOR))
							color = action_color;
						showOnMap = true;
						// break unless %CONTINUE% is used
						if ((*it)->action.stopProcessing) break;
					}
				}
				//PrintText(1, "Item on ground: %s, %s, %s, %X", item.name.c_str(), item.code, item.attrs->category.c_str(), item.attrs->flags);
				if (showOnMap && !(*BH::MiscToggles2)["Item Detailed Notifications"].state) {
					if (!IsTown(GetPlayerArea())) {  //run tracker相关,只要有提示的都记录一下试试看
						ScreenInfo::AddDrop(item.name.c_str(), item.x, item.y);
					}
					if (color == UNDEFINED_COLOR) {
						color = ItemColorFromQuality(item.quality);
					}
					if ((*BH::MiscToggles2)["Item Drop Notifications"].state &&
						item.action == ITEM_ACTION_NEW_GROUND &&
						color != DEAD_COLOR
						) {
						PrintText(color, "%s%s Dropped",
							item.name.c_str(),
							(*BH::MiscToggles2)["Verbose Notifications"].state ? " \377c5drop" : ""
						);
					}
					if ((*BH::MiscToggles2)["Item Close Notifications"].state &&
						item.action == ITEM_ACTION_OLD_GROUND &&
						color != DEAD_COLOR
						) {
						PrintText(color, "%s%s",
							item.name.c_str(),
							(*BH::MiscToggles2)["Verbose Notifications"].state ? " \377c5close" : ""
						);
					}
				}
				else if (!showOnMap) {
					for (vector<Rule*>::iterator it = RuleList.begin(); it != RuleList.end(); it++) {
						if ((*it)->Evaluate(NULL, &item)) {
							if ((*it)->action.name.length() == 0 && Item::GetFilterLevel() > 0) {
								*block = true;
							}
							if ((*it)->action.stopProcessing) break;
						}
					}
				}
			}
		}
		break;
	}
	case 0x9d:
	{
		// We get this packet after picking up an item
		if (FirstInit) {
			BYTE action = packet[1];
			unsigned int itemId = *(unsigned int*)&packet[4];
			Lock();
			if (itemId == ActivePacket.itemId) {
				//PrintText(2, "Picked up item id %d", itemId);
				if (ActivePacket.itemTargetId != 0) {
					StackItem();
					if (ActivePacket.x != -1 && ActivePacket.y != -1) {
						PutItemInContainer();
					}
					ActivePacket.itemId = 0;
					ActivePacket.itemTargetId = 0;
					ActivePacket.x = 0;
					ActivePacket.y = 0;
					ActivePacket.startTicks = 0;
					ActivePacket.destination = 0;
				}
				else if (ActivePacket.destination == STORAGE_NULL) {
					PutItemOnGround();
				}
				else {
					PutItemInContainer();
				}
			}
			Unlock();
		}
		break;
	}
	case 0x60:  //by zyl 自动回城
	{
		if (!AutoBackTown) return;   //不自动回城就直接返回
		if (packet[1] == 0x00) { //00限定只能走通向城里的传送门
			BYTE castMove[9] = { 0x13 };
			*(DWORD*)&castMove[1] = 2;
			*(DWORD*)&castMove[5] = *(DWORD*)&packet[3]; // portal ID
			D2NET_SendPacket(sizeof(castMove), 0, castMove);
			AutoBackTown = false;
		}
	}
	default:
		break;
	}
	return;
}

//void AutoBackToTown(BYTE* aPacket)
//{
//	//PrintText(Blue, "%s", "RecvCommand...");
//	if (aPacket[0] == 0x60 && aPacket[1] == 0x00) //00限定只能走通向城里的传送门
//	{
//		PrintText(Blue, "%s", "RecvCommand...");
//		BYTE castMove[9] = { 0x13 };
//		*(DWORD*)&castMove[1] = 2;
//		*(DWORD*)&castMove[5] = *(DWORD*)&aPacket[3]; // portal ID
//		D2NET_SendPacket(sizeof(castMove), 0, castMove);
//
//
//	}
//}

void ItemMover::OnGameExit() {
	ActivePacket.itemId = 0;
	ActivePacket.itemTargetId = 0;
	ActivePacket.x = 0;
	ActivePacket.y = 0;
	ActivePacket.startTicks = 0;
	ActivePacket.destination = 0;
}

void ItemMover::OnLoop()
{
	//分辩率有发生变化的话
	if (my_screenWidth!=*p_D2CLIENT_ScreenSizeX|| my_screenHeight != *p_D2CLIENT_ScreenSizeY)
	{
		//if (my_screenWidth != -1 && my_screenHeight != -1) {
			//对"ZYLPD2设置"按钮做一个位置调整(X暂不调整)			
			BH::settingsUI->SetMinimizedY(*p_D2CLIENT_ScreenSizeY - TITLE_BAR_HEIGHT);
		//}
		//Init();  //初始化一次
		my_screenWidth = *p_D2CLIENT_ScreenSizeX;
		my_screenHeight = *p_D2CLIENT_ScreenSizeY;
		//BH::settingsUI->SetMinimizedX(1);
		//BH::settingsUI->SetMinimizedY();
	}
}


// Code for reading the 0x9c bitstream (borrowed from heroin_glands)
void ParseItem(const unsigned char* data, ItemInfo* item, bool* success) {
	*success = true;
	try {
		BitReader reader(data);
		unsigned long packet = reader.read(8);
		item->action = reader.read(8);
		unsigned long messageSize = reader.read(8);
		item->category = reader.read(8); // item type
		item->id = reader.read(32);

		if (packet == 0x9d) {
			reader.read(32);
			reader.read(8);
		}

		item->equipped = reader.readBool();
		reader.readBool();
		reader.readBool();
		item->inSocket = reader.readBool();
		item->identified = reader.readBool();
		reader.readBool();
		item->switchedIn = reader.readBool();
		item->switchedOut = reader.readBool();

		item->broken = reader.readBool();
		reader.readBool();
		item->potion = reader.readBool();
		item->hasSockets = reader.readBool();
		reader.readBool();
		item->inStore = reader.readBool();
		item->notInSocket = reader.readBool();
		reader.readBool();

		item->ear = reader.readBool();
		item->startItem = reader.readBool();
		reader.readBool();
		reader.readBool();
		reader.readBool();
		item->simpleItem = reader.readBool();
		item->ethereal = reader.readBool();
		reader.readBool();

		item->personalized = reader.readBool();
		item->gambling = reader.readBool();
		item->runeword = reader.readBool();
		reader.read(5);

		item->version = static_cast<unsigned int>(reader.read(8));

		reader.read(2);
		unsigned long destination = reader.read(3);

		item->ground = (destination == 0x03);

		if (item->ground) {
			item->x = reader.read(16);
			item->y = reader.read(16);
		}
		else {
			item->directory = reader.read(4);
			item->x = reader.read(4);
			item->y = reader.read(3);
			item->container = static_cast<unsigned int>(reader.read(4));
		}

		item->unspecifiedDirectory = false;

		if (item->action == ITEM_ACTION_TO_STORE || item->action == ITEM_ACTION_FROM_STORE) {
			long container = static_cast<long>(item->container);
			container |= 0x80;
			if (container & 1) {
				container--; //remove first bit
				item->y += 8;
			}
			item->container = static_cast<unsigned int>(container);
		}
		else if (item->container == CONTAINER_UNSPECIFIED) {
			if (item->directory == EQUIP_NONE) {
				if (item->inSocket) {
					//y is ignored for this container type, x tells you the index
					item->container = CONTAINER_ITEM;
				}
				else if (item->action == ITEM_ACTION_PLACE_BELT || item->action == ITEM_ACTION_REMOVE_BELT) {
					item->container = CONTAINER_BELT;
					item->y = item->x / 4;
					item->x %= 4;
				}
			}
			else {
				item->unspecifiedDirectory = true;
			}
		}

		if (item->ear) {
			item->earClass = static_cast<BYTE>(reader.read(3));
			item->earLevel = reader.read(7);
			item->code[0] = 'e';
			item->code[1] = 'a';
			item->code[2] = 'r';
			item->code[3] = 0;
			for (std::size_t i = 0; i < 16; i++) {
				char letter = static_cast<char>(reader.read(7));
				if (letter == 0) {
					break;
				}
				item->earName.push_back(letter);
			}
			item->attrs = ItemAttributeMap[item->code];
			item->name = item->attrs->name;
			item->width = item->attrs->width;
			item->height = item->attrs->height;
			//PrintText(1, "Ear packet: %s, %s, %d, %d", item->earName.c_str(), item->code, item->earClass, item->earLevel);
			return;
		}

		for (std::size_t i = 0; i < 4; i++) {
			item->code[i] = static_cast<char>(reader.read(8));
		}
		item->code[3] = 0;

		if (ItemAttributeMap.find(item->code) == ItemAttributeMap.end()) {
			HandleUnknownItemCode(item->code, "from packet");
			*success = false;
			return;
		}
		item->attrs = ItemAttributeMap[item->code];
		item->name = item->attrs->name;
		item->width = item->attrs->width;
		item->height = item->attrs->height;

		item->isGold = (item->code[0] == 'g' && item->code[1] == 'l' && item->code[2] == 'd');

		if (item->isGold) {
			bool big_pile = reader.readBool();
			if (big_pile) {
				item->amount = reader.read(32);
			}
			else {
				item->amount = reader.read(12);
			}
			return;
		}

		item->usedSockets = (BYTE)reader.read(3);

		if (item->simpleItem || item->gambling) {
			return;
		}

		item->level = (BYTE)reader.read(7);
		item->quality = static_cast<unsigned int>(reader.read(4));

		item->hasGraphic = reader.readBool();;
		if (item->hasGraphic) {
			item->graphic = reader.read(3);
		}

		item->hasColor = reader.readBool();
		if (item->hasColor) {
			item->color = reader.read(11);
		}

		if (item->identified) {
			switch (item->quality) {
			case ITEM_QUALITY_INFERIOR:
				item->prefix = reader.read(3);
				break;

			case ITEM_QUALITY_SUPERIOR:
				item->superiority = static_cast<unsigned int>(reader.read(3));
				break;

			case ITEM_QUALITY_MAGIC:
				item->prefix = reader.read(11);
				item->suffix = reader.read(11);
				break;

			case ITEM_QUALITY_CRAFT:
			case ITEM_QUALITY_RARE:
				item->prefix = reader.read(8) - 156;
				item->suffix = reader.read(8) - 1;
				break;

			case ITEM_QUALITY_SET:
				item->setCode = reader.read(12);
				break;

			case ITEM_QUALITY_UNIQUE:
				if (item->code[0] != 's' || item->code[1] != 't' || item->code[2] != 'd') { //standard of heroes exception?
					item->uniqueCode = reader.read(12);
				}
				break;
			}
		}

		if (item->quality == ITEM_QUALITY_RARE || item->quality == ITEM_QUALITY_CRAFT) {
			for (unsigned long i = 0; i < 3; i++) {
				if (reader.readBool()) {
					item->prefixes.push_back(reader.read(11));
				}
				if (reader.readBool()) {
					item->suffixes.push_back(reader.read(11));
				}
			}
		}

		if (item->runeword) {
			item->runewordId = reader.read(12);
			item->runewordParameter = reader.read(4);
		}

		if (item->personalized) {
			for (std::size_t i = 0; i < 16; i++) {
				char letter = static_cast<char>(reader.read(7));
				if (letter == 0) {
					break;
				}
				item->personalizedName.push_back(letter);
			}
			//PrintText(1, "Personalized packet: %s, %s", item->personalizedName.c_str(), item->code);
		}

		item->isArmor = (item->attrs->flags & ITEM_GROUP_ALLARMOR) > 0;
		item->isWeapon = (item->attrs->flags & ITEM_GROUP_ALLWEAPON) > 0;

		if (item->isArmor) {
			item->defense = reader.read(11) - 10;
		}

		/*if(entry.throwable)
		{
			reader.read(9);
			reader.read(17);
		} else */
		//special case: indestructible phase blade
		if (item->code[0] == '7' && item->code[1] == 'c' && item->code[2] == 'r') {
			reader.read(8);
		}
		else if (item->isArmor || item->isWeapon) {
			item->maxDurability = reader.read(8);
			item->indestructible = item->maxDurability == 0;
			/*if (!item->indestructible) {
				item->durability = reader.read(8);
				reader.readBool();
			}*/
			//D2Hackit always reads it, hmmm. Appears to work.
			item->durability = reader.read(8);
			reader.readBool();
		}

		if (item->hasSockets) {
			item->sockets = (BYTE)reader.read(4);
		}

		if (!item->identified) {
			return;
		}

		if (item->attrs->stackable) {
			if (item->attrs->useable) {
				reader.read(5);
			}
			item->amount = reader.read(9);
		}

		if (item->quality == ITEM_QUALITY_SET) {
			unsigned long set_mods = reader.read(5);
		}

		while (true) {
			unsigned long stat_id = reader.read(9);
			if (stat_id == 0x1ff) {
				break;
			}
			ItemProperty prop = {};
			if (!ProcessStat(stat_id, reader, prop) &&
				!(*BH::MiscToggles2)["Suppress Invalid Stats"].state) {
				//PrintText(1, "Invalid stat: %d, %c%c%c", stat_id, item->code[0], item->code[1], item->code[2]);
				*success = false;
				break;
			}
			item->properties.push_back(prop);
		}
	}
	catch (int e) {
		PrintText(1, "Int exception parsing item: %c%c%c, %d", item->code[0], item->code[1], item->code[2], e);
	}
	catch (std::exception const& ex) {
		PrintText(1, "Exception parsing item: %c%c%c, %s", item->code[0], item->code[1], item->code[2], ex.what());
	}
	catch (...) {
		PrintText(1, "Miscellaneous exception parsing item: %c%c%c", item->code[0], item->code[1], item->code[2]);
		*success = false;
	}
	return;
}

bool ProcessStat(unsigned int stat, BitReader& reader, ItemProperty& itemProp) {
	if (stat > STAT_MAX) {
		return false;
	}

	StatProperties* bits = GetStatProperties(stat);
	unsigned int saveBits = bits->saveBits;
	unsigned int saveParamBits = bits->saveParamBits;
	unsigned int saveAdd = bits->saveAdd;
	itemProp.stat = stat;

	if (saveParamBits > 0) {
		switch (stat) {
		case STAT_CLASSSKILLS:
		{
			itemProp.characterClass = reader.read(saveParamBits);
			itemProp.value = reader.read(saveBits);
			return true;
		}
		case STAT_NONCLASSSKILL:
		case STAT_SINGLESKILL:
		{
			itemProp.skill = reader.read(saveParamBits);
			itemProp.value = reader.read(saveBits);
			return true;
		}
		case STAT_ELEMENTALSKILLS:
		{
			ulong element = reader.read(saveParamBits);
			itemProp.value = reader.read(saveBits);
			return true;
		}
		case STAT_AURA:
		{
			itemProp.skill = reader.read(saveParamBits);
			itemProp.value = reader.read(saveBits);
			return true;
		}
		case STAT_REANIMATE:
		{
			itemProp.monster = reader.read(saveParamBits);
			itemProp.value = reader.read(saveBits);
			return true;
		}
		case STAT_SKILLTAB:
		{
			itemProp.tab = reader.read(3);
			itemProp.characterClass = reader.read(3);
			ulong unknown = reader.read(10);
			itemProp.value = reader.read(saveBits);
			return true;
		}
		case STAT_SKILLONDEATH:
		case STAT_SKILLONHIT:
		case STAT_SKILLONKILL:
		case STAT_SKILLONLEVELUP:
		case STAT_SKILLONSTRIKING:
		case STAT_SKILLWHENSTRUCK:
		{
			itemProp.level = reader.read(6);
			itemProp.skill = reader.read(10);
			itemProp.skillChance = reader.read(saveBits);
			return true;
		}
		case STAT_CHARGED:
		{
			itemProp.level = reader.read(6);
			itemProp.skill = reader.read(10);
			itemProp.charges = reader.read(8);
			itemProp.maximumCharges = reader.read(8);
			return true;
		}
		case STAT_STATE:
		case STAT_ATTCKRTNGVSMONSTERTYPE:
		case STAT_DAMAGETOMONSTERTYPE:
		{
			// For some reason heroin_glands doesn't read these, even though
			// they have saveParamBits; maybe they don't occur in practice?
			itemProp.value = reader.read(saveBits) - saveAdd;
			return true;
		}
		default:
			reader.read(saveParamBits);
			reader.read(saveBits);
			return true;
		}
	}

	if (bits->op >= 2 && bits->op <= 5) {
		itemProp.perLevel = reader.read(saveBits);
		return true;
	}

	switch (stat) {
	case STAT_ENHANCEDMAXIMUMDAMAGE:
	case STAT_ENHANCEDMINIMUMDAMAGE:
	{
		itemProp.minimum = reader.read(saveBits);
		itemProp.maximum = reader.read(saveBits);
		return true;
	}
	case STAT_MINIMUMFIREDAMAGE:
	{
		itemProp.minimum = reader.read(saveBits);
		itemProp.maximum = reader.read(GetStatProperties(STAT_MAXIMUMFIREDAMAGE)->saveBits);
		return true;
	}
	case STAT_MINIMUMLIGHTNINGDAMAGE:
	{
		itemProp.minimum = reader.read(saveBits);
		itemProp.maximum = reader.read(GetStatProperties(STAT_MAXIMUMLIGHTNINGDAMAGE)->saveBits);
		return true;
	}
	case STAT_MINIMUMMAGICALDAMAGE:
	{
		itemProp.minimum = reader.read(saveBits);
		itemProp.maximum = reader.read(GetStatProperties(STAT_MAXIMUMMAGICALDAMAGE)->saveBits);
		return true;
	}
	case STAT_MINIMUMCOLDDAMAGE:
	{
		itemProp.minimum = reader.read(saveBits);
		itemProp.maximum = reader.read(GetStatProperties(STAT_MAXIMUMCOLDDAMAGE)->saveBits);
		itemProp.length = reader.read(GetStatProperties(STAT_COLDDAMAGELENGTH)->saveBits);
		return true;
	}
	case STAT_MINIMUMPOISONDAMAGE:
	{
		itemProp.minimum = reader.read(saveBits);
		itemProp.maximum = reader.read(GetStatProperties(STAT_MAXIMUMPOISONDAMAGE)->saveBits);
		itemProp.length = reader.read(GetStatProperties(STAT_POISONDAMAGELENGTH)->saveBits);
		return true;
	}
	case STAT_REPAIRSDURABILITY:
	case STAT_REPLENISHESQUANTITY:
	{
		itemProp.value = reader.read(saveBits);
		return true;
	}
	default:
	{
		itemProp.value = reader.read(saveBits) - saveAdd;
		return true;
	}
	}
}



