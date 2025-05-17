#pragma once
#include "../../Drawing.h"
#include "../Module.h"
#include "../../Config.h"
#include "../../Common.h"
#include "../../Constants.h"
#include <list>

class GameSettings : public Module {
public:
	static unsigned int KeyHookOffset;
	static map<std::string, Toggle> Toggles;
	
	// 将这些变量改为public并改为unsigned int类型
	static unsigned int showPlayer;
	static unsigned int resyncKey;
	static unsigned int advStatMenuKey;

	GameSettings() : Module("GameSettings") {};
	~GameSettings() {};

	void Init();
	void OnLoad();
	void OnLoop();
	void OnUnload();
	void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
	void LoadConfig();
	void LoadGeneralTab();
	void LoadInteractionTab();

	static void ResetGamePatches();
};
