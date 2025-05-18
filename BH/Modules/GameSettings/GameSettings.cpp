#include "GameSettings.h"
#include "../../Drawing.h"
#include "../Module.h"
#include "../../Config.h"
#include "../../Common.h"
#include "../../BH.h"
#include "../Item/Item.h"
#include "../ScreenInfo/ScreenInfo.h"
#include "../MapNotify/MapNotify.h"
#include "../../D2Ptrs.h"
#include "../../D2Intercepts.h"
#include "../../UIString_Base.h"

// This module was inspired by the RedVex plugin "Item Mover", written by kaiks.
// Thanks to kaiks for sharing his code.

map<std::string, Toggle> GameSettings::Toggles;
unsigned int GameSettings::KeyHookOffset = 300;

// 定义静态成员变量
unsigned int GameSettings::showPlayer;
unsigned int GameSettings::resyncKey;
unsigned int GameSettings::advStatMenuKey;

// 前向声明
VOID __stdcall GS_Shake_Interception(LPDWORD lpX, LPDWORD lpY);

// 添加两个功能使用的补丁 - 使用不同名称避免链接错误
Patch* gs_shakePatch = new Patch(Call, D2CLIENT, { 0x442A2, 0x452F2 }, (int)GS_Shake_Interception, 5);
Patch* gs_cpuPatch = new Patch(NOP, D2CLIENT, { 0x3CB7C, 0x2770C }, 0, 9);

void GameSettings::Init() {
}

// 重置补丁函数
void GameSettings::ResetGamePatches() {
    // 地震效果补丁
    if (Toggles["Remove Shake"].state)
        gs_shakePatch->Install();
    else
        gs_shakePatch->Remove();

    // CPU补丁
    if (Toggles["Apply CPU Patch"].state)
        gs_cpuPatch->Install();
    else
        gs_cpuPatch->Remove();
}

void GameSettings::LoadConfig() {
	BH::config->ReadToggle("Quick Cast", "None", false, GameSettings::Toggles["Quick Cast"]);
	BH::config->ReadToggle("Skill Bar", "None", false, GameSettings::Toggles["Skill Bar"]);
	BH::config->ReadToggle("Skill Bar Disable", "None", false, GameSettings::Toggles["Skill Bar Disable"]);
	BH::config->ReadToggle("Buff Timers", "None", false, GameSettings::Toggles["Buff Timers"]);

	BH::config->ReadToggle("Developer Aura", "None", true, GameSettings::Toggles["Developer Aura"]);
	BH::config->ReadToggle("99 Aura", "None", true, GameSettings::Toggles["99 Aura"]);
	BH::config->ReadToggle("Rathma Aura", "None", true, GameSettings::Toggles["Rathma Aura"]);
	BH::config->ReadToggle("Dclone Aura", "None", true, GameSettings::Toggles["Dclone Aura"]);
	
	// 为地图功能添加Toggle
	BH::config->ReadToggle("Remove Shake", "None", false, GameSettings::Toggles["Remove Shake"]);
	BH::config->ReadToggle("Apply CPU Patch", "None", true, GameSettings::Toggles["Apply CPU Patch"]);

	BH::config->ReadKey("Show Players Gear", "VK_0", showPlayer);
	BH::config->ReadKey("Resync Hotkey", "VK_9", resyncKey);
	BH::config->ReadKey("Character Stats", "VK_8", advStatMenuKey);
}

void GameSettings::LoadGeneralTab() {
	Drawing::Texthook* colored_text;

	Drawing::UITab* generalTab = new Drawing::UITab(UI_GENERAL_TAB, BH::settingsUI);

	unsigned int x_tab = 15;
	unsigned int x = 8;
	unsigned int y = 7;

	// General
	new Drawing::Texthook(generalTab, x, (y), UI_GENERAL_TAB);

	// y += 15;
	// colored_text = new Drawing::Texthook(generalTab, x, (y), "显示其它玩家鸡儿");
	// colored_text->SetColor(Gold);
	// new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &showPlayer, "");

	// y += 15;
	// colored_text = new Drawing::Texthook(generalTab, x, (y), "同步(卡角色的时候按一下)");
	// colored_text->SetColor(Gold);
	// new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &resyncKey, "");

	y += 15;
	colored_text = new Drawing::Texthook(generalTab, x, (y), UI_GS_ADV_STATS);
	colored_text->SetColor(Gold);
	new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &advStatMenuKey, "");

	y += 15;
	new Drawing::Checkhook(generalTab, x, y, &ScreenInfo::Toggles["Experience Meter"].state, UI_GS_EXP_METER);
	new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &ScreenInfo::Toggles["Experience Meter"].toggle, "");

	y += 15;
	new Drawing::Checkhook(generalTab, x, y, &GameSettings::Toggles["Remove Shake"].state, UI_GS_REMOVE_SHAKE);
	new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &GameSettings::Toggles["Remove Shake"].toggle, "");

	y += 15;
	new Drawing::Checkhook(generalTab, x, y, &GameSettings::Toggles["Apply CPU Patch"].state, UI_GS_CPU_PATCH);
	new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &GameSettings::Toggles["Apply CPU Patch"].toggle, "");

	// // Quick Cast
	// y += 20;
	// new Drawing::Texthook(generalTab, x, (y), "快速施法");

	// y += 15;
	// new Drawing::Checkhook(generalTab, x, y, &GameSettings::Toggles["Quick Cast"].state, "快速施法");
	// new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &GameSettings::Toggles["Quick Cast"].toggle, "");

	// y += 15;
	// new Drawing::Checkhook(generalTab, x, y, &GameSettings::Toggles["Skill Bar"].state, "技能栏");
	// new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &GameSettings::Toggles["Skill Bar"].toggle, "");

	// y += 15;
	// new Drawing::Checkhook(generalTab, x + x_tab, y, &GameSettings::Toggles["Skill Bar Disable"].state, "只启用快速施法，不显示技能栏");
	// new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &GameSettings::Toggles["Skill Bar Disable"].toggle, "");

	// y += 15;
	// new Drawing::Checkhook(generalTab, x, y, &GameSettings::Toggles["Buff Timers"].state, "总是显示状态计时器");
	// new Drawing::Keyhook(generalTab, GameSettings::KeyHookOffset, y + 2, &GameSettings::Toggles["Buff Timers"].toggle, "");

}

void GameSettings::LoadInteractionTab() {
	Drawing::Texthook* colored_text;
	Drawing::UITab* tab = new Drawing::UITab(UI_QUICKHELP_TAB, BH::settingsUI);

	unsigned int x = 8;
	unsigned int y = 7;
	int offset = 150;
	int indent = 5;

	// QoL
	colored_text = new Drawing::Texthook(tab, x, y,
		UI_QH_MOUSE_HOVER);

	y += 15;
	colored_text = new Drawing::Texthook(tab, x + indent, y,
		UI_QH_SHIFT_LEFT);
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
		UI_QH_SHIFT_LEFT_DESC);
	colored_text->SetColor(Gold);

	y += 15;
	colored_text = new Drawing::Texthook(tab, x + indent, y,
		UI_QH_SHIFT_RIGHT);
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
		UI_QH_SHIFT_RIGHT_DESC);
	colored_text->SetColor(Gold);

	y += 15;
	colored_text = new Drawing::Texthook(tab, x + indent, y,
		UI_QH_CTRL_RIGHT);
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
		UI_QH_CTRL_RIGHT_DESC);
	colored_text->SetColor(Gold);

	y += 15;
	colored_text = new Drawing::Texthook(tab, x + indent, y,
		UI_QH_CTRL_SHIFT_RIGHT);
	colored_text->SetColor(Gold);
	colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
		UI_QH_CTRL_SHIFT_RIGHT_DESC);
	colored_text->SetColor(Gold);

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"Ctrl+Shift+左键");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"切换 堆叠/非堆叠");
	// colored_text->SetColor(Gold);

	// y += 20;
	// colored_text = new Drawing::Texthook(tab, x, y,
	// 	"鼠标拿起物品时");

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"Ctrl+左键");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"放置 1 个堆叠的物品");
	// colored_text->SetColor(Gold);

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"Ctrl+Shift+左键");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"放置 1 个非堆叠的物品");
	// colored_text->SetColor(Gold);

	// y += 20;
	// colored_text = new Drawing::Texthook(tab, x, y,
	// 	"在快捷栏、背包上按");

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"Ctrl+右键");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"使用药水、卷轴等");
	// colored_text->SetColor(Gold);

	// y += 20;
	// colored_text = new Drawing::Texthook(tab, x, y,
	// 	"整理背包");

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"ALT+r");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"所有包包一起整理");
	// colored_text->SetColor(Gold);

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"ALT+number");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"对应包包");
	// colored_text->SetColor(Gold);

	// y += 20;
	// colored_text = new Drawing::Texthook(tab, x, y,
	// 	"其他");

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"ALT+s");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"保存小号插件仓库");
	// colored_text->SetColor(Gold);

	// y += 15;
	// colored_text = new Drawing::Texthook(tab, x + indent, y,
	// 	"ALT+l");
	// colored_text->SetColor(Gold);
	// colored_text = new Drawing::Texthook(tab, x + indent + offset, y,
	// 	"加载小号插件仓库");
	// colored_text->SetColor(Gold);

}

void GameSettings::OnLoad() {
	LoadConfig();
	LoadGeneralTab();
	LoadInteractionTab();
	ResetGamePatches(); // 加载时应用补丁
}

void GameSettings::OnLoop() {
	ResetGamePatches(); // 持续检查和应用补丁
}

void GameSettings::OnUnload() {
	// 卸载时移除补丁
	gs_shakePatch->Remove();
	gs_cpuPatch->Remove();
}

void GameSettings::OnKey(bool up, BYTE key, LPARAM lParam, bool* block) {
	for (map<string, Toggle>::iterator it = Toggles.begin(); it != Toggles.end(); it++) {
		if (key == (*it).second.toggle) {
			*block = true;
			if (up) {
				(*it).second.state = !(*it).second.state;
				
				// 如果是我们添加的功能，重新应用补丁
				if (it->first == "Remove Shake" || it->first == "Apply CPU Patch") {
					ResetGamePatches();
				}
				
				BH::config->Write();
			}
			return;
		}
	}

	if (key == showPlayer) {
		*block = true;
		if (up)
			return;
		UnitAny* selectedUnit = D2CLIENT_GetSelectedUnit();
		if (selectedUnit && selectedUnit->dwMode != 0 && selectedUnit->dwMode != 17 && ( // Alive
			selectedUnit->dwType == 0 ||					// Player
			selectedUnit->dwTxtFileNo == 291 ||		// Iron Golem
			selectedUnit->dwTxtFileNo == 357 ||		// Valkerie
			selectedUnit->dwTxtFileNo == 418)) {	// Shadow Master
			Item::viewingUnit = selectedUnit;
			if (!D2CLIENT_GetUIState(0x01))
				D2CLIENT_SetUIVar(0x01, 0, 0);
			return;
		}
	}
}

// 移除地震效果的拦截函数
VOID __stdcall GS_Shake_Interception(LPDWORD lpX, LPDWORD lpY)
{
	*p_D2CLIENT_xShake = 0;
	*p_D2CLIENT_yShake = 0;
}
