/**
*
* D2Structs.h
* BH: Copyright 2011 (C) McGod
* SlashDiablo Maphack: Copyright (C) SlashDiablo Community
*
*  This file is part of SlashDiablo Maphack.
*
*  SlashDiablo Maphack is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Affero General Public License as published
*  by the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Affero General Public License for more details.
*
*  You should have received a copy of the GNU Affero General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* This file incorporates work covered by the following copyright and
* permission notice:
*
*   ==========================================================
*   D2Ex2
*   https://github.com/lolet/D2Ex2
*   ==========================================================
*   Copyright (c) 2011-2014 Bartosz Jankowski
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*   ==========================================================
*
*/

#pragma once
#ifndef _D2STRUCTS_H
#define _D2STRUCTS_H

#include <windows.h>
#include "CommonStructs.h"
#include "D2DataTables.h"

#pragma warning ( push )
#pragma warning ( disable: 4201 )
#pragma optimize ( "", off )

struct UnitAny;
struct Room1;
struct Room2;
struct Level;
struct Act;
struct ActMisc;
struct RosterUnit;
struct OverheadMsg;
struct NetClient;
struct UnitAnyHM;
struct DrlgMisc;
struct DrlgLevel;
struct DrlgRoom2;
struct DrlgRoom1;
struct GameStruct;

struct InventoryInfo {
	int nLocation;
	int nMaxXCells;
	int nMaxYCells;
};

struct GameStructInfo {
	BYTE _1[0x1B];					//0x00
	char szGameName[0x18];			//0x1B
	char szGameServerIp[0x56];		//0x33
	char szAccountName[0x30];		//0x89
	char szCharName[0x18];			//0xB9
	char szRealmName[0x18];			//0xD1
	BYTE _2[0x158];					//0xE9
	char szGamePassword[0x18];		//0x241
};

struct AutomapCell {
	DWORD fSaved;					//0x00
	WORD nCellNo;					//0x04
	WORD xPixel;					//0x06
	WORD yPixel;					//0x08
	WORD wWeight;					//0x0A
	AutomapCell* pLess;				//0x0C
	AutomapCell* pMore;				//0x10
};

struct GfxCell {
	DWORD flags;					//0x00
	DWORD width;					//0x04
	DWORD height;					//0x08
	DWORD xoffs;					//0x0C
	DWORD yoffs;					//0x10
	DWORD _2;						//0x14
	DWORD lpParent;					//0x18
	DWORD length;					//0x1C
	BYTE cols;						//0x20
};

struct InteractStruct {
	DWORD dwMoveType;			//0x00
	UnitAny* lpPlayerUnit;		//0x04
	UnitAny* lpTargetUnit;		//0x08
	DWORD dwTargetX;			//0x0C
	DWORD dwTargetY;			//0x10
	DWORD _1;					//0x14
	DWORD _2;					//0x18
};

struct CellContext {
	DWORD nCellNo;					//0x00
	DWORD _1[12];					//0x04
	CellFile* pCellFile;			//0x34
};

struct AutomapLayer {
	DWORD nLayerNo;					//0x00
	DWORD fSaved;					//0x04
	AutomapCell* pFloors;			//0x08
	AutomapCell* pWalls;			//0x0C
	AutomapCell* pObjects;			//0x10
	AutomapCell* pExtras;			//0x14
	AutomapLayer* pNextLayer;		//0x18
};

struct AutomapLayer2 {
	DWORD _1[2];					//0x00
	DWORD nLayerNo;					//0x08
};

struct LevelText {
	DWORD dwLevelNo;				//0x00
	DWORD _1[60];					//0x04
	BYTE _2;						//0xF4
	char szName[40];				//0xF5
	char szEntranceText[40];		//0x11D
	char szLevelDesc[41];			//0x145
	wchar_t wName[40];				//0x16E
	wchar_t wEntranceText[40];		//0x1BE
	BYTE nObjGroup[8];				//0x196
	BYTE nObjPrb[8];				//0x19E
};

struct ControlPreferences
{
	DWORD dwType;//0x00
	// ..
};

struct Control {
	DWORD dwType;					//0x00
	CellFile* pCellFile;				//0x04
	DWORD dwDisabled;				//0x08
	DWORD dwPosX;					//0x0C
	DWORD dwPosY;					//0x10
	DWORD dwSizeX;					//0x14
	DWORD dwSizeY;					//0x18
	VOID(__fastcall* Initialize)(Control* pControl);//0x1c
	DWORD _3;						//0x20
	DWORD* _4;						//0x24
	DWORD* _5;						//0x28
	DWORD _6;						//0x2C
	DWORD* _7;						//0x30
	DWORD* _8;						//0x34
	DWORD _9;						//0x38
	Control* pNext;					//0x3C
	DWORD _10;						//0x40
	DWORD dwMaxLength;				//0x44
	union {
		ControlText* pFirstText;		//0x48
		DWORD dwScrollEntries;		//0x48
	};
	union {
		ControlText* pLastText;		//0x4C
		DWORD dwScrollPosition;		//0x4C
	};
	ControlText* pSelectedText;		//0x50
	DWORD dwSelectEnd;				//0x54
	DWORD dwSelectStart;				//0x58
	wchar_t wText[0x1A];				//0x5C
	Control* pChildControl;			//0x90
};


struct NetClient
{
	DWORD	clientID;					//+00
	BYTE	uk1[0x06];					//+04
	union {								//+0A
		WORD	flag;
		struct {
			WORD f1 : 1;
			WORD f2 : 1;
			WORD isHardCoreGame : 1;
		};
	};
	BYTE	uk2;						//+0C
	char	name[0x10];					//+0D
	BYTE	uk3[0x15F];					//+1D
	BYTE* savefile;					//+17C
	DWORD	finalSize;					//+180
	DWORD	counter;					//+184
	DWORD	currentSize;				//+188
	BYTE	uk4[0x1C];					//+18C
	GameStruct* ptGame;					//+1A8
	DWORD  unk5[0xBF];					//+1AC
	NetClient* pNext;					//+4A8
	NetClient* pListNext;				//0x4AC
	NetClient* pNextByName;				//0x4B0
	DWORD __4B4[19];						//0x4B4
	DWORD dwSentPings;						//0x500
	DWORD __504;							//0x504
	DWORD dwExpLost;						//0x508
	DWORD dwLangId;							//0x50C
	DWORD __510[2];							//0x510
/*
Kingpin: ptPclient
+16C is either act or unit type
+170 Active UniqueID for player
+174 Active ptPlayer on Client
+1a8 ptGame
+1b4 Current or next ptRoom
*/
};

//by zyl from HM
struct LevelTxt {				//size = 0x220
	WORD	wLevelNo;			//+00
	BYTE	nPal;				//+02
	BYTE	nAct;				//+03
	DWORD	_1[2];				//+04
	DWORD	dwWarpDist;			//+0C
	WORD	nMonLv[2][3];		//+10	Area Level
	DWORD	nMonDen[3];			//+1C 
	BYTE	monumin[3];			//+28
	BYTE	monumax[3];			//+2B
	BYTE	MonWndr;			//+2E
	BYTE	MonSpcWalk;			//+2F
	DWORD	_2[49];				//+30
	BYTE	_3;					//+F4
	char	szName[40];			//+F5
	char	szEntranceText[40];	//+11D
	char	szLvlDesc[40];		//+145
	BYTE	_4;					//+16D	
	wchar_t wszName[40];		//+16E
	wchar_t wszEntranceText[40];//+1BE
	BYTE	_5[2];				//+20E
	DWORD	wSoundEnv;			//+210
	DWORD	_6[2];				//+214
	WORD	wThemes;			//+21C
	WORD	_7;					//+21E
};

//这个是HM的状态数据结构
struct StatMonitor {
	wchar_t wszDesc[2][30];
	DWORD	dwStatNo;
	DWORD	dwTimer;
	DWORD   dwColor;
	BOOL	fEnable;
	BOOL    fCountDown;   //1=倒计时
};

//HM的文字输入框
struct D2EditBox {
	DWORD	dwType;				//+00
	CellFile* pCellFile;		//+04
	DWORD   dwFlag;				//+08
	DWORD	dwPosX;				//+0C   
	DWORD	dwPosY;				//+10
	DWORD	dwSizeX;			//+14
	DWORD	dwSizeY;			//+18
	void(__fastcall* fnCallBack)(D2EditBox*); // +1C
	DWORD _3[7];				//+20
	D2EditBox* pNext;			//+3C
	DWORD   _4;					//+40
	DWORD   dwOffsetY;			//+44
	union {
		DWORD   dwMaxLength;		//+48
		DWORD	dwScrollEntries;	//+48
	};
	DWORD   dwScrollPosition;	//+4C ?
	DWORD	_5;					//+50
	DWORD	dwSelectStart;		//+54
	DWORD	dwSelectEnd;		//+58
	wchar_t wszText[256];		//+5C
	DWORD	dwCursorPos;		//+25C
	DWORD	dwIsCloaked;		//+260
};

#pragma pack(push)
#pragma pack(1)

struct RoomTile {
	Room2* pRoom2;				//0x00
	RoomTile* pNext; 			//0x04
	DWORD _2[2];				//0x08
	DWORD* nNum; 				//0x10
};

struct RosterUnit {
	char szName[16];				//0x00
	DWORD dwUnitId;					//0x10
	DWORD dwPartyLife;				//0x14
	DWORD _1;						//0x18
	DWORD dwClassId;                //0x1C
	WORD wLevel;					//0x20
	WORD wPartyId;  				//0x22
	DWORD dwLevelId;                //0x24
	DWORD Xpos;						//0x28
	DWORD Ypos;						//0x2C
	DWORD dwPartyFlags;				//0x30
	BYTE* _5;						//0x34
	DWORD _6[11];					//0x38
	WORD _7;						//0x64
	char szName2[16];				//0x66
	WORD _8;						//0x76
	DWORD _9[2];					//0x78
	RosterUnit* pNext;				//0x80
};

struct QuestInfo {
	void* pBuffer;					//0x00
	DWORD _1;						//0x04
};

struct Waypoint {
	BYTE flags;						//0x00
};

struct PlayerData {
	char szName[0x10];				//0x00
	QuestInfo* pNormalQuest;		//0x10
	QuestInfo* pNightmareQuest;		//0x14
	QuestInfo* pHellQuest;			//0x18
	Waypoint* pNormalWaypoint;		//0x1c
	Waypoint* pNightmareWaypoint;	//0x20
	Waypoint* pHellWaypoint;		//0x24
};

struct saveBitField
{
	BYTE* data;
	DWORD	maxBits;
	DWORD	currentByte;
	DWORD	currentBit;
	DWORD	overflaw;
};

struct Stash
{
	DWORD id;
	union {
		DWORD flags;
		struct {
			DWORD isShared : 1;	//
			DWORD isIndex : 1;	//
			DWORD isMainIndex : 1;	//
			DWORD isReserved : 1;	// For swap items comand
		};
	};
	char* name;
	UnitAnyHM* ptListItem;
	Stash* previousStash;
	Stash* nextStash;
};

struct PYPlayerData
{
	union {
		DWORD flags;
		struct {
			DWORD selfStashIsOpened : 1;	//
			DWORD sharedStashIsOpened : 1;//
			DWORD showSharedStash : 1;	//
//			DWORD notOnRealm:1;			//set to 1 when the player is load from client or in SP
		};
	};
	DWORD	sharedGold;
	DWORD	nbSelfPages;
	DWORD	nbSharedPages;
	Stash* currentStash;
	Stash* selfStash;
	Stash* sharedStash;
};

struct PlayerDataHM
{
	char		name[0x10];				//+00	Player Name  
	QuestInfo* ptQuest[3];				//+10	Quest Pointers for each difficulty  
	Waypoint* pWaypoints[3];				//+1C
	BYTE		uk1[0xC];				//+28		//before : 0x14
	void* ptArenaUnit;			//+34	ptArena for the UnitAny  
	BYTE		uk2[0x4];				//+38		//before : 0x7
	WORD		MPSourcePortalUniqueID;	//+3C	Source Portal Unique_ID  
	BYTE		uk3[0x2];				//+3E
	WORD		MPDestPortalUniqueID;	//+40	Destination Portal Unique_ID  
	BYTE		uk4[0x06];				//+42  
	BYTE		ptObjectUnID;			//+48	Object UniqueID for TownPortals       
	BYTE		uk5[0x53];				//+49  
	NetClient* ptNetClient;			//+9C	ptClient  
	DWORD		unk6[0x33];				//+A0
	PYPlayerData ptPYPlayerData;		//+16C
};

struct PresetUnit {
	DWORD _1;						//0x00
	DWORD dwTxtFileNo;				//0x04
	DWORD dwPosX;					//0x08
	PresetUnit* pPresetNext;		//0x0C
	DWORD _3;						//0x10
	DWORD dwType;					//0x14
	DWORD dwPosY;					//0x18
};

struct Level {
	DWORD _1[4];			//0x00
	Room2* pRoom2First;		//0x10
	DWORD _2[2];			//0x14
	DWORD dwPosX;			//0x1C
	DWORD dwPosY;			//0x20
	DWORD dwSizeX;			//0x24
	DWORD dwSizeY;			//0x28
	DWORD _3[96];			//0x2C
	Level* pNextLevel;		//0x1AC
	DWORD _4;				//0x1B0
	ActMisc* pMisc;			//0x1B4
	DWORD _5[6];			//0x1BC
	DWORD dwLevelNo;		//0x1D0
};

struct Room2 {
	DWORD _1[2];			//0x00
	Room2** pRoom2Near;		//0x08
	DWORD _2[5];			//0x0C
	struct {
		DWORD dwRoomNumber; //0x00
		DWORD _1;			//0x04
		DWORD* pdwSubNumber;//0x08
	} *pType2Info;			//0x20
	Room2* pRoom2Next;		//0x24
	DWORD dwRoomFlags;		//0x28
	DWORD dwRoomsNear;		//0x2C
	Room1* pRoom1;			//0x30
	DWORD dwPosX;			//0x34
	DWORD dwPosY;			//0x38
	DWORD dwSizeX;			//0x3C
	DWORD dwSizeY;			//0x40
	DWORD _3;				//0x44
	DWORD dwPresetType;		//0x48
	RoomTile* pRoomTiles;	//0x4C
	DWORD _4[2];			//0x50
	Level* pLevel;			//0x58
	PresetUnit* pPreset;	//0x5C
};

#pragma pack(pop)

struct Room1 {
	Room1** pRoomsNear; 	//0x00
	DWORD _1[3];			//0x04
	Room2* pRoom2;			//0x10
	DWORD _2[3];			//0x14
	CollMap* Coll;			//0x20
	DWORD dwRoomsNear;		//0x24
	DWORD _3[9];			//0x28
	DWORD dwXStart;			//0x4C
	DWORD dwYStart;			//0x50
	DWORD dwXSize;			//0x54
	DWORD dwYSize;			//0x58
	DWORD _4[6];			//0x5C
	UnitAny* pUnitFirst;	//0x74
	DWORD _5;				//0x78
	Room1* pRoomNext;		//0x7C
};

struct ActMisc {
	DWORD _1[37];			//0x00
	DWORD dwStaffTombLevel; //0x94
	DWORD _2[245];			//0x98
	Act* pAct;				//0x46C
	DWORD _3[3];			//0x470
	Level* pLevelFirst;		//0x47C
	DWORD _4;				//+480 参考了HM 
	DWORD dwBossTombLvl;	//+484 参考了HM
};

struct Act {
	DWORD _1[3];			//0x00
	DWORD dwMapSeed;		//0x0C
	Room1* pRoom1;			//0x10
	DWORD dwAct;			//0x14
	DWORD _2[12];			//0x18
	ActMisc* pMisc;			//0x48
};

struct Path {
	WORD xOffset;					//0x00
	WORD xPos;						//0x02
	WORD yOffset;					//0x04
	WORD yPos;						//0x06
	DWORD _1[2];					//0x08
	WORD xTarget;					//0x10
	WORD yTarget;					//0x12
	DWORD _2[2];					//0x14
	Room1* pRoom1;					//0x1C
	Room1* pRoomUnk;				//0x20
	DWORD _3[3];					//0x24
	UnitAny* pUnit;					//0x30
	DWORD dwFlags;					//0x34
	DWORD _4;						//0x38
	DWORD dwPathType;				//0x3C
	DWORD dwPrevPathType;			//0x40
	DWORD dwUnitSize;				//0x44
	DWORD _5[4];					//0x48
	UnitAny* pTargetUnit;			//0x58
	DWORD dwTargetType;				//0x5C
	DWORD dwTargetId;				//0x60
	BYTE bDirection;				//0x64
};

struct ItemPath {
	DWORD _1[3];					//0x00
	DWORD dwPosX;					//0x0C
	DWORD dwPosY;					//0x10
	//Use Path for the rest
};

struct Stat {
	WORD wSubIndex;					//0x00
	WORD wStatIndex;				//0x02
	DWORD dwStatValue;				//0x04
};

// Credits to SVR, http://phrozenkeep.hugelaser.com/forum/viewtopic.php?f=8&t=31458&p=224066
struct StatList {
	DWORD _1;						//0x00
	UnitAny* pUnit;					//0x04
	DWORD dwUnitType;				//0x08
	DWORD dwUnitId;					//0x0C
	DWORD dwFlags;					//0x10
	DWORD _2[4];					//0x14
	Stat* pStat;					//0x24
	WORD wStatCount1;				//0x28
	WORD wnSize;					//0x2A
	StatList* pPrevLink;			//0x2C
	DWORD _3;						//0x30
	StatList* pPrev;				//0x34
	DWORD _4;						//0x38
	StatList* pNext;				//0x3C
	StatList* pSetList;				//0x40
	DWORD _5;						//0x44
	Stat* pSetStat;					//0x48
	WORD wSetStatCount;				//0x4C
};

struct InventoryStore
{
	DWORD pFirstItem;				//0x00
	DWORD pLastItem;				//0x04
	BYTE Width;						//0x08
	BYTE Height;					//0x09
	BYTE unk[2];					//0x0A
	DWORD pArray;					//0x0C UnitAny* [height][width]
};

struct Inventory {
	DWORD dwSignature;				//0x00
	BYTE* bGame1C;					//0x04
	UnitAny* pOwner;				//0x08
	UnitAny* pFirstItem;			//0x0C
	UnitAny* pLastItem;				//0x10
	InventoryStore* pStores;		//0x14 InventoryStore* [count]
	DWORD dwStoresCount;			//0x18
	DWORD dwLeftItemUid;			//0x1C
	UnitAny* pCursorItem;			//0x20
	DWORD dwOwnerId;				//0x24
	DWORD dwItemCount;				//0x28
};

struct Light {
	DWORD _1[3];					//0x00
	DWORD dwType;					//0x0C
	DWORD _2[7];					//0x10
	DWORD dwStaticValid;			//0x2C
	int* pnStaticMap;				//0x30
};

struct SkillInfo {
	WORD wSkillId;					//0x00
};

struct Skill {
	SkillInfo* pSkillInfo;			//0x00
	Skill* pNextSkill;				//0x04
	DWORD _1[8];					//0x08
	DWORD dwSkillLevel;				//0x28
	DWORD _2[2];					//0x2C
	DWORD dwFlags;					//0x30
};

struct Info {
	BYTE* pGame1C;					//0x00
	Skill* pFirstSkill;				//0x04
	Skill* pLeftSkill;				//0x08
	Skill* pRightSkill;				//0x0C
};

struct ItemData {
	DWORD dwQuality;				//0x00
	DWORD _1[2];					//0x04
	DWORD dwItemFlags;				//0x0C 1 = Owned by player, 0xFFFFFFFF = Not owned
	DWORD _2[2];					//0x10
	DWORD dwFlags;					//0x18
	DWORD _3[3];					//0x1C
	DWORD dwFileIndex;				//0x28
	DWORD dwItemLevel;				//0x2C
	WORD wVersion;					//0x30
	WORD wRarePrefix;				//0x32
	WORD wRareSuffix;				//0x34
	WORD wAutoPrefix;				//0x36
	WORD wPrefix[3];				//0x38
	WORD wSuffix[3];				//0x3E
	BYTE BodyLocation;				//0x44
	BYTE ItemLocation;				//0x45 Non-body/belt location (Body/Belt == 0xFF)
	BYTE EarLevel;					//0x46
	BYTE VarGfx;					//0x47
	CHAR personalizedName[16];		//0x4A
	WORD _10;						//0x5A
	Inventory* pOwnerInventory;		//0x5C
	UnitAny* pPrevInvItem;			//0x60
	UnitAny* pNextInvItem;			//0x64
	BYTE _11;						//0x68
	BYTE NodePage;					//0x69 Actual location, this is the most reliable by far
	WORD _12;						//0x6A
	DWORD _13[6];					//0x6C
	UnitAny* pOwner;				//0x84
};

struct ItemText {
	wchar_t szName2[0x40];			//0x00
	union {
		DWORD dwCode;
		char szCode[4];
	};								//0x40
	BYTE _uncharted1[0x54];			//0x44
	DWORD speed;					//0x98
	BYTE _uncharted2[0x18];			//0x9C
	WORD nLocaleTxtNo;				//0xB4
	BYTE _uncharted3[0x0E];			//0xB6
	WORD rangeadder;				//0xC4
	BYTE _uncharted4[0x09];			//0xC6
	BYTE xSize;						//0xCF
	BYTE ySize;						//0xD0
	BYTE _uncharted5[0x0d];			//0xD1
	BYTE nType;						//0xDE
	BYTE _uncharted6[0x0d];			//0xDF
	BYTE fQuest;					//0xEC
	BYTE _uncharted7[0x12];			//0xED
	BYTE reqlvl;					//0xFF 
	BYTE magiclvl;					//0x100
};

struct MonsterText {
	BYTE _1[0x6];					//0x00
	WORD nLocaleTxtNo;				//0x06
	WORD flag;						//0x08
	WORD _1a;						//0x0A
	union {
		DWORD flag1;				//0x0C
		struct {
			BYTE flag1a;			//0x0C
			BYTE flag1b;			//0x0D
			BYTE flag1c[2];			//0x0E
		};
	};
	BYTE _2[0x22];					//0x10
	WORD velocity;					//0x32
	BYTE _2a[0x52];					//0x34
	WORD tcs[3][4];					//0x86
	BYTE _2b[0x52];					//0x9E
	wchar_t szDescriptor[0x3c];		//0xF0
	BYTE _3[0x1a0];					//0x12C
};

//by zyl from HM
struct MonsterTxt {			//size = 0x1A8
	WORD	hcIdx;			//+00
	WORD	hcIdx2;			//+02
	BYTE	_1[2];			//+04
	WORD	wLocaleTxtNo;	//+06
	WORD	wFlag;			//+08
	WORD	_2;				//+0A
	union {
		BYTE	nflag1;
		struct {
			BYTE _2a : 6;
			BYTE fBoss : 1;
			BYTE fPrimeevil : 1;
		};
	};						//+0C
	union {
		BYTE	nflag2;
		struct {
			BYTE fNpc : 1;
		};
	};						//+0D

	BYTE	flag3;			//+0E
	union {
		BYTE	nflag4;
		struct {
			BYTE fInventory : 1;
		};
	};						//+0F
	char   szCode[4];		//+10
	char	_3[30];			//+14
	WORD	velocity;		//+32
	WORD	run;			//+34
	BYTE	_4[24];			//+36
	BYTE	nTreat;			//+4E
	BYTE	Aidel[3];		//+4F
	BYTE	Aidst[3];		//+52
	BYTE	_5;				//+55
	WORD	Aip1[3];		//+56
	WORD	Aip2[3];		//+5C
	WORD	Aip3[3];		//+62
	WORD	Aip4[3];		//+68
	char	_6[24];			//+6E
	WORD	Tcs[3][4];		//+86
	BYTE	nTcQuestId;		//+9E
	BYTE	nTcQuestCp;		//+9F
	BYTE	Drain[3];		//+A0
	BYTE	ToBlock[3];		//+A3
	BYTE	nCrit;			//+A6
	BYTE	_7[3];			//+A7
	WORD	Level[3];		//+AA
	WORD	MinHp[3];		//+B0
	WORD	MaxHp[3];		//+B6
	WORD	Ac[3];			//+BC
	WORD	A1TH[3];		//+C2
	WORD	A2TH[3];		//+C8
	WORD	S1TH[3];		//+CE
	WORD	Exp[3];			//+D4
	WORD	A1MinD[3];		//+DA
	WORD	A1MaxD[3];		//+E0
	WORD	A2MinD[3];		//+E6
	WORD	A2MaxD[3];		//+EC
	WORD	S1MinD[3];		//+F2
	WORD	S1MaxD[3];		//+F8
	WORD	_8[3];			//+FE
	char	szDesc[48];		//+104
	char	_9[116];		//+134

};

struct MonsterDataHM {
	MonsterTxt* pMonsterTxt;	//+00
	BYTE	nComponents[16];	//+04	Order: HD, TR, LG, RA, LA, RH, LH, SH, S1, S2, S3, S4, S5, S6, S7, S8
	WORD	wNameSeed;			//+14;
	union {
		BYTE	bTypeFlags;	//+16
		struct {
			BYTE fOther : 1;		//set for some champs, uniques
			BYTE fUnique : 1;		//super unique
			BYTE fChamp : 1;
			BYTE fBoss : 1;		//unique monster ,usually boss
			BYTE fMinion : 1;
			BYTE fPoss : 1;		//possessed
			BYTE fGhost : 1;		//ghostly
			BYTE fMulti : 1;		//multishotfiring
		}; //+16
	};//+16
	BYTE	nLastMode;			//+17
	DWORD	dwDuriel;			//+18
	BYTE	anEnchants[9];		//+1C
	BYTE	_1;					//+25
	WORD	wUniqueNo;			//+26	hcIdx from superuniques.txt for superuniques 
	void* pAiGeneral;		//+28
	wchar_t* wszMonName;		//+2C	server side is pAiParams
	BYTE	_2[16];				//+30
	DWORD	dwNecroPet;			//+40
	BYTE	_3[16];				//+44
	DWORD	dwAiState;			//+54	this is used to tell monsters what special state has been set, this tells them they just got attacked etc
	DWORD	dwLevelNo;			//+58	the Id from levels.txt of the level they got spawned in 
	BYTE	nSummonerFlags;		//+5C	byte used only by the summoner
};

struct MonsterData {
	MonStatsTxt* pMonStatsTxt;				//0x00
	BYTE Components[16];					//0x04
	WORD NameSeed;							//0x14
	struct
	{
		BYTE fUnk : 1;
		BYTE fSuperUniq : 1;
		BYTE fNormal : 1;
		BYTE fChamp : 1;
		BYTE fBoss : 1;
		BYTE fMinion : 1;
	};				//0x16
	BYTE dwLastMode;				//0x17
	DWORD dwDuriel;					//0x18
	BYTE anEnchants[9];				//0x1C
	WORD wUniqueNo;					//0x26
	DWORD _5;						//0x28
	struct {
		wchar_t wName[28];
	};								//0x2C
};

struct ObjectData {
	ObjectTxt* pTxt;				//0x00
	union {
		BYTE Type;					//0x04 (0x0F would be a Exp Shrine)
		struct {
			BYTE _1 : 7;
			BYTE ChestLocked : 1;
		};
	};
	DWORD _2[8];					//0x08
	char szOwner[0x10];				//0x28
};

struct ObjectPath {
	Room1* pRoom1;					//0x00
	DWORD _1[2];					//0x04
	DWORD dwPosX;					//0x0C
	DWORD dwPosY;					//0x10
	//Leaving rest undefined, use Path
};

//by zyl From HM
struct SkillHM {
	struct {
		WORD	wSkillId;
	} *pSkillInfo;			//+00
	SkillHM* pNextSkill;	//+04
	DWORD	dwMode;			//+08
	DWORD	dwFlag0;		//+0C
	DWORD	_1[2];			//+10
	DWORD	dwTagets;		//+18
	DWORD	dwTargetType;	//+1C unit type
	DWORD	dwTargetId;		//+20 unit id
	DWORD	_2;				//+24
	DWORD	dwSkillLevel;	//+28
	DWORD	dwLevelBonus;	//+2C
	DWORD	dwQuality;		//+30  质量 ==> 回城书之类的数量
	DWORD	dwFlags;		//+34
};
//by zyl from HM
struct SkillInfoHM {
	DWORD* pGame1C;			//+00
	SkillHM* pFirstSkill;		//+04
	SkillHM* pLeftSkill;		//+08
	SkillHM* pRightSkill;		//+0C
	SkillHM* pCurrentSkill;	//+10
};

struct DrlgAct {
	DWORD  _1[4];			//+00
	DrlgRoom1* pRoom1;		//+10
	DWORD	dwActNo;		//+14
	DWORD  _2[12];			//+18
	DrlgMisc* pDrlgMisc;	//+48
};

struct DrlgMisc {
	DWORD	_1[37];				//+00
	DWORD	dwStaffTombLvl;		//+94
	DWORD	_2[248];			//+98
	DWORD* pMemPool;			//+478
	DrlgLevel* pLevelFirst;		//+47C
	DWORD	_3;					//+480
	DWORD	dwBossTombLvl;		//+484
};

struct DrlgLevel {
	DWORD	_1[4];			//+00
	DrlgRoom2* pRoom2First; //+10
	DWORD	_2[102];		//+14
	DrlgLevel* pNext;		//+1AC
	DWORD	_3;				//+1B0
	DrlgMisc* pDrlgMisc;	//+1B4 
	DWORD	_4[2];			//+1B8	
	DWORD	dwLvlType;		//+1C0
	DWORD	_5[3];			//+1C4			
	DWORD	dwLevelNo;		//+1D0
};

struct DrlgRoom2 {
	DWORD   _1[2];			 //+00
	DrlgRoom2** paRoomsNear; //+08
	DWORD   _2[5];			 //+0C
	struct {
		DWORD dwDef;
	}	*pLvlPreset;		 //+20
	DrlgRoom2* pNext;		 //+24
	DWORD   dwRoomTiles;	 //+28
	DWORD	dwRoomsNear;	 //+2C
	DrlgRoom1* pRoom1;	     //+30
	DWORD	dwPosX;			 //+34
	DWORD	dwPosY;			 //+38
	DWORD	dwSizeX;		 //+3C
	DWORD	dwSizeY;		 //+40
	DWORD   _4;				 //+44
	DWORD	dwPresetType;	 //+48
	RoomTile* pRoomTiles;	 //+4C
	DWORD   _5[2];			 //+50
	DrlgLevel* pDrlgLevel;	 //+58
	PresetUnit* pPresetUnits;//+5C	
};

struct ActHM
{
	int ActNum;			                   // 0x00
	DrlgRoom1* ptFirstRoom;		                   // 0x04
	DrlgMisc* ptMisc;			                   // 0x08
	DWORD _1[2];			                   // 0x0C
	BYTE* _2;			                   // 0x14
	//__fastcall int(*pfnRoomCallBack)(DWORD, DWORD);    // 0x18 act callback.
	DWORD pfnActCallBack;                              // 0x18
	DWORD _4[4];                                       // 0x1C
};

struct DrlgRoom1 {
	DrlgRoom1** paRoomsNear; //+00
	DWORD   _1[3];			 //+04				
	DrlgRoom2* pRoom2;		 //+10
	DWORD	_2[4];			 //+14
	DWORD	dwRoomsNear;	 //+24
	UnitAnyHM* _3[19];			 //+28
	UnitAnyHM* pUnitFirst;
	ActHM* ptAct;			 //+78
	DrlgRoom1* pNext;		 //+7C
};

struct ActMap//ptGame+BC size=0x60
{
	DWORD	isNotManaged;
	DWORD	uk4;
	DWORD	uk8;//size = 0x488
	DrlgRoom1* ptFirstRoom;
};

enum D2C_ChatMessageTypes
{
	CHAT_NONE,
	CHAT_PLAYERMESSAGE,
	CHAT_RECEIVEDWHISPER,
	CHAT_UNUSED,
	CHAT_MESSAGE,
	CHAT_EMOTE,
	CHAT_SENTWHISPER,
	CHAT_CLUESCROLL,
};

struct D2GSPacketSrv26		//variable size
{
	BYTE nHeader;			//0x00
	BYTE nMessageType;		//0x01
	BYTE Unk1;				//Unknown 0x02
	BYTE Unk2;				//0x09
	DWORD Unk2_0;			//0x04 0
	BYTE nMessageColor;		//0x08
	BYTE nNameColor;		//0x09
	char szStrings[502];	//0x0A
	//[WORD Unknown(0x02)]  00 00 00 00[BYTE 0x05 = Normal Chat || 0x01 = Whisper][Char Name] 00[Message] 00
};

//ptGame : 04E4007C
struct GameStruct
{                              			//Offset from Code.
	BYTE	uk1[0x18];					//+00
	DWORD	_ptLock;					//+18 Unknown  
	DWORD	memoryPool;					//+1C Memory Pool (??)  
	BYTE	uk2[0x4D];					//+20
	BYTE	difficultyLevel;			//+6D (Difficulty 0,1 or 2)
	WORD	unknown1;					//+6E Cube puts 4 here
	DWORD	isLODGame;					//+70 (D2=0 LOD =1) (DWORD ?)
	BYTE	uk3[0x04];					//+71
	WORD	unknown2;					//+78
	BYTE	uk4[0x0E];					//+7A
	NetClient* ptClientLastJoined;				//+88
	DWORD	nClient;						//+8C
	DWORD	uk90[11];					//+90
	ActMap* mapAct[5];					//+BC
	BYTE	ukD0[0x1024];				//+D0
	DWORD* game10F4;					//+10F4
	NetClient** ptClientList;			//+10F8
	BYTE	uk6[0x24];					//+10FC
	UnitAnyHM* units[0xA00];				//+1120
	UnitAnyHM* roomtitles[0x200];			//+1B20
};

struct UnitInventory
{                               		//Offset from Code.		Size: 30 ?40
	DWORD	tag;						//+00	= 0x01020304
	BYTE	uk1[0x04];					//+04	=? 0
	UnitAnyHM* ptChar;						//+08
	UnitAnyHM* ptItem;						//+0C
	UnitAnyHM* pLastItem;			//+10 
	UnitInventory* pInvInfo;	//+14 list of pointers to equipped gear 
	DWORD	dwInvInfoCount;		//+18 count for above 
	DWORD	dwWeaponId;			//+1C
	UnitAnyHM* inventory1;					//+20
	BYTE	uk3[0x04];					//+24
	DWORD	currentUsedSocket;			//+28 //Kingpin : a variable to know how many sockets that have added to item
	DWORD	Inventory2C;				//+2C //one value
	DWORD	Inventory30;				//+30
	void* ptCorpse;					//+34
	BYTE	uk5[0x04];					//+38
	DWORD	nextCorpseId;				//+3C //max = 15
	BYTE	uk6[0x04];					//+40
};

struct StatEx
{
	WORD	index;
	WORD	id;
	int		value;
};

struct StatsList				//sizeof(StatsList)=0x64
{
	DWORD	nMemoryPool;				//+00
	UnitAnyHM* ptUnit;					//+04
	DWORD	nUnitType;				//+08
	DWORD	nAllocUnitID;				//+0C
	union
	{
		DWORD	flags;				//+10
		struct
		{
			DWORD fuk1 : 13;			//0x00001FFF
			DWORD isDisabled : 1;		//0x00002000
			DWORD fuk2 : 17;			//0x7FFFC000
			DWORD dontUseBaseValue : 1;//0x80000000
		};
	};
	DWORD	id;						//+14
	DWORD	uk18;					//+18
	BYTE	uk2[0x08];				//+1C
	StatEx* ptBaseStatsTable;		//+24
	WORD	nbBaseStats;			//+28
	WORD	sizeOfBaseStatsTable;	//+2A ??
	StatsList* ptStats;				//+2C
	StatsList* pNextList;		// 0x30
	StatsList* pPrev;			//+34
	BYTE	uk4[0x04];				//+38
	StatsList* ptAffixStats;			//+3C
	StatsList* ptNextStats2;			//+40
	union
	{
		UnitAnyHM* ptChar;				//+44
		UnitAnyHM* ptItem;
	};
	StatEx* ptStatsTable;			//+48
	WORD	nbStats;				//+4C
	WORD	sizeOfStatsTable;		//+4E ??
	BYTE	uk5[0x8];				//+50
	BYTE* unknow0;				//+58 (sizeof(*unknow0)=0x30 (calculated)
	DWORD	unknow1;				//+5C (=0)
	DWORD	unknow2;				//+60 (=0)
};

struct StaticPath {
	DWORD* pRoom1;		//+1C
	DWORD	dwMapPosX;		//+04
	DWORD	dwMapPosY;		//+08
	DWORD	dwPosX;			//+0C
	DWORD	dwPosY;			//+10
};

struct DynamicPath {
	WORD	wOffsetX;		//+00
	WORD	wPosX;			//+02
	WORD	wOffsetY;		//+04
	WORD	wPosY;			//+06
	DWORD	dwMapPosX;		//+08
	DWORD	dwMapPosY;		//+0C
	WORD	wTargetX;		//+10
	WORD	wTargetY;		//+12
	DWORD	_2[2];			//+14
	DWORD* pRoom1;		//+1C
	DWORD* pRoomUnk;	//+20
	DWORD	_3[3];			//+24
	UnitAnyHM* pUnit;			//+30
	DWORD	dwFlags;		//+34
	DWORD	_4;				//+38
	DWORD	dwPathType;		//+3C
	DWORD	dwPrevPathType;	//+40
	DWORD	dwUnitSize;		//+44
	DWORD	_5[4];			//+48
	UnitAnyHM* pTargetUnit;	//+58
	DWORD	dwTargetType;	//+5C
	DWORD	dwTargetId;		//+60
	BYTE	nDirection;		//+64
};


struct CBPlayerData;
struct CBItemData;

struct ItemDataHM//size=0x74
{										//Offset from Code.
	DWORD	quality;					//+00
	DWORD	seedLow;					//+04
	DWORD	seedHi;						//+08
	DWORD	playerID;					//+0C #10734 / #10735 (PCInventory->ptPlayer->0C)
	DWORD	seedStarting;				//+10
	DWORD	flags1;						//+14
	union {
		DWORD	flags2;					//+18
		struct {
			DWORD	fuk1 : 1;				//0x00000001
			DWORD	isIndentified : 1;	//0x00000002
			DWORD	fuk2 : 2;				//0x0000000C
			DWORD	isUnindentified : 1;	//0x00000010
			DWORD	fuk3 : 3;				//0x000000E0
			DWORD	isBroken : 1;			//0x00000100
			DWORD	fuk4 : 2;				//0x00000600
			DWORD	isSocketed : 1;		//0x00000800
			DWORD	fuk5 : 10;			//0x003FF000
			DWORD	isEtheral : 1;		//0x00400000
			DWORD	fuk6 : 3;				//0x03800000
			DWORD	isRuneword : 1;		//0x04000000
			DWORD	fuk7 : 1;				//0x08000000
			DWORD	isPersonalized : 1;	//0x10000000
			DWORD	fuk8 : 3;				//0xE0000000
		};
	};
	/*
	ITEMFLAG_NEWITEM               = 0x00000001,
	ITEMFLAG_TAGETING               = 0x00000004,
	ITEMFLAG_UNIDENTIFIED               = 0x00000010,
	ITEMFLAG_QUANTITY               = 0x00000020,
	ITEMFLAG_Durability               = 0x00000100,
	ITEMFLAG_UNKNOWN2               = 0x00000400,
	ITEMFLAG_SOCKETED               = 0x00000800,
	ITEMFLAG_NON_SELLABLE               = 0x00001000,
	ITEMFLAG_NEWITEM2               = 0x00002000,
	ITEMFLAG_UNKNOWN3               = 0x00004000,
	ITEMFLAG_CHECKSECPRICE               = 0x00010000,
	ITEMFLAG_CHECKGAMBLEPRICE          = 0x00020000,
	ITEMFLAG_UNKNOWN4               = 0x00080000,
	ITEMFLAG_INDESTRUCTIBLE(ETHERAL) = 0x00400000,
	ITEMFLAG_UNKNOWN5               = 0x00800000,
	ITEMFLAG_FROMPLAYER               = 0x01000000,
	ITEMFLAG_RUNEWORD               = 0x04000000
	*/
	DWORD	guid1;						//+1C Global Unique ID 1  
	DWORD	guid2;						//+20 Global Unique ID 2  
	DWORD	guid3;						//+24 Global Unique ID 3  
	DWORD	uniqueID;					//+28
	BYTE	ilvl;						//+2C
	BYTE	uk1[0x03];					//+2D
	WORD	version;					//+30
	WORD	rarePrefix;					//+32
	WORD	rareSuffix;					//+34
	WORD	autoPref;					//+36
	WORD	prefix[3];					//+38
	WORD	suffix[3];					//+3E
	BYTE	equipLoc;					//+44
	/*	emplacement si 閝uip?
	*	00 = noequip/inBelt
	*   01 = head
	*	02 = neck
	*	03 = tors
	*	04 = rarm
	*	05 = larm
	*	06 = lrin
	*	07 = rrin
	*	08 = belt
	*	09 = feet
	*	0A = glov
	*	0B = ralt
	*	0C = lalt
*/
	BYTE	page;						//+45
	/*	page dans laquel se trouve l'item
	*	FF = mouse/equip/onEarth
	*	00 = inventory
	*   01 = cube
	*	04 = stash
	*/
	BYTE	uk4[0x01];					//+46
	BYTE	ItemData3;					//+47 //D2Common10854 D2Common10853
	BYTE	pEarLevel;					//+48
	BYTE	varGfx;						//+49
	char	IName[0x12];				//+4A //inscribed/ear name  
	UnitInventory* ptInventory;			//+5C
	UnitAnyHM* ptPrevItem;					//+60
	UnitAnyHM* ptNextItem;					//+64
	BYTE	uk8[0x01];					//+68
	BYTE	ItemData2;					//+69
	BYTE	uk9[0x0A];					//+6A
};

//by zyl from MxCen 113
struct UnitAnyHM
{										//Offset from Code.		Size: 0xF4+4
	DWORD		nUnitType;				//+00
	union {
		DWORD			nPlayerClass;
		DWORD			dwTxtFileNo;
	};									//+04
	DWORD		nMemoryPool;				//+08
	DWORD		nAllocUnitID;				//+0C
	DWORD		CurrentAnim;			//+10
	union {
		MonsterDataHM* ptMonsterData;
		ObjectData* ptObjectData;
		ItemDataHM* ptItemData;
		PlayerDataHM* ptPlayerData;
	};									//+14
	BYTE		act;					//+18
	BYTE		uk12[3];				//+19
	void* ptAct;					//+1C
	DWORD		SeedLow;				//+20
	DWORD		SeedHi;					//+24
	DWORD		SeedStarting;			//+28
	union {
		StaticPath* pItemPath;	//	(Objects, VisTiles, Items) 
		DynamicPath* pMonPath;	//	(Players, Monsters, Missiles) 
	};	//+2C
	BYTE		uk1[0x08];				//+30
	Path* ptPos;					//+38
	DWORD		animSpeed;				//+3C
	BYTE		uk2[0x04];				//+40
	DWORD		curFrame;				//+44
	DWORD		remainingFrame;			//+48
	WORD		animSpeedW;				//+4C
	BYTE		actionFlag;				//+4E
	BYTE		uk3[0x1];				//+4F
	void* ptAnimData;				//+50
	BYTE		uk4[0x08];				//+54
	StatsList* ptStats;				//+5C
	UnitInventory* ptInventory;		    //+60
	DWORD		uk5[7];				//+64
	GameStruct* ptGame;					//+80
	DWORD		uk6[4];				//+84
	DWORD	dwOwnerType;	//+94
	DWORD	dwOwnerId;		//+98
	DWORD	_3[3];		//+9C
	SkillInfo* ptSkills;				//+A8
	void* ptCombatData;			//+AC
	BYTE		uk7[0x08];				//+B0
	DWORD		itemCode;				//+B8 for an item being dropped by this unit
	BYTE		uk8[0x08];				//+BC
	DWORD		flags1;					//+C4
	union {
		DWORD		flags2;				//+C8	//flags
		struct {
			DWORD	ukf1 : 25;
			DWORD	isLod : 1;
		};
	};
	BYTE		uk9[0x04];				//+CC
	DWORD		clientId;				//+D0
	BYTE		uk10[0x0C];				//+D4
	UnitAny* ptFirstMonster;			//+E0
	UnitAny* Unit1;					//+E4
	UnitAny* Unit2;					//+E8
	BYTE		uk11[0x08];				//+EC
	union {
		CBPlayerData* ptCBPlayerData;
		CBItemData* ptCBItemData;
		void* ptCBData;
	};									//+F4
};



struct UnitAny {
	DWORD dwType;					//0x00
	DWORD dwTxtFileNo;				//0x04
	DWORD _1;						//0x08
	DWORD dwUnitId;					//0x0C
	DWORD dwMode;					//0x10
	union
	{
		PlayerData* pPlayerData;
		ItemData* pItemData;
		MonsterData* pMonsterData;
		ObjectData* pObjectData;
		//TileData *pTileData doesn't appear to exist anymore
	};								//0x14
	DWORD dwAct;					//0x18
	Act* pAct;						//0x1C
	DWORD dwSeed[2];				//0x20
	DWORD _2;						//0x28
	union
	{
		Path* pPath;
		ItemPath* pItemPath;
		ObjectPath* pObjectPath;
	};								//0x2C
	DWORD _3[5];					//0x30
	DWORD dwGfxFrame;				//0x44
	DWORD dwFrameRemain;			//0x48
	WORD wFrameRate;				//0x4C
	WORD _4;						//0x4E
	BYTE* pGfxUnk;					//0x50
	DWORD* pGfxInfo;				//0x54
	DWORD _5;						//0x58
	StatList* pStats;				//0x5C
	Inventory* pInventory;			//0x60
	Light* ptLight;					//0x64
	DWORD _6[9];					//0x68
	WORD wX;						//0x8C
	WORD wY;						//0x8E
	DWORD _7;						//0x90
	DWORD dwOwnerType;				//0x94
	DWORD dwOwnerId;				//0x98
	DWORD _8[2];					//0x9C
	OverheadMsg* pOMsg;				//0xA4
	Info* pInfo;					//0xA8
	DWORD _9[6];					//0xAC
	DWORD dwFlags;					//0xC4
	DWORD dwFlags2;					//0xC8
	DWORD _10[5];					//0xCC
	UnitAny* pChangedNext;			//0xE0
	UnitAny* pRoomNext;				//0xE4
	UnitAny* pListNext;				//0xE8 -> 0xD8
};

struct BnetData {
	DWORD dwId;					//0x00
	DWORD dwId2;				//0x04	
	BYTE _12[13];				//0xC0
	//DWORD dwId3;				//0x14
	//WORD Unk3;					//0x18	
	BYTE _13[6];				//0xC0
	char szGameName[22];		//0x1A
	char szGameIP[16];			//0x30
	DWORD _2[15];				//0x40
	DWORD dwId4;				//0x80
	BYTE _3[5];					//0x84
	char szAccountName[48];		//0x88
	char szPlayerName[24];		//0xB8
	char szRealmName[8];		//0xD0
	BYTE _4[273];				//0xD8
	BYTE nCharClass;			//0x1E9
	BYTE nCharFlags;			//0x1EA
	BYTE nMaxLvlDifference;		//0x1EB
	BYTE _5[31];				//0x1EC
	BYTE nDifficulty;			//0x20B
	void* _6;					//0x20C
	DWORD _7[3];				//0x210
	WORD _8;					//0x224
	BYTE _9[7];					//0x226
	char szRealmName2[24];		//0x227
	char szGamePass[24];		//0x23F
	char szGameDesc[256];		//0x257
	WORD _10;					//0x348
	BYTE _11;					//0x34B
};


struct WardenClientRegion_t {
	DWORD cbAllocSize; //+00
	DWORD offsetFunc1; //+04
	DWORD offsetRelocAddressTable; //+08
	DWORD nRelocCount; //+0c
	DWORD offsetWardenSetup; //+10
	DWORD _2[2];
	DWORD offsetImportAddressTable; //+1c
	DWORD nImportDllCount; //+20
	DWORD nSectionCount; //+24
};

struct SMemBlock_t {
	DWORD _1[6];
	DWORD cbSize; //+18
	DWORD _2[31];
	BYTE data[1]; //+98
};

struct WardenClient_t {
	WardenClientRegion_t* pWardenRegion; //+00
	DWORD cbSize; //+04
	DWORD nModuleCount; //+08
	DWORD param; //+0c
	DWORD fnSetupWarden; //+10
};

struct WardenIATInfo_t {
	DWORD offsetModuleName;
	DWORD offsetImportTable;
};

struct AttackStruct {
	DWORD dwAttackType;			//0x00
	UnitAny* lpPlayerUnit;		//0x04
	UnitAny* lpTargetUnit;		//0x08
	DWORD dwTargetX;			//0x0C
	DWORD dwTargetY;			//0x10
	DWORD _1;					//0x14
	DWORD _2;					//0x18
};

#pragma pack(push)
#pragma pack(1)

struct NPCMenu {
	DWORD dwNPCClassId;
	DWORD dwEntryAmount;
	WORD wEntryId1;
	WORD wEntryId2;
	WORD wEntryId3;
	WORD wEntryId4;
	WORD _1;
	DWORD dwEntryFunc1;
	DWORD dwEntryFunc2;
	DWORD dwEntryFunc3;
	DWORD dwEntryFunc4;
	BYTE _2[5];
};

struct OverheadMsg {
	DWORD _1;
	DWORD dwTrigger;
	DWORD _2[2];
	char Msg[232];
};

#pragma pack(pop)

struct D2MSG {
	HWND myHWND;
	char lpBuf[256];
};

#pragma warning ( pop )
#pragma optimize ( "", on )

#endif
