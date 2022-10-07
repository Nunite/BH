#include "PlugYFiles.h"
#include "../../D2Helpers.h"
#include "../../Patch.h"
#include "../../D2Stubs.h"


void* stashBtnsImages = NULL;
void* sharedGoldBtnsImages = NULL;

char* modDataDirectory = "PlugY";

void loadImagesFile(void** images, const char* name)
{
	if (!images || !name) return;
	if (!*images)
	{
		char buffer[MAX_PATH];
		sprintf(buffer, "%s\\%s", modDataDirectory, name);
		LogMsg("Images file to load : %s\n", buffer);

		*images = D2ClientASM_LoadImage_STUB(buffer, 0);
		if (!*images)
		{
			sprintf(buffer, "Don't find Buttons Images File : %s.dc6", name);
			d2_assert(!*images, buffer, __FILE__, __LINE__);
		}
	}
}

void __stdcall loadCustomImages()
{
	//if (active_SkillsPoints) loadImagesFile(&unassignSkillsBtnImages, "UnassignSkillsBtns");
	//if (active_StatsPoints) loadImagesFile(&unassignStatsBtnImages, "UnassignStatsBtns");
	/*if (active_multiPageStash || active_newInterfaces)*/ loadImagesFile(&stashBtnsImages, "StashBtns");
	/*if (active_sharedGold)*/ loadImagesFile(&sharedGoldBtnsImages, "SharedGoldBtns");
	//if (active_newInterfaces) loadImagesFile(&newStatsInterfaceImages, "EmptyPage");
	//if (active_newInterfaces) loadImagesFile(&statsBackgroundImages, "StatsBackground");
}

void __declspec(naked) caller_loadCustomImages()
{
	__asm
	{
		CALL loadCustomImages
		MOV ECX, 0x0C
		RETN
	}
}

#define FREE_IMAGE_FILE(I) if(I) {D2ClientASM_FreeImage_STUB(I);I=NULL;}

void __stdcall freeCustomImages()
{
	//FREE_IMAGE_FILE(unassignSkillsBtnImages);
	//FREE_IMAGE_FILE(unassignStatsBtnImages);
	FREE_IMAGE_FILE(stashBtnsImages);
	FREE_IMAGE_FILE(sharedGoldBtnsImages);
	//FREE_IMAGE_FILE(newStatsInterfaceImages);
	//FREE_IMAGE_FILE(statsBackgroundImages);
}

void __declspec(naked) caller_freeCustomImages()
{
	__asm
	{
		CALL freeCustomImages
		MOV ECX, 0x0C
		RETN
	}
}

void Install_PlugYImagesFiles() {

	LogMsg("Install_PlugYImagesFiles ¿ªÊ¼\n");

	//D2Client.dll offset 27EAE
	//caller_loadCustomImages
	Patch* patch1 = new Patch(Call, D2CLIENT, { 0x27EAE,0x27EAE }, (DWORD)caller_loadCustomImages, 5);
	patch1->Install();

	//D2Client.dll offset 26F8D
	//caller_freeCustomImages
	Patch* patch2 = new Patch(Call, D2CLIENT, { 0x26F8D,0x26F8D }, (DWORD)caller_freeCustomImages, 5);
	patch2->Install();
	
	LogMsg("Install_PlugYImagesFiles ½áÊø\n");
}