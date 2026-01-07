/**
 * @file player.cpp
 *
 * Implementation of player functionality, leveling, actions, creation, loading, etc.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

int plr_lframe_size;
int plr_wframe_size;
BYTE plr_gfx_flag = 0;
int plr_aframe_size;
int myplr;
#ifdef HELLFIRE
Player *Players;
#else
Player Players[MAX_PLRS];
#endif
int plr_fframe_size;
int plr_qframe_size;
BOOL deathflag;
int plr_hframe_size;
int plr_bframe_size;
BYTE plr_gfx_bflag = 0;
int plr_sframe_size;
int deathdelay;
int plr_dframe_size;

/** Maps from armor animation to letter used in graphic files. */
const char ArmourChar[4] = { 'L', 'M', 'H', 0 };
/** Maps from weapon animation to letter used in graphic files. */
const char WepChar[10] = { 'N', 'U', 'S', 'D', 'B', 'A', 'M', 'H', 'T', 0 };
/** Maps from player class to letter used in graphic files. */
const char CharChar[] = {
	'W',
	'R',
	'S',
#ifdef HELLFIRE
	'M',
	'R',
	'W',
	0
};
const char CharCharHF[] = {
	'W',
	'R',
	'S',
	'M',
	'B',
	'C',
#endif
	0
};

/* data */

/** Specifies the X-coordinate delta from the player start location in Tristram. */
int plrxoff[9] = { 0, 2, 0, 2, 1, 0, 1, 2, 1 };
/** Specifies the Y-coordinate delta from the player start location in Tristram. */
int plryoff[9] = { 0, 2, 2, 0, 1, 1, 0, 1, 2 };
/** Specifies the X-coordinate delta from a player, used for instanced when casting resurrect. */
int plrxoff2[9] = { 0, 1, 0, 1, 2, 0, 1, 2, 2 };
/** Specifies the Y-coordinate delta from a player, used for instanced when casting resurrect. */
int plryoff2[9] = { 0, 0, 1, 1, 0, 2, 2, 1, 2 };
/** Specifies the frame of each animation for which an action is triggered, for each player class. */
char PlrGFXAnimLens[NUM_CLASSES][11] = {
	{ 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
	{ 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
	{ 8, 16, 8, 6, 20, 12, 8, 20, 8, 12, 8 },
#ifdef HELLFIRE
	{ 8, 16, 8, 3, 20, 18, 6, 20, 8, 12, 13 },
	{ 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
	{ 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
#endif
};
/** Maps from player class to player velocity. */
int PWVel[NUM_CLASSES][3] = {
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
#ifdef HELLFIRE
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
#endif
};
/** Total number of frames in walk animation. */
int AnimLenFromClass[NUM_CLASSES] = {
	8,
	8,
	8,
#ifdef HELLFIRE
	8,
	8,
	8,
#endif
};
/** Maps from player_class to starting stat in strength. */
int StrengthTbl[NUM_CLASSES] = {
	30,
	20,
	15,
#ifdef HELLFIRE
	25,
	20,
	40,
#endif
};
/** Maps from player_class to starting stat in magic. */
int MagicTbl[NUM_CLASSES] = {
	// clang-format off
	10,
	15,
	35,
#ifdef HELLFIRE
	15,
	20,
	 0,
#endif
	// clang-format on
};
/** Maps from player_class to starting stat in dexterity. */
int DexterityTbl[NUM_CLASSES] = {
	20,
	30,
	15,
#ifdef HELLFIRE
	25,
	25,
	20,
#endif
};
/** Maps from player_class to starting stat in vitality. */
int VitalityTbl[NUM_CLASSES] = {
	25,
	20,
	20,
#ifdef HELLFIRE
	20,
	20,
	25,
#endif
};
/** Specifies the chance to block bonus of each player class.*/
int ToBlkTbl[NUM_CLASSES] = {
	30,
	20,
	10,
#ifdef HELLFIRE
	25,
	25,
	30,
#endif
};
const char *const ClassStrTblOld[] = {
	"Warrior",
	"Rogue",
	"Sorceror",
#ifdef HELLFIRE
	"Monk",
	"Bard",
	"Barbarian",
#endif
};
/** Maps from player_class to maximum stats. */
int MaxStats[NUM_CLASSES][4] = {
	// clang-format off
	{ 250,  50,  60, 100 },
	{  55,  70, 250,  80 },
	{  45, 250,  85,  80 },
#ifdef HELLFIRE
	{ 150,  80, 150,  80 },
	{ 120, 120, 120, 100 },
	{ 255,   0,  55, 150 },
#endif
	// clang-format on
};
/** Specifies the experience point limit of each level. */
int ExpLvlsTbl[MAXCHARLEVEL] = {
	0,
	2000,
	4620,
	8040,
	12489,
	18258,
	25712,
	35309,
	47622,
	63364,
	83419,
	108879,
	141086,
	181683,
	231075,
	313656,
	424067,
	571190,
	766569,
	1025154,
	1366227,
	1814568,
	2401895,
	3168651,
	4166200,
	5459523,
	7130496,
	9281874,
	12042092,
	15571031,
	20066900,
	25774405,
	32994399,
	42095202,
	53525811,
	67831218,
	85670061,
	107834823,
	135274799,
	169122009,
	210720231,
	261657253,
	323800420,
	399335440,
	490808349,
	601170414,
	733825617,
	892680222,
	1082908612,
	1310707109,
	1583495809
};
const char *const ClassStrTbl[NUM_CLASSES] = {
	"Warrior",
	"Rogue",
	"Sorceror",
#ifdef HELLFIRE
	"Monk",
	"Rogue",
	"Warrior",
#endif
};
/** Unused local of PM_ChangeLightOff, originally for computing light radius. */
BYTE fix[9] = { 0, 0, 3, 3, 3, 6, 6, 6, 8 };

void SetPlayerGPtrs(BYTE *pData, BYTE **pAnim)
{
	int i;

	for (i = 0; i < 8; i++) {
		pAnim[i] = pData + ((DWORD *)pData)[i];
	}
}

void LoadPlrGFX(int pnum, player_graphic gfxflag)
{
	char prefix[16];
	char pszName[256];
	const char *szCel;
	Player *p;
	const char *cs;
	BYTE *pData, *pAnim;
	DWORD i;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("LoadPlrGFX: illegal player %d", pnum);
	}

	p = &Players[pnum];
#ifdef HELLFIRE
	if ((p->_pClass != PC_BARD || hfbard_mpq == NULL) && (p->_pClass != PC_BARBARIAN || hfbarb_mpq == NULL)) {
#endif
		sprintf(prefix, "%c%c%c", CharChar[p->_pClass], ArmourChar[p->_pgfxnum >> 4], WepChar[p->_pgfxnum & 0xF]);
		cs = ClassStrTbl[p->_pClass];
#ifdef HELLFIRE
	} else {
		sprintf(prefix, "%c%c%c", CharCharHF[p->_pClass], ArmourChar[p->_pgfxnum >> 4], WepChar[p->_pgfxnum & 0xF]);
		cs = ClassStrTbl[p->_pClass];
		cs = ClassStrTblOld[p->_pClass];
	}
#endif

	for (i = 1; i <= PFILE_NONDEATH; i <<= 1) {
		if (!(i & gfxflag)) {
			continue;
		}

		switch (i) {
		case PFILE_STAND:
			szCel = "AS";
			if (leveltype == DTYPE_TOWN) {
				szCel = "ST";
			}
			pData = p->_pNData;
			pAnim = (BYTE *)p->_pNAnim;
			break;
		case PFILE_WALK:
			szCel = "AW";
			if (leveltype == DTYPE_TOWN) {
				szCel = "WL";
			}
			pData = p->_pWData;
			pAnim = (BYTE *)p->_pWAnim;
			break;
		case PFILE_ATTACK:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "AT";
			pData = p->_pAData;
			pAnim = (BYTE *)p->_pAAnim;
			break;
		case PFILE_HIT:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "HT";
			pData = p->_pHData;
			pAnim = (BYTE *)p->_pHAnim;
			break;
		case PFILE_LIGHTNING:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "LM";
			pData = p->_pLData;
			pAnim = (BYTE *)p->_pLAnim;
			break;
		case PFILE_FIRE:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "FM";
			pData = p->_pFData;
			pAnim = (BYTE *)p->_pFAnim;
			break;
		case PFILE_MAGIC:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "QM";
			pData = p->_pTData;
			pAnim = (BYTE *)p->_pTAnim;
			break;
		case PFILE_DEATH:
			if (p->_pgfxnum & 0xF) {
				continue;
			}
			szCel = "DT";
			pData = p->_pDData;
			pAnim = (BYTE *)p->_pDAnim;
			break;
		case PFILE_BLOCK:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			if (!p->_pBlockFlag) {
				continue;
			}

			szCel = "BL";
			pData = p->_pBData;
			pAnim = (BYTE *)p->_pBAnim;
			break;
		default:
			app_fatal("PLR:2");
			break;
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs, prefix, prefix, szCel);
		LoadFileWithMem(pszName, pData);
		SetPlayerGPtrs((BYTE *)pData, (BYTE **)pAnim);
		p->_pGFXLoad |= i;
	}
}

void InitPlayerGFX(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayerGFX: illegal player %d", pnum);
	}

	if (Players[pnum]._pHitPoints >> 6 == 0) {
		Players[pnum]._pgfxnum = 0;
		LoadPlrGFX(pnum, PFILE_DEATH);
	} else {
		LoadPlrGFX(pnum, PFILE_NONDEATH);
	}
}

static DWORD GetPlrGFXSize(const char *szCel)
{
	DWORD c;
	const char *a, *w;
	DWORD dwSize, dwMaxSize;
	HANDLE hsFile;
	char pszName[256];
	char Type[16];

	dwMaxSize = 0;

	for (c = 0; c < NUM_CLASSES; c++) {
#ifdef SPAWN
		if (c != 0)
			continue;
#endif
		for (a = &ArmourChar[0]; *a; a++) {
#ifdef SPAWN
			if (a != &ArmourChar[0])
				break;
#endif
			for (w = &WepChar[0]; *w; w++) { // BUGFIX loads non-existing animagions; DT is only for N, BT is only for U, D & H
#ifdef HELLFIRE
				if ((c == PC_BARD && hfbard_mpq == NULL) || (c == PC_BARBARIAN && hfbarb_mpq == NULL)) {
#endif
					sprintf(Type, "%c%c%c", CharChar[c], *a, *w);
					sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", ClassStrTbl[c], Type, Type, szCel);
#ifdef HELLFIRE
				} else {
					sprintf(Type, "%c%c%c", CharCharHF[c], *a, *w);
					sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", ClassStrTblOld[c], Type, Type, szCel);
				}
#endif
				if (WOpenFile(pszName, &hsFile, TRUE)) {
					/// ASSERT: assert(hsFile);
					dwSize = WGetFileSize(hsFile, NULL);
					WCloseFile(hsFile);
					if (dwMaxSize <= dwSize) {
						dwMaxSize = dwSize;
					}
				}
			}
		}
	}

	return dwMaxSize;
}

void InitPlrGFXMem(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlrGFXMem: illegal player %d", pnum);
	}

	if (!(plr_gfx_flag & 0x1)) { // STAND
		plr_gfx_flag |= 0x1;
		if (GetPlrGFXSize("ST") > GetPlrGFXSize("AS")) {
			plr_sframe_size = GetPlrGFXSize("ST"); // TOWN
		} else {
			plr_sframe_size = GetPlrGFXSize("AS"); // DUNGEON
		}
	}
	Players[pnum]._pNData = DiabloAllocPtr(plr_sframe_size);

	if (!(plr_gfx_flag & 0x2)) { // WALK
		plr_gfx_flag |= 0x2;
		if (GetPlrGFXSize("WL") > GetPlrGFXSize("AW")) {
			plr_wframe_size = GetPlrGFXSize("WL"); // TOWN
		} else {
			plr_wframe_size = GetPlrGFXSize("AW"); // DUNGEON
		}
	}
	Players[pnum]._pWData = DiabloAllocPtr(plr_wframe_size);

	if (!(plr_gfx_flag & 0x4)) { // ATTACK
		plr_gfx_flag |= 0x4;
		plr_aframe_size = GetPlrGFXSize("AT");
	}
	Players[pnum]._pAData = DiabloAllocPtr(plr_aframe_size);

	if (!(plr_gfx_flag & 0x8)) { // HIT
		plr_gfx_flag |= 0x8;
		plr_hframe_size = GetPlrGFXSize("HT");
	}
	Players[pnum]._pHData = DiabloAllocPtr(plr_hframe_size);

	if (!(plr_gfx_flag & 0x10)) { // LIGHTNING
		plr_gfx_flag |= 0x10;
		plr_lframe_size = GetPlrGFXSize("LM");
	}
	Players[pnum]._pLData = DiabloAllocPtr(plr_lframe_size);

	if (!(plr_gfx_flag & 0x20)) { // FIRE
		plr_gfx_flag |= 0x20;
		plr_fframe_size = GetPlrGFXSize("FM");
	}
	Players[pnum]._pFData = DiabloAllocPtr(plr_fframe_size);

	if (!(plr_gfx_flag & 0x40)) { // MAGIC
		plr_gfx_flag |= 0x40;
		plr_qframe_size = GetPlrGFXSize("QM");
	}
	Players[pnum]._pTData = DiabloAllocPtr(plr_qframe_size);

	if (!(plr_gfx_flag & 0x80)) { // DEATH
		plr_gfx_flag |= 0x80;
		plr_dframe_size = GetPlrGFXSize("DT");
	}
	Players[pnum]._pDData = DiabloAllocPtr(plr_dframe_size);

	if (!(plr_gfx_bflag & 0x1)) { // BLOCK
		plr_gfx_bflag |= 0x1;
		plr_bframe_size = GetPlrGFXSize("BL");
	}
	Players[pnum]._pBData = DiabloAllocPtr(plr_bframe_size);

	Players[pnum]._pGFXLoad = 0;
}

void FreePlayerGFX(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FreePlayerGFX: illegal player %d", pnum);
	}

	MemFreeDbg(Players[pnum]._pNData);
	MemFreeDbg(Players[pnum]._pWData);
	MemFreeDbg(Players[pnum]._pAData);
	MemFreeDbg(Players[pnum]._pHData);
	MemFreeDbg(Players[pnum]._pLData);
	MemFreeDbg(Players[pnum]._pFData);
	MemFreeDbg(Players[pnum]._pTData);
	MemFreeDbg(Players[pnum]._pDData);
	MemFreeDbg(Players[pnum]._pBData);
	Players[pnum]._pGFXLoad = 0;
}

void NewPlrAnim(int pnum, BYTE *Peq, int numFrames, int Delay, int width)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("NewPlrAnim: illegal player %d", pnum);
	}

	Players[pnum]._pAnimData = Peq;
	Players[pnum]._pAnimLen = numFrames;
	Players[pnum]._pAnimFrame = 1;
	Players[pnum]._pAnimCnt = 0;
	Players[pnum]._pAnimDelay = Delay;
	Players[pnum]._pAnimWidth = width;
	Players[pnum]._pAnimWidth2 = (width - 64) >> 1;
}

void ClearPlrPVars(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ClearPlrPVars: illegal player %d", pnum);
	}

	Players[pnum]._pVar1 = 0;
	Players[pnum]._pVar2 = 0;
	Players[pnum]._pVar3 = 0;
	Players[pnum]._pVar4 = 0;
	Players[pnum]._pVar5 = 0;
	Players[pnum]._pVar6 = 0;
	Players[pnum]._pVar7 = 0;
	Players[pnum]._pVar8 = 0;
}

void SetPlrAnims(int pnum)
{
	int pc, gn;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlrAnims: illegal player %d", pnum);
	}

	Players[pnum]._pNWidth = 96;
	Players[pnum]._pWWidth = 96;
	Players[pnum]._pAWidth = 128;
	Players[pnum]._pHWidth = 96;
	Players[pnum]._pSWidth = 96;
	Players[pnum]._pDWidth = 128;
	Players[pnum]._pBWidth = 96;

	pc = Players[pnum]._pClass;

	if (leveltype == DTYPE_TOWN) {
		Players[pnum]._pNFrames = PlrGFXAnimLens[pc][7];
		Players[pnum]._pWFrames = PlrGFXAnimLens[pc][8];
		Players[pnum]._pDFrames = PlrGFXAnimLens[pc][4];
		Players[pnum]._pSFrames = PlrGFXAnimLens[pc][5];
	} else {
		Players[pnum]._pNFrames = PlrGFXAnimLens[pc][0];
		Players[pnum]._pWFrames = PlrGFXAnimLens[pc][2];
		Players[pnum]._pAFrames = PlrGFXAnimLens[pc][1];
		Players[pnum]._pHFrames = PlrGFXAnimLens[pc][6];
		Players[pnum]._pSFrames = PlrGFXAnimLens[pc][5];
		Players[pnum]._pDFrames = PlrGFXAnimLens[pc][4];
		Players[pnum]._pBFrames = PlrGFXAnimLens[pc][3];
		Players[pnum]._pAFNum = PlrGFXAnimLens[pc][9];
	}
	Players[pnum]._pSFNum = PlrGFXAnimLens[pc][10];

	gn = Players[pnum]._pgfxnum & 0xF;
	if (pc == PC_WARRIOR) {
		if (gn == ANIM_ID_BOW) {
			if (leveltype != DTYPE_TOWN) {
				Players[pnum]._pNFrames = 8;
			}
			Players[pnum]._pAWidth = 96;
			Players[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_AXE) {
			Players[pnum]._pAFrames = 20;
			Players[pnum]._pAFNum = 10;
		} else if (gn == ANIM_ID_STAFF) {
			Players[pnum]._pAFrames = 16;
			Players[pnum]._pAFNum = 11;
		}
#ifndef SPAWN
	} else if (pc == PC_ROGUE) {
		if (gn == ANIM_ID_AXE) {
			Players[pnum]._pAFrames = 22;
			Players[pnum]._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			Players[pnum]._pAFrames = 12;
			Players[pnum]._pAFNum = 7;
		} else if (gn == ANIM_ID_STAFF) {
			Players[pnum]._pAFrames = 16;
			Players[pnum]._pAFNum = 11;
		}
	} else if (pc == PC_SORCERER) {
		Players[pnum]._pSWidth = 128;
		if (gn == ANIM_ID_UNARMED) {
			Players[pnum]._pAFrames = 20;
		} else if (gn == ANIM_ID_UNARMED_SHIELD) {
			Players[pnum]._pAFNum = 9;
		} else if (gn == ANIM_ID_BOW) {
			Players[pnum]._pAFrames = 20;
			Players[pnum]._pAFNum = 16;
		} else if (gn == ANIM_ID_AXE) {
			Players[pnum]._pAFrames = 24;
			Players[pnum]._pAFNum = 16;
		}
#endif
#ifdef HELLFIRE
	} else if (pc == PC_MONK) {
		Players[pnum]._pNWidth = 112;
		Players[pnum]._pWWidth = 112;
		Players[pnum]._pAWidth = 130;
		Players[pnum]._pHWidth = 98;
		Players[pnum]._pSWidth = 114;
		Players[pnum]._pDWidth = 160;
		Players[pnum]._pBWidth = 98;

		switch (gn) {
		case ANIM_ID_UNARMED:
		case ANIM_ID_UNARMED_SHIELD:
			Players[pnum]._pAFrames = 12;
			Players[pnum]._pAFNum = 7;
			break;
		case ANIM_ID_BOW:
			Players[pnum]._pAFrames = 20;
			Players[pnum]._pAFNum = 14;
			break;
		case ANIM_ID_AXE:
			Players[pnum]._pAFrames = 23;
			Players[pnum]._pAFNum = 14;
			break;
		case ANIM_ID_STAFF:
			Players[pnum]._pAFrames = 13;
			Players[pnum]._pAFNum = 8;
			break;
		}
	} else if (pc == PC_BARD) {
		if (gn == ANIM_ID_AXE) {
			Players[pnum]._pAFrames = 22;
			Players[pnum]._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			Players[pnum]._pAFrames = 12;
			Players[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_STAFF) {
			Players[pnum]._pAFrames = 16;
			Players[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_SWORD_SHIELD || gn == ANIM_ID_SWORD) {
			Players[pnum]._pAFNum = 10;
		}
	} else if (pc == PC_BARBARIAN) {
		if (gn == ANIM_ID_AXE) {
			Players[pnum]._pAFrames = 20;
			Players[pnum]._pAFNum = 8;
		} else if (gn == ANIM_ID_BOW) {
			if (leveltype != DTYPE_TOWN) {
				Players[pnum]._pNFrames = 8;
			}
			Players[pnum]._pAWidth = 96;
			Players[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_STAFF) {
			Players[pnum]._pAFrames = 16;
			Players[pnum]._pAFNum = 11;
		} else if (gn == ANIM_ID_MACE || gn == ANIM_ID_MACE_SHIELD) {
			Players[pnum]._pAFNum = 8;
		}
#endif
	}
}

void ClearPlrRVars(Player *p)
{
	// TODO: Missing debug assert p != NULL
#ifdef HELLFIRE
	p->pManaShield = 0;
#endif
	p->bReserved[0] = 0;
	p->bReserved[1] = 0;
	p->bReserved[2] = 0;

#ifndef HELLFIRE
	p->wReflections = 0;
#endif
	p->wReserved[0] = 0;
	p->wReserved[1] = 0;
	p->wReserved[2] = 0;
	p->wReserved[3] = 0;
	p->wReserved[4] = 0;
	p->wReserved[5] = 0;
	p->wReserved[6] = 0;

#ifndef HELLFIRE
	p->pDifficulty = 0;
	p->pDamAcFlags = 0;
#endif
	p->dwReserved[0] = 0;
	p->dwReserved[1] = 0;
	p->dwReserved[2] = 0;
	p->dwReserved[3] = 0;
	p->dwReserved[4] = 0;
}

/**
 * @param c plr_classes value
 */
void CreatePlayer(int pnum, char c)
{
	char val;
	int hp, mana;
	int i;

#ifdef HELLFIRE
	memset(&Players[pnum], 0, sizeof(Player));
#endif
	ClearPlrRVars(&Players[pnum]);
	SetRndSeed(GetTickCount());

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("CreatePlayer: illegal player %d", pnum);
	}
	Players[pnum]._pClass = c;

	val = StrengthTbl[c];
	if (val < 0) {
		val = 0;
	}
	Players[pnum]._pStrength = val;
	Players[pnum]._pBaseStr = val;

	val = MagicTbl[c];
	if (val < 0) {
		val = 0;
	}
	Players[pnum]._pMagic = val;
	Players[pnum]._pBaseMag = val;

	val = DexterityTbl[c];
	if (val < 0) {
		val = 0;
	}
	Players[pnum]._pDexterity = val;
	Players[pnum]._pBaseDex = val;

	val = VitalityTbl[c];
	if (val < 0) {
		val = 0;
	}
	Players[pnum]._pVitality = val;
	Players[pnum]._pBaseVit = val;

	Players[pnum]._pStatPts = 0;
	Players[pnum].pTownWarps = 0;
	Players[pnum].pDungMsgs = 0;
#ifdef HELLFIRE
	Players[pnum].pDungMsgs2 = 0;
#endif
	Players[pnum].pLvlLoad = 0;
	Players[pnum].pDiabloKillLevel = 0;
#ifdef HELLFIRE
	Players[pnum].pDifficulty = DIFF_NORMAL;
#endif

#ifdef HELLFIRE
	if (Players[pnum]._pClass == PC_MONK) {
		Players[pnum]._pDamageMod = (Players[pnum]._pStrength + Players[pnum]._pDexterity) * Players[pnum]._pLevel / 150;
	} else if (Players[pnum]._pClass == PC_ROGUE || Players[pnum]._pClass == PC_BARD) {
#else
	if (Players[pnum]._pClass == PC_ROGUE) {
#endif
		Players[pnum]._pDamageMod = Players[pnum]._pLevel * (Players[pnum]._pStrength + Players[pnum]._pDexterity) / 200;
	} else {
		Players[pnum]._pDamageMod = Players[pnum]._pStrength * Players[pnum]._pLevel / 100;
	}

	Players[pnum]._pBaseToBlk = ToBlkTbl[c]; // BUGFIX: _pBaseToBlk not set in player struct if creating a New Game using an existing save file (since CreatePlayer is never invoked in this case).

	Players[pnum]._pHitPoints = (Players[pnum]._pVitality + 10) << 6;
	if (Players[pnum]._pClass == PC_WARRIOR
#ifdef HELLFIRE
	    || Players[pnum]._pClass == PC_BARBARIAN
#endif
	) {
		Players[pnum]._pHitPoints <<= 1;
#ifdef HELLFIRE
	} else if (Players[pnum]._pClass == PC_ROGUE || Players[pnum]._pClass == PC_MONK || Players[pnum]._pClass == PC_BARD) {
#else
	}
	if (Players[pnum]._pClass == PC_ROGUE) {
#endif
		Players[pnum]._pHitPoints += Players[pnum]._pHitPoints >> 1;
	}

	Players[pnum]._pMaxHP = Players[pnum]._pHitPoints;
	Players[pnum]._pHPBase = Players[pnum]._pHitPoints;
	Players[pnum]._pMaxHPBase = Players[pnum]._pHitPoints;

	Players[pnum]._pMana = Players[pnum]._pMagic << 6;
	if (Players[pnum]._pClass == PC_SORCERER) {
		Players[pnum]._pMana <<= 1;
#ifdef HELLFIRE
	} else if (Players[pnum]._pClass == PC_BARD) {
		Players[pnum]._pMana += Players[pnum]._pMana * 3 / 4;
	} else if (Players[pnum]._pClass == PC_ROGUE
	    || Players[pnum]._pClass == PC_MONK) {
#else
	}
	if (Players[pnum]._pClass == PC_ROGUE) {
#endif
		Players[pnum]._pMana += Players[pnum]._pMana >> 1;
	}

	Players[pnum]._pMaxMana = Players[pnum]._pMana;
	Players[pnum]._pManaBase = Players[pnum]._pMana;
	Players[pnum]._pMaxManaBase = Players[pnum]._pMana;

	Players[pnum]._pLevel = 1;
	Players[pnum]._pMaxLvl = Players[pnum]._pLevel;
	Players[pnum]._pExperience = 0;
	Players[pnum]._pMaxExp = Players[pnum]._pExperience;
	Players[pnum]._pNextExper = ExpLvlsTbl[1];
	Players[pnum]._pArmorClass = 0;
#ifdef HELLFIRE
	if (Players[pnum]._pClass == PC_BARBARIAN) {
		Players[pnum]._pMagResist = 1;
		Players[pnum]._pFireResist = 1;
		Players[pnum]._pLghtResist = 1;
	} else {
#endif
		Players[pnum]._pMagResist = 0;
		Players[pnum]._pFireResist = 0;
		Players[pnum]._pLghtResist = 0;
#ifdef HELLFIRE
	}
#endif
	Players[pnum]._pLightRad = 10;
	Players[pnum]._pInfraFlag = FALSE;

	if (c == PC_WARRIOR) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_REPAIR);
#ifndef SPAWN
	} else if (c == PC_ROGUE) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_DISARM);
	} else if (c == PC_SORCERER) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_RECHARGE);
#endif
#ifdef HELLFIRE
	} else if (c == PC_MONK) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_SEARCH);
	} else if (c == PC_BARD) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_IDENTIFY);
	} else if (c == PC_BARBARIAN) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_BLODBOIL);
#endif
	}

	if (c == PC_SORCERER) {
		Players[pnum]._pMemSpells = SPELLBIT(SPL_FIREBOLT);
	} else {
		Players[pnum]._pMemSpells = 0;
	}

	for (i = 0; i < sizeof(Players[pnum]._pSplLvl) / sizeof(Players[pnum]._pSplLvl[0]); i++) {
		Players[pnum]._pSplLvl[i] = 0;
	}

	Players[pnum]._pSpellFlags = 0;

	if (Players[pnum]._pClass == PC_SORCERER) {
		Players[pnum]._pSplLvl[SPL_FIREBOLT] = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for (i = 0; i < 3; i++) {
		Players[pnum]._pSplHotKey[i] = -1;
	}

	if (c == PC_WARRIOR) {
		Players[pnum]._pgfxnum = ANIM_ID_SWORD_SHIELD;
#ifndef SPAWN
	} else if (c == PC_ROGUE) {
		Players[pnum]._pgfxnum = ANIM_ID_BOW;
	} else if (c == PC_SORCERER) {
		Players[pnum]._pgfxnum = ANIM_ID_STAFF;
#endif
#ifdef HELLFIRE
	} else if (c == PC_MONK) {
		Players[pnum]._pgfxnum = ANIM_ID_STAFF;
	} else if (c == PC_BARD) {
		Players[pnum]._pgfxnum = ANIM_ID_SWORD_SHIELD;
	} else if (c == PC_BARBARIAN) {
		Players[pnum]._pgfxnum = ANIM_ID_SWORD_SHIELD;
#endif
	}

	for (i = 0; i < NUMLEVELS; i++) {
		Players[pnum]._pLvlVisited[i] = FALSE;
	}

	for (i = 0; i < 10; i++) {
		Players[pnum]._pSLvlVisited[i] = FALSE;
	}

	Players[pnum]._pLvlChanging = FALSE;
	Players[pnum].pTownWarps = 0;
	Players[pnum].pLvlLoad = 0;
#ifndef HELLFIRE
	Players[pnum].pBattleNet = FALSE;
	Players[pnum].pManaShield = FALSE;
#else
	Players[pnum].pDamAcFlags = 0;
	Players[pnum].wReflections = 0;
#endif

	InitDungMsgs(pnum);
	CreatePlrItems(pnum);
	SetRndSeed(0);
}

int CalcStatDiff(int pnum)
{
	int c;

	c = Players[pnum]._pClass;
	return MaxStats[c][ATTRIB_STR]
	    - Players[pnum]._pBaseStr
	    + MaxStats[c][ATTRIB_MAG]
	    - Players[pnum]._pBaseMag
	    + MaxStats[c][ATTRIB_DEX]
	    - Players[pnum]._pBaseDex
	    + MaxStats[c][ATTRIB_VIT]
	    - Players[pnum]._pBaseVit;
}

void NextPlrLevel(int pnum)
{
	int hp, mana;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("NextPlrLevel: illegal player %d", pnum);
	}

	Players[pnum]._pLevel++;
	Players[pnum]._pMaxLvl++;

	if (CalcStatDiff(pnum) < 5) {
		Players[pnum]._pStatPts = CalcStatDiff(pnum);
	} else {
		Players[pnum]._pStatPts += 5;
	}

	Players[pnum]._pNextExper = ExpLvlsTbl[Players[pnum]._pLevel];

	hp = Players[pnum]._pClass == PC_SORCERER ? 64 : 128;
	if (gbMaxPlayers == 1) {
		hp++;
	}
	Players[pnum]._pMaxHP += hp;
	Players[pnum]._pHitPoints = Players[pnum]._pMaxHP;
	Players[pnum]._pMaxHPBase += hp;
	Players[pnum]._pHPBase = Players[pnum]._pMaxHPBase;

	if (pnum == myplr) {
		drawhpflag = TRUE;
	}

	if (Players[pnum]._pClass == PC_WARRIOR)
		mana = 64;
#ifdef HELLFIRE
	else if (Players[pnum]._pClass == PC_BARBARIAN)
		mana = 0;
#endif
	else
		mana = 128;

	if (gbMaxPlayers == 1) {
		mana++;
	}
	Players[pnum]._pMaxMana += mana;
	Players[pnum]._pMaxManaBase += mana;

	if (!(Players[pnum]._pIFlags & ISPL_NOMANA)) {
		Players[pnum]._pMana = Players[pnum]._pMaxMana;
		Players[pnum]._pManaBase = Players[pnum]._pMaxManaBase;
	}

	if (pnum == myplr) {
#ifdef HELLFIRE
		if (Players[pnum]._pMana > 0)
#endif
			drawmanaflag = TRUE;
	}
}

void AddPlrExperience(int pnum, int lvl, int exp)
{
	int powerLvlCap, expCap, newLvl, i;

	if (pnum != myplr) {
		return;
	}

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("AddPlrExperience: illegal player %d", myplr);
	}

	if (Players[myplr]._pHitPoints <= 0) {
		return;
	}

	// Adjust xp based on difference in level between player and monster
	exp *= 1 + ((double)lvl - Players[pnum]._pLevel) / 10;
	if (exp < 0) {
		exp = 0;
	}

	// Prevent power leveling
	if (gbMaxPlayers > 1) {
		powerLvlCap = Players[pnum]._pLevel < 0 ? 0 : Players[pnum]._pLevel;
		if (powerLvlCap >= 50) {
			powerLvlCap = 50;
		}
		// cap to 1/20 of current levels xp
		if (exp >= ExpLvlsTbl[powerLvlCap] / 20) {
			exp = ExpLvlsTbl[powerLvlCap] / 20;
		}
		// cap to 200 * current level
		expCap = 200 * powerLvlCap;
		if (exp >= expCap) {
			exp = expCap;
		}
	}

	Players[pnum]._pExperience += exp;
	if ((DWORD)Players[pnum]._pExperience > MAXEXP) {
		Players[pnum]._pExperience = MAXEXP;
	}

	if (Players[pnum]._pExperience >= ExpLvlsTbl[49]) {
		Players[pnum]._pLevel = 50;
		return;
	}

	// Increase player level if applicable
	newLvl = 0;
	while (Players[pnum]._pExperience >= ExpLvlsTbl[newLvl]) {
		newLvl++;
	}
	if (newLvl != Players[pnum]._pLevel) {
		for (i = newLvl - Players[pnum]._pLevel; i > 0; i--) {
			NextPlrLevel(pnum);
		}
	}

	NetSendCmdParam1(FALSE, CMD_PLRLEVEL, Players[myplr]._pLevel);
}

void AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs, i, e;

	totplrs = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if ((1 << i) & pmask) {
			totplrs++;
		}
	}

	if (totplrs) {
		e = exp / totplrs;
		if (pmask & (1 << myplr))
			AddPlrExperience(myplr, lvl, e);
	}
}

void InitPlayer(int pnum, BOOL FirstTime)
{
	DWORD i;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", pnum);
	}

	ClearPlrRVars(&Players[pnum]);

	if (FirstTime) {
		Players[pnum]._pRSplType = RSPLTYPE_INVALID;
		Players[pnum]._pRSpell = SPL_INVALID;
		Players[pnum]._pSBkSpell = SPL_INVALID;
		Players[pnum]._pSpell = Players[pnum]._pRSpell;
		Players[pnum]._pSplType = Players[pnum]._pRSplType;
		if ((Players[pnum]._pgfxnum & 0xF) == ANIM_ID_BOW) {
			Players[pnum]._pwtype = WT_RANGED;
		} else {
			Players[pnum]._pwtype = WT_MELEE;
		}
#ifndef HELLFIRE
		Players[pnum].pManaShield = FALSE;
#endif
	}

	if (Players[pnum].plrlevel == currlevel || leveldebug) {

		SetPlrAnims(pnum);

		Players[pnum]._pxoff = 0;
		Players[pnum]._pyoff = 0;
		Players[pnum]._pxvel = 0;
		Players[pnum]._pyvel = 0;

		ClearPlrPVars(pnum);

		if (Players[pnum]._pHitPoints >> 6 > 0) {
			Players[pnum]._pmode = PM_STAND;
			NewPlrAnim(pnum, Players[pnum]._pNAnim[DIR_S], Players[pnum]._pNFrames, 3, Players[pnum]._pNWidth);
			Players[pnum]._pAnimFrame = random_(2, Players[pnum]._pNFrames - 1) + 1;
			Players[pnum]._pAnimCnt = random_(2, 3);
		} else {
			Players[pnum]._pmode = PM_DEATH;
			NewPlrAnim(pnum, Players[pnum]._pDAnim[DIR_S], Players[pnum]._pDFrames, 1, Players[pnum]._pDWidth);
			Players[pnum]._pAnimFrame = Players[pnum]._pAnimLen - 1;
			Players[pnum]._pVar8 = 2 * Players[pnum]._pAnimLen;
		}

		Players[pnum]._pdir = DIR_S;
		Players[pnum]._peflag = 0;

		if (pnum == myplr) {
			if (!FirstTime || currlevel != 0) {
				Players[pnum]._px = ViewX;
				Players[pnum]._py = ViewY;
			}
			Players[pnum]._ptargx = Players[pnum]._px;
			Players[pnum]._ptargy = Players[pnum]._py;
		} else {
			Players[pnum]._ptargx = Players[pnum]._px;
			Players[pnum]._ptargy = Players[pnum]._py;
			for (i = 0; i < 8 && !PosOkPlayer(pnum, plrxoff2[i] + Players[pnum]._px, plryoff2[i] + Players[pnum]._py); i++)
				;
			Players[pnum]._px += plrxoff2[i];
			Players[pnum]._py += plryoff2[i];
		}

		Players[pnum]._pfutx = Players[pnum]._px;
		Players[pnum]._pfuty = Players[pnum]._py;
		Players[pnum].walkpath[0] = WALK_NONE;
		Players[pnum].destAction = ACTION_NONE;

		if (pnum == myplr) {
			Players[pnum]._plid = AddLight(Players[pnum]._px, Players[pnum]._py, Players[pnum]._pLightRad);
		} else {
			Players[pnum]._plid = -1;
		}
		Players[pnum]._pvid = AddVision(Players[pnum]._px, Players[pnum]._py, Players[pnum]._pLightRad, pnum == myplr);
	}

	if (Players[pnum]._pClass == PC_WARRIOR) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_REPAIR);
#ifndef SPAWN
	} else if (Players[pnum]._pClass == PC_ROGUE) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_DISARM);
	} else if (Players[pnum]._pClass == PC_SORCERER) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_RECHARGE);
#ifdef HELLFIRE
	} else if (Players[pnum]._pClass == PC_MONK) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_SEARCH);
	} else if (Players[pnum]._pClass == PC_BARD) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_IDENTIFY);
	} else if (Players[pnum]._pClass == PC_BARBARIAN) {
		Players[pnum]._pAblSpells = SPELLBIT(SPL_BLODBOIL);
#endif
#endif
	}

#ifdef _DEBUG
	if (debug_mode_dollar_sign && FirstTime) {
		Players[pnum]._pMemSpells |= 1 << (SPL_TELEPORT - 1);
		if (!Players[myplr]._pSplLvl[SPL_TELEPORT]) {
			Players[myplr]._pSplLvl[SPL_TELEPORT] = 1;
		}
	}
	if (debug_mode_key_inverted_v && FirstTime) {
		Players[pnum]._pMemSpells = SPL_INVALID;
	}
#endif

	Players[pnum]._pNextExper = ExpLvlsTbl[Players[pnum]._pLevel];
	Players[pnum]._pInvincible = FALSE;

	if (pnum == myplr) {
		deathdelay = 0;
		deathflag = FALSE;
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SDIR_NONE;
	}
}

void InitMultiView()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", myplr);
	}

	ViewX = Players[myplr]._px;
	ViewY = Players[myplr]._py;
}

void CheckEFlag(int pnum, BOOL flag)
{
	int x, y, i;
	int bitflags;
	MICROS *pieces;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %d", pnum);
	}

	x = Players[pnum]._px - 1;
	y = Players[pnum]._py + 1;
	bitflags = 0;
	pieces = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 2; i < 10; i++) {
		bitflags |= pieces->mt[i];
	}

	if (bitflags | nSolidTable[dPiece[x][y]] | dSpecial[x][y]) {
		Players[pnum]._peflag = 1;
	} else {
		Players[pnum]._peflag = 0;
	}

	if (flag != 1 || Players[pnum]._peflag != 1) {
		return;
	}

	x = Players[pnum]._px;
	y = Players[pnum]._py + 2;
	bitflags = 0;
	pieces = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 2; i < 10; i++) {
		bitflags |= pieces->mt[i];
	}

	if (bitflags | dSpecial[x][y]) {
		return;
	}

	x = Players[pnum]._px - 2;
	y = Players[pnum]._py + 1;
	bitflags = 0;
	pieces = &dpiece_defs_map_1[IsometricCoord(x, y)];

	for (i = 2; i < 10; i++) {
		bitflags |= pieces->mt[i];
	}

	if (bitflags | dSpecial[x][y]) {
		Players[pnum]._peflag = 2;
	}
}

BOOL SolidLoc(int x, int y)
{
#ifndef HELLFIRE
	if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY) {
		return FALSE;
	}
#endif

	return nSolidTable[dPiece[x][y]];
}

BOOL PlrDirOK(int pnum, int dir)
{
	int px, py;
	BOOL isOk;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrDirOK: illegal player %d", pnum);
	}

	px = Players[pnum]._px + offset_x[dir];
	py = Players[pnum]._py + offset_y[dir];

	if (px < 0 || !dPiece[px][py] || !PosOkPlayer(pnum, px, py)) {
		return FALSE;
	}

	isOk = TRUE;
	if (dir == DIR_E) {
		isOk = !SolidLoc(px, py + 1) && !(dFlags[px][py + 1] & BFLAG_PLAYERLR);
	}

	if (isOk && dir == DIR_W) {
		isOk = !SolidLoc(px + 1, py) && !(dFlags[px + 1][py] & BFLAG_PLAYERLR);
	}

	return isOk;
}

void PlrClrTrans(int x, int y)
{
	int i, j;

	for (i = y - 1; i <= y + 1; i++) {
		for (j = x - 1; j <= x + 1; j++) {
			TransList[dTransVal[j][i]] = FALSE;
		}
	}
}

void PlrDoTrans(int x, int y)
{
	int i, j;

	if (leveltype != DTYPE_CATHEDRAL && leveltype != DTYPE_CATACOMBS) {
		TransList[1] = TRUE;
	} else {
		for (i = y - 1; i <= y + 1; i++) {
			for (j = x - 1; j <= x + 1; j++) {
				if (!nSolidTable[dPiece[j][i]] && dTransVal[j][i]) {
					TransList[dTransVal[j][i]] = TRUE;
				}
			}
		}
	}
}

void SetPlayerOld(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlayerOld: illegal player %d", pnum);
	}

	Players[pnum]._poldx = Players[pnum]._px;
	Players[pnum]._poldy = Players[pnum]._py;
}

void FixPlayerLocation(int pnum, int bDir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FixPlayerLocation: illegal player %d", pnum);
	}

	Players[pnum]._pfutx = Players[pnum]._px;
	Players[pnum]._pfuty = Players[pnum]._py;
	Players[pnum]._ptargx = Players[pnum]._px;
	Players[pnum]._ptargy = Players[pnum]._py;
	Players[pnum]._pxoff = 0;
	Players[pnum]._pyoff = 0;
	CheckEFlag(pnum, FALSE);
	Players[pnum]._pdir = bDir;
	if (pnum == myplr) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SDIR_NONE;
		ViewX = Players[pnum]._px;
		ViewY = Players[pnum]._py;
	}
}

void StartStand(int pnum, int dir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartStand: illegal player %d", pnum);
	}

	if (!Players[pnum]._pInvincible || Players[pnum]._pHitPoints != 0 || pnum != myplr) {
		if (!(Players[pnum]._pGFXLoad & PFILE_STAND)) {
			LoadPlrGFX(pnum, PFILE_STAND);
		}

		NewPlrAnim(pnum, Players[pnum]._pNAnim[dir], Players[pnum]._pNFrames, 3, Players[pnum]._pNWidth);
		Players[pnum]._pmode = PM_STAND;
		FixPlayerLocation(pnum, dir);
		FixPlrWalkTags(pnum);
		dPlayer[Players[pnum]._px][Players[pnum]._py] = pnum + 1;
		SetPlayerOld(pnum);
	} else {
		SyncPlrKill(pnum, -1);
	}
}

void StartWalkStand(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalkStand: illegal player %d", pnum);
	}

	Players[pnum]._pmode = PM_STAND;
	Players[pnum]._pfutx = Players[pnum]._px;
	Players[pnum]._pfuty = Players[pnum]._py;
	Players[pnum]._pxoff = 0;
	Players[pnum]._pyoff = 0;

	CheckEFlag(pnum, FALSE);

	if (pnum == myplr) {
		ScrollInfo._sxoff = 0;
		ScrollInfo._syoff = 0;
		ScrollInfo._sdir = SDIR_NONE;
		ViewX = Players[pnum]._px;
		ViewY = Players[pnum]._py;
	}
}

void PM_ChangeLightOff(int pnum)
{
	int x, y;
	int xmul, ymul;
	int lx, ly;
	int offx, offy;
	const LightListStruct *l;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_ChangeLightOff: illegal player %d", pnum);
	}

	l = &LightList[Players[pnum]._plid];
	x = 2 * Players[pnum]._pyoff + Players[pnum]._pxoff;
	y = 2 * Players[pnum]._pyoff - Players[pnum]._pxoff;
	if (x < 0) {
		xmul = -1;
		x = -x;
	} else {
		xmul = 1;
	}
	if (y < 0) {
		ymul = -1;
		y = -y;
	} else {
		ymul = 1;
	}

	x = (x >> 3) * xmul;
	y = (y >> 3) * ymul;
	lx = x + (l->_lx << 3);
	ly = y + (l->_ly << 3);
	offx = l->_xoff + (l->_lx << 3);
	offy = l->_yoff + (l->_ly << 3);

	if (abs(lx - offx) < 3 && abs(ly - offy) < 3)
		return;

	ChangeLightOff(Players[pnum]._plid, x, y);
}

void PM_ChangeOffset(int pnum)
{
	int px, py;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_ChangeOffset: illegal player %d", pnum);
	}

	Players[pnum]._pVar8++;
	px = Players[pnum]._pVar6 >> 8;
	py = Players[pnum]._pVar7 >> 8;

	Players[pnum]._pVar6 += Players[pnum]._pxvel;
	Players[pnum]._pVar7 += Players[pnum]._pyvel;

#ifdef HELLFIRE
	if (currlevel == 0 && jogging_opt) {
		Players[pnum]._pVar6 += Players[pnum]._pxvel;
		Players[pnum]._pVar7 += Players[pnum]._pyvel;
	}
#endif

	Players[pnum]._pxoff = Players[pnum]._pVar6 >> 8;
	Players[pnum]._pyoff = Players[pnum]._pVar7 >> 8;

	px -= Players[pnum]._pVar6 >> 8;
	py -= Players[pnum]._pVar7 >> 8;

	if (pnum == myplr && ScrollInfo._sdir) {
		ScrollInfo._sxoff += px;
		ScrollInfo._syoff += py;
	}

	PM_ChangeLightOff(pnum);
}

/**
 * @brief Starting a move action towards NW, N, or NE
 */
void StartWalk(int pnum, int xvel, int yvel, int xadd, int yadd, int EndDir, int sdir)
{
	int px, py;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalk: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);

	px = xadd + Players[pnum]._px;
	py = yadd + Players[pnum]._py;

	if (!PlrDirOK(pnum, EndDir)) {
		return;
	}

	Players[pnum]._pfutx = px;
	Players[pnum]._pfuty = py;

	if (pnum == myplr) {
		ScrollInfo._sdx = Players[pnum]._px - ViewX;
		ScrollInfo._sdy = Players[pnum]._py - ViewY;
	}

	dPlayer[px][py] = -(pnum + 1);
	Players[pnum]._pmode = PM_WALK;
	Players[pnum]._pxvel = xvel;
	Players[pnum]._pyvel = yvel;
	Players[pnum]._pxoff = 0;
	Players[pnum]._pyoff = 0;
	Players[pnum]._pVar1 = xadd;
	Players[pnum]._pVar2 = yadd;
	Players[pnum]._pVar3 = EndDir;

	if (!(Players[pnum]._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}

	NewPlrAnim(pnum, Players[pnum]._pWAnim[EndDir], Players[pnum]._pWFrames, 0, Players[pnum]._pWWidth);

	Players[pnum]._pdir = EndDir;
	Players[pnum]._pVar6 = 0;
	Players[pnum]._pVar7 = 0;
	Players[pnum]._pVar8 = 0;

	CheckEFlag(pnum, FALSE);

	if (pnum != myplr) {
		return;
	}

	if (zoomflag) {
		if (abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

/**
 * @brief Starting a move action towards SW, S, or SE
 */
void StartWalk2(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir, int sdir)
{
	int px, py;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalk2: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);
	px = xadd + Players[pnum]._px;
	py = yadd + Players[pnum]._py;

	if (!PlrDirOK(pnum, EndDir)) {
		return;
	}

	Players[pnum]._pfutx = px;
	Players[pnum]._pfuty = py;

	if (pnum == myplr) {
		ScrollInfo._sdx = Players[pnum]._px - ViewX;
		ScrollInfo._sdy = Players[pnum]._py - ViewY;
	}

	dPlayer[Players[pnum]._px][Players[pnum]._py] = -1 - pnum;
	Players[pnum]._pVar1 = Players[pnum]._px;
	Players[pnum]._pVar2 = Players[pnum]._py;
	Players[pnum]._px = px; // Move player to the next tile to maintain correct render order
	Players[pnum]._py = py;
	dPlayer[Players[pnum]._px][Players[pnum]._py] = pnum + 1;
	Players[pnum]._pxoff = xoff; // Offset player sprite to align with their previous tile position
	Players[pnum]._pyoff = yoff;

	// BUGFIX: missing `if (leveltype != DTYPE_TOWN) {` for call to ChangeLightXY and PM_ChangeLightOff.
	ChangeLightXY(Players[pnum]._plid, Players[pnum]._px, Players[pnum]._py);
	PM_ChangeLightOff(pnum);

	Players[pnum]._pmode = PM_WALK2;
	Players[pnum]._pxvel = xvel;
	Players[pnum]._pyvel = yvel;
	Players[pnum]._pVar6 = xoff << 8;
	Players[pnum]._pVar7 = yoff << 8;
	Players[pnum]._pVar3 = EndDir;

	if (!(Players[pnum]._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}
	NewPlrAnim(pnum, Players[pnum]._pWAnim[EndDir], Players[pnum]._pWFrames, 0, Players[pnum]._pWWidth);

	Players[pnum]._pdir = EndDir;
	Players[pnum]._pVar8 = 0;

	if (EndDir == DIR_SE) {
		CheckEFlag(pnum, TRUE);
	} else {
		CheckEFlag(pnum, FALSE);
	}

	if (pnum != myplr) {
		return;
	}

	if (zoomflag) {
		if (abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

/**
 * @brief Starting a move action towards W or E
 */
void StartWalk3(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir, int sdir)
{
	int px, py, x, y;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalk3: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(pnum);
	px = xadd + Players[pnum]._px;
	py = yadd + Players[pnum]._py;
	x = mapx + Players[pnum]._px;
	y = mapy + Players[pnum]._py;

	if (!PlrDirOK(pnum, EndDir)) {
		return;
	}

	Players[pnum]._pfutx = px;
	Players[pnum]._pfuty = py;

	if (pnum == myplr) {
		ScrollInfo._sdx = Players[pnum]._px - ViewX;
		ScrollInfo._sdy = Players[pnum]._py - ViewY;
	}

	dPlayer[Players[pnum]._px][Players[pnum]._py] = -1 - pnum;
	dPlayer[px][py] = -1 - pnum;
	Players[pnum]._pVar4 = x;
	Players[pnum]._pVar5 = y;
	dFlags[x][y] |= BFLAG_PLAYERLR;
	Players[pnum]._pxoff = xoff; // Offset player sprite to align with their previous tile position
	Players[pnum]._pyoff = yoff;

	if (leveltype != DTYPE_TOWN) {
		ChangeLightXY(Players[pnum]._plid, x, y);
		PM_ChangeLightOff(pnum);
	}

	Players[pnum]._pmode = PM_WALK3;
	Players[pnum]._pxvel = xvel;
	Players[pnum]._pyvel = yvel;
	Players[pnum]._pVar1 = px;
	Players[pnum]._pVar2 = py;
	Players[pnum]._pVar6 = xoff << 8;
	Players[pnum]._pVar7 = yoff << 8;
	Players[pnum]._pVar3 = EndDir;

	if (!(Players[pnum]._pGFXLoad & PFILE_WALK)) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}
	NewPlrAnim(pnum, Players[pnum]._pWAnim[EndDir], Players[pnum]._pWFrames, 0, Players[pnum]._pWWidth);

	Players[pnum]._pdir = EndDir;
	Players[pnum]._pVar8 = 0;

	CheckEFlag(pnum, FALSE);

	if (pnum != myplr) {
		return;
	}

	if (zoomflag) {
		if (abs(ScrollInfo._sdx) >= 3 || abs(ScrollInfo._sdy) >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (abs(ScrollInfo._sdx) >= 2 || abs(ScrollInfo._sdy) >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

void StartAttack(int pnum, int d)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartAttack: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (!(Players[pnum]._pGFXLoad & PFILE_ATTACK)) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}

	NewPlrAnim(pnum, Players[pnum]._pAAnim[d], Players[pnum]._pAFrames, 0, Players[pnum]._pAWidth);
	Players[pnum]._pmode = PM_ATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);
}

void StartRangeAttack(int pnum, int d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartRangeAttack: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (!(Players[pnum]._pGFXLoad & PFILE_ATTACK)) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}
	NewPlrAnim(pnum, Players[pnum]._pAAnim[d], Players[pnum]._pAFrames, 0, Players[pnum]._pAWidth);

	Players[pnum]._pmode = PM_RATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);
	Players[pnum]._pVar1 = cx;
	Players[pnum]._pVar2 = cy;
}

void StartPlrBlock(int pnum, int dir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlrBlock: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	PlaySfxLoc(IS_ISWORD, Players[pnum]._px, Players[pnum]._py);

	if (!(Players[pnum]._pGFXLoad & PFILE_BLOCK)) {
		LoadPlrGFX(pnum, PFILE_BLOCK);
	}
	NewPlrAnim(pnum, Players[pnum]._pBAnim[dir], Players[pnum]._pBFrames, 2, Players[pnum]._pBWidth);

	Players[pnum]._pmode = PM_BLOCK;
	FixPlayerLocation(pnum, dir);
	SetPlayerOld(pnum);
}

void StartSpell(int pnum, int d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartSpell: illegal player %d", pnum);

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (leveltype != DTYPE_TOWN) {
		switch (spelldata[Players[pnum]._pSpell].sType) {
		case STYPE_FIRE:
			if (!(Players[pnum]._pGFXLoad & PFILE_FIRE)) {
				LoadPlrGFX(pnum, PFILE_FIRE);
			}
			NewPlrAnim(pnum, Players[pnum]._pFAnim[d], Players[pnum]._pSFrames, 0, Players[pnum]._pSWidth);
			break;
		case STYPE_LIGHTNING:
			if (!(Players[pnum]._pGFXLoad & PFILE_LIGHTNING)) {
				LoadPlrGFX(pnum, PFILE_LIGHTNING);
			}
			NewPlrAnim(pnum, Players[pnum]._pLAnim[d], Players[pnum]._pSFrames, 0, Players[pnum]._pSWidth);
			break;
		case STYPE_MAGIC:
			if (!(Players[pnum]._pGFXLoad & PFILE_MAGIC)) {
				LoadPlrGFX(pnum, PFILE_MAGIC);
			}
			NewPlrAnim(pnum, Players[pnum]._pTAnim[d], Players[pnum]._pSFrames, 0, Players[pnum]._pSWidth);
			break;
		}
	}

	PlaySfxLoc(spelldata[Players[pnum]._pSpell].sSFX, Players[pnum]._px, Players[pnum]._py);

	Players[pnum]._pmode = PM_SPELL;

	FixPlayerLocation(pnum, d);
	SetPlayerOld(pnum);

	Players[pnum]._pVar1 = cx;
	Players[pnum]._pVar2 = cy;
	Players[pnum]._pVar4 = GetSpellLevel(pnum, Players[pnum]._pSpell);
	Players[pnum]._pVar8 = 1;
}

void FixPlrWalkTags(int pnum)
{
	int pp, pn;
	int dx, dy, y, x;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FixPlrWalkTags: illegal player %d", pnum);
	}

	pp = pnum + 1;
	pn = -(pnum + 1);
	dx = Players[pnum]._poldx;
	dy = Players[pnum]._poldy;
	for (y = dy - 1; y <= dy + 1; y++) {
		for (x = dx - 1; x <= dx + 1; x++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && (dPlayer[x][y] == pp || dPlayer[x][y] == pn)) {
				dPlayer[x][y] = 0;
			}
		}
	}

	if (dx >= 0 && dx < MAXDUNX - 1 && dy >= 0 && dy < MAXDUNY - 1) {
		dFlags[dx + 1][dy] &= ~BFLAG_PLAYERLR;
		dFlags[dx][dy + 1] &= ~BFLAG_PLAYERLR;
	}
}

void RemovePlrFromMap(int pnum)
{
	int x, y;
	int pp, pn;

	pp = pnum + 1;
	pn = -(pnum + 1);

	for (y = 1; y < MAXDUNY; y++)
		for (x = 1; x < MAXDUNX; x++)
			if (dPlayer[x][y - 1] == pn || dPlayer[x - 1][y] == pn)
				if (dFlags[x][y] & BFLAG_PLAYERLR)
					dFlags[x][y] &= ~BFLAG_PLAYERLR;

	for (y = 0; y < MAXDUNY; y++)
		for (x = 0; x < MAXDUNX; x++)
			if (dPlayer[x][y] == pp || dPlayer[x][y] == pn)
				dPlayer[x][y] = 0;
}

void StartPlrHit(int pnum, int dam, BOOL forcehit)
{
	int pd;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlrHit: illegal player %d", pnum);
	}

	if (Players[pnum]._pInvincible && Players[pnum]._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (Players[pnum]._pClass == PC_WARRIOR) {
		PlaySfxLoc(PS_WARR69, Players[pnum]._px, Players[pnum]._py);
	} else if (Players[pnum]._pClass == PC_ROGUE) {
		PlaySfxLoc(PS_ROGUE69, Players[pnum]._px, Players[pnum]._py);
	} else if (Players[pnum]._pClass == PC_SORCERER) {
		PlaySfxLoc(PS_MAGE69, Players[pnum]._px, Players[pnum]._py);
	}

	drawhpflag = TRUE;
	if (dam >> 6 < Players[pnum]._pLevel && !forcehit) {
		return;
	}

	pd = Players[pnum]._pdir;

	if (!(Players[pnum]._pGFXLoad & PFILE_HIT)) {
		LoadPlrGFX(pnum, PFILE_HIT);
	}
	NewPlrAnim(pnum, Players[pnum]._pHAnim[pd], Players[pnum]._pHFrames, 0, Players[pnum]._pHWidth);

	Players[pnum]._pmode = PM_GOTHIT;
	FixPlayerLocation(pnum, pd);
	Players[pnum]._pVar8 = 1;
	FixPlrWalkTags(pnum);
	dPlayer[Players[pnum]._px][Players[pnum]._py] = pnum + 1;
	SetPlayerOld(pnum);
}

void RespawnDeadItem(Item *itm, int x, int y)
{
	int ii;

	if (numitems >= MAXITEMS) {
		return;
	}

	if (FindGetItem(itm->IDidx, itm->_iCreateInfo, itm->_iSeed) >= 0) {
		DrawInvMsg("A duplicate item has been detected.  Destroying duplicate...");
		SyncGetItem(x, y, itm->IDidx, itm->_iCreateInfo, itm->_iSeed);
	}

	ii = itemavail[0];
	dItem[x][y] = ii + 1;
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;
	GroundItems[ii] = *itm;
	GroundItems[ii]._ix = x;
	GroundItems[ii]._iy = y;
	RespawnItem(ii, TRUE);
	numitems++;
	itm->_itype = ITYPE_NONE;
}

static void PlrDeadItem(int pnum, Item *itm, int xx, int yy)
{
	int x, y;
	int i, j, k;

	if (itm->_itype == ITYPE_NONE)
		return;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrDeadItem: illegal player %d", pnum);
	}

	x = xx + Players[pnum]._px;
	y = yy + Players[pnum]._py;
	if ((xx || yy) && ItemSpaceOk(x, y)) {
		RespawnDeadItem(itm, x, y);
		Players[pnum].HoldItem = *itm;
		NetSendCmdPItem(FALSE, CMD_RESPAWNITEM, x, y);
		return;
	}

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			y = j + Players[pnum]._py;
			for (i = -k; i <= k; i++) {
				x = i + Players[pnum]._px;
				if (ItemSpaceOk(x, y)) {
					RespawnDeadItem(itm, x, y);
					Players[pnum].HoldItem = *itm;
					NetSendCmdPItem(FALSE, CMD_RESPAWNITEM, x, y);
					return;
				}
			}
		}
	}
}

void StartPlayerKill(int pnum, int earflag)
{
	BOOL diablolevel;
	int i, pdd;
	Player *p;
	Item ear;
	Item *pi;

	p = &Players[pnum];
	if (p->_pHitPoints <= 0 && p->_pmode == PM_DEATH) {
		return;
	}

	if (myplr == pnum) {
		NetSendCmdParam1(TRUE, CMD_PLRDEAD, earflag);
	}

	diablolevel = gbMaxPlayers > 1 && Players[pnum].plrlevel == 16;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlayerKill: illegal player %d", pnum);
	}

	if (Players[pnum]._pClass == PC_WARRIOR) {
		PlaySfxLoc(PS_DEAD, p->_px, p->_py); // BUGFIX: should use `PS_WARR71` like other classes
#ifndef SPAWN
	} else if (Players[pnum]._pClass == PC_ROGUE) {
		PlaySfxLoc(PS_ROGUE71, p->_px, p->_py);
	} else if (Players[pnum]._pClass == PC_SORCERER) {
		PlaySfxLoc(PS_MAGE71, p->_px, p->_py);
#ifdef HELLFIRE
	} else if (Players[pnum]._pClass == PC_MONK) {
		PlaySfxLoc(PS_MONK71, p->_px, p->_py);
	} else if (Players[pnum]._pClass == PC_BARD) {
		PlaySfxLoc(PS_ROGUE71, p->_px, p->_py);
	} else if (Players[pnum]._pClass == PC_BARBARIAN) {
		PlaySfxLoc(PS_WARR71, p->_px, p->_py);
#endif
#endif
	}

	if (p->_pgfxnum) {
		p->_pgfxnum = 0;
		p->_pGFXLoad = 0;
		SetPlrAnims(pnum);
	}

	if (!(p->_pGFXLoad & PFILE_DEATH)) {
		LoadPlrGFX(pnum, PFILE_DEATH);
	}

	NewPlrAnim(pnum, p->_pDAnim[p->_pdir], p->_pDFrames, 1, p->_pDWidth);

	p->_pBlockFlag = FALSE;
	p->_pmode = PM_DEATH;
	p->_pInvincible = TRUE;
	SetPlayerHitPoints(pnum, 0);
	p->_pVar8 = 1;

	if (pnum != myplr && !earflag && !diablolevel) {
		for (i = 0; i < NUM_INVLOC; i++) {
			p->InvBody[i]._itype = ITYPE_NONE;
		}
		CalcPlrInv(pnum, FALSE);
	}

	if (Players[pnum].plrlevel == currlevel) {
		FixPlayerLocation(pnum, p->_pdir);
		RemovePlrFromMap(pnum);
		dFlags[p->_px][p->_py] |= BFLAG_DEAD_PLAYER;
		SetPlayerOld(pnum);

		if (pnum == myplr) {
			drawhpflag = TRUE;
			deathdelay = 30;

			if (pcurs >= CURSOR_FIRSTITEM) {
				PlrDeadItem(pnum, &p->HoldItem, 0, 0);
				SetCursor_(CURSOR_HAND);
			}

			if (!diablolevel) {
				DropHalfPlayersGold(pnum);
				if (earflag != -1) {
					if (earflag != 0) {
						SetPlrHandItem(&ear, IDI_EAR);
						sprintf(ear._iName, "Ear of %s", Players[pnum]._pName);
						if (Players[pnum]._pClass == PC_SORCERER) {
							ear._iCurs = ICURS_EAR_SORCEROR;
						} else if (Players[pnum]._pClass == PC_WARRIOR) {
							ear._iCurs = ICURS_EAR_WARRIOR;
						} else if (Players[pnum]._pClass == PC_ROGUE) {
							ear._iCurs = ICURS_EAR_ROGUE;
#ifdef HELLFIRE
						} else if (Players[pnum]._pClass == PC_MONK || Players[pnum]._pClass == PC_BARD || Players[pnum]._pClass == PC_BARBARIAN) {
							ear._iCurs = ICURS_EAR_ROGUE;
#endif
						}

						ear._iCreateInfo = Players[pnum]._pName[0] << 8 | Players[pnum]._pName[1];
						ear._iSeed = Players[pnum]._pName[2] << 24 | Players[pnum]._pName[3] << 16 | Players[pnum]._pName[4] << 8 | Players[pnum]._pName[5];
						ear._ivalue = Players[pnum]._pLevel;

						if (FindGetItem(IDI_EAR, ear._iCreateInfo, ear._iSeed) == -1) {
							PlrDeadItem(pnum, &ear, 0, 0);
						}
					} else {
						pi = &p->InvBody[0];
						i = NUM_INVLOC;
						while (i--) {
							pdd = (i + p->_pdir) & 7;
							PlrDeadItem(pnum, pi, offset_x[pdd], offset_y[pdd]);
							pi++;
						}

						CalcPlrInv(pnum, FALSE);
					}
				}
			}
		}
	}
#ifndef HELLFIRE
	SetPlayerHitPoints(pnum, 0);
#endif
}

void DropHalfPlayersGold(int pnum)
{
	int i, hGold;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("DropHalfPlayersGold: illegal player %d", pnum);
	}

	hGold = Players[pnum]._pGold >> 1;
	for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
		if (Players[pnum].SpdList[i]._itype == ITYPE_GOLD &&
#ifndef HELLFIRE
		    Players[pnum].SpdList[i]._ivalue != GOLD_MAX_LIMIT) {
#else
		    Players[pnum].SpdList[i]._ivalue != MaxGold) {
#endif
			if (hGold < Players[pnum].SpdList[i]._ivalue) {
				Players[pnum].SpdList[i]._ivalue -= hGold;
				SetSpdbarGoldCurs(pnum, i);
				SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &Players[pnum].HoldItem);
				SetPlrHandGoldCurs(&Players[pnum].HoldItem);
				Players[pnum].HoldItem._ivalue = hGold;
				PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
				hGold = 0;
			} else {
				hGold -= Players[pnum].SpdList[i]._ivalue;
				RemoveSpdBarItem(pnum, i);
				SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &Players[pnum].HoldItem);
				SetPlrHandGoldCurs(&Players[pnum].HoldItem);
				Players[pnum].HoldItem._ivalue = Players[pnum].SpdList[i]._ivalue;
				PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
				i = -1;
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
			if (Players[pnum].SpdList[i]._itype == ITYPE_GOLD) {
				if (hGold < Players[pnum].SpdList[i]._ivalue) {
					Players[pnum].SpdList[i]._ivalue -= hGold;
					SetSpdbarGoldCurs(pnum, i);
					SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &Players[pnum].HoldItem);
					SetPlrHandGoldCurs(&Players[pnum].HoldItem);
					Players[pnum].HoldItem._ivalue = hGold;
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= Players[pnum].SpdList[i]._ivalue;
					RemoveSpdBarItem(pnum, i);
					SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &Players[pnum].HoldItem);
					SetPlrHandGoldCurs(&Players[pnum].HoldItem);
					Players[pnum].HoldItem._ivalue = Players[pnum].SpdList[i]._ivalue;
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	force_redraw = 255;
	if (hGold > 0) {
		for (i = 0; i < Players[pnum]._pNumInv && hGold > 0; i++) {
			if (Players[pnum].InvList[i]._itype == ITYPE_GOLD &&
#ifndef HELLFIRE
			    Players[pnum].InvList[i]._ivalue != GOLD_MAX_LIMIT) {
#else
			    Players[pnum].InvList[i]._ivalue != MaxGold) {
#endif
				if (hGold < Players[pnum].InvList[i]._ivalue) {
					Players[pnum].InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &Players[pnum].HoldItem);
					SetPlrHandGoldCurs(&Players[pnum].HoldItem);
					Players[pnum].HoldItem._ivalue = hGold;
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= Players[pnum].InvList[i]._ivalue;
					RemoveInvItem(pnum, i);
					SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &Players[pnum].HoldItem);
					SetPlrHandGoldCurs(&Players[pnum].HoldItem);
					Players[pnum].HoldItem._ivalue = Players[pnum].InvList[i]._ivalue;
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < Players[pnum]._pNumInv && hGold > 0; i++) {
			if (Players[pnum].InvList[i]._itype == ITYPE_GOLD) {
				if (hGold < Players[pnum].InvList[i]._ivalue) {
					Players[pnum].InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &Players[pnum].HoldItem);
					SetPlrHandGoldCurs(&Players[pnum].HoldItem);
					Players[pnum].HoldItem._ivalue = hGold;
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= Players[pnum].InvList[i]._ivalue;
					RemoveInvItem(pnum, i);
					SetPlrHandItem(&Players[pnum].HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &Players[pnum].HoldItem);
					SetPlrHandGoldCurs(&Players[pnum].HoldItem);
					Players[pnum].HoldItem._ivalue = Players[pnum].InvList[i]._ivalue;
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	Players[pnum]._pGold = CalculateGold(pnum);
}

#ifdef HELLFIRE
void StripTopGold(int pnum)
{
	Item tmpItem;
	int i, val;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StripTopGold: illegal player %d", pnum);
	}
	tmpItem = Players[pnum].HoldItem;

	for (i = 0; i < Players[pnum]._pNumInv; i++) {
		if (Players[pnum].InvList[i]._itype == ITYPE_GOLD) {
			if (Players[pnum].InvList[i]._ivalue > MaxGold) {
				val = Players[pnum].InvList[i]._ivalue - MaxGold;
				Players[pnum].InvList[i]._ivalue = MaxGold;
				SetGoldCurs(pnum, i);
				SetPlrHandItem(&Players[pnum].HoldItem, 0);
				GetGoldSeed(pnum, &Players[pnum].HoldItem);
				SetPlrHandGoldCurs(&Players[pnum].HoldItem);
				Players[pnum].HoldItem._ivalue = val;
				if (!GoldAutoPlace(pnum))
					PlrDeadItem(pnum, &Players[pnum].HoldItem, 0, 0);
			}
		}
	}
	Players[pnum]._pGold = CalculateGold(pnum);
	Players[pnum].HoldItem = tmpItem;
}

#endif
void SyncPlrKill(int pnum, int earflag)
{
	int ma, i;

#ifdef HELLFIRE
	if (Players[pnum]._pHitPoints <= 0 && currlevel == 0) {
#else
	if (Players[pnum]._pHitPoints == 0 && currlevel == 0) {
#endif
		SetPlayerHitPoints(pnum, 64);
		return;
	}

	for (i = 0; i < nummissiles; i++) {
		ma = missileactive[i];
		if (missile[ma]._mitype == MIS_MANASHIELD && missile[ma]._misource == pnum && missile[ma]._miDelFlag == FALSE) {
			if (earflag != -1) {
				missile[ma]._miVar8 = earflag;
			}

			return;
		}
	}

	SetPlayerHitPoints(pnum, 0);
	StartPlayerKill(pnum, earflag);
}

void RemovePlrMissiles(int pnum)
{
	int i, am;
	int mx, my;

	if (currlevel != 0 && pnum == myplr && (monster[myplr]._mx != 1 || monster[myplr]._my != 0)) {
		M_StartKill(myplr, myplr);
		AddDead(monster[myplr]._mx, monster[myplr]._my, (monster[myplr].MType)->mdeadval, monster[myplr]._mdir);
		mx = monster[myplr]._mx;
		my = monster[myplr]._my;
		dMonster[mx][my] = 0;
		monster[myplr]._mDelFlag = TRUE;
		DeleteMonsterList();
	}

	for (i = 0; i < nummissiles; i++) {
		am = missileactive[i];
		if (missile[am]._mitype == MIS_STONE && missile[am]._misource == pnum) {
			monster[missile[am]._miVar2]._mmode = missile[am]._miVar1;
		}
		if (missile[am]._mitype == MIS_MANASHIELD && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
		if (missile[am]._mitype == MIS_ETHEREALIZE && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
	}
}

void InitLevelChange(int pnum)
{
	RemovePlrMissiles(pnum);
	if (pnum == myplr && qtextflag) {
		qtextflag = FALSE;
		stream_stop();
	}

	RemovePlrFromMap(pnum);
	SetPlayerOld(pnum);
	if (pnum == myplr) {
		dPlayer[Players[myplr]._px][Players[myplr]._py] = myplr + 1;
	} else {
		Players[pnum]._pLvlVisited[Players[pnum].plrlevel] = TRUE;
	}

	ClrPlrPath(pnum);
	Players[pnum].destAction = ACTION_NONE;
	Players[pnum]._pLvlChanging = TRUE;

	if (pnum == myplr) {
		Players[pnum].pLvlLoad = 10;
	}
}

void StartNewLvl(int pnum, int fom, int lvl)
{
	InitLevelChange(pnum);

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartNewLvl: illegal player %d", pnum);
	}

	switch (fom) {
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
		Players[pnum].plrlevel = lvl;
		break;
	case WM_DIABRTNLVL:
	case WM_DIABTOWNWARP:
		Players[pnum].plrlevel = lvl;
		break;
	case WM_DIABSETLVL:
		setlvlnum = lvl;
		break;
	case WM_DIABTWARPUP:
		Players[myplr].pTownWarps |= 1 << (leveltype - 2);
		Players[pnum].plrlevel = lvl;
		break;
	case WM_DIABRETOWN:
		break;
	default:
		app_fatal("StartNewLvl");
		break;
	}

	if (pnum == myplr) {
		Players[pnum]._pmode = PM_NEWLVL;
		Players[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd, fom, 0, 0);
		if (gbMaxPlayers > 1) {
			NetSendCmdParam2(TRUE, CMD_NEWLVL, fom, lvl);
		}
	}
}
void RestartTownLvl(int pnum)
{
	InitLevelChange(pnum);
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("RestartTownLvl: illegal player %d", pnum);
	}

	Players[pnum].plrlevel = 0;
	Players[pnum]._pInvincible = FALSE;

	SetPlayerHitPoints(pnum, 64);

	Players[pnum]._pMana = 0;
	Players[pnum]._pManaBase = Players[pnum]._pMana - (Players[pnum]._pMaxMana - Players[pnum]._pMaxManaBase);

	CalcPlrInv(pnum, FALSE);

	if (pnum == myplr) {
		Players[pnum]._pmode = PM_NEWLVL;
		Players[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd, WM_DIABRETOWN, 0, 0);
	}
}

void StartWarpLvl(int pnum, int pidx)
{
	InitLevelChange(pnum);

	if (gbMaxPlayers != 1) {
		if (Players[pnum].plrlevel != 0) {
			Players[pnum].plrlevel = 0;
		} else {
			Players[pnum].plrlevel = portal[pidx].level;
		}
	}

	if (pnum == myplr) {
		SetCurrentPortal(pidx);
		Players[pnum]._pmode = PM_NEWLVL;
		Players[pnum]._pInvincible = TRUE;
		PostMessage(ghMainWnd, WM_DIABWARPLVL, 0, 0);
	}
}

BOOL PM_DoStand(int pnum)
{
	return FALSE;
}

/**
 * @brief Movement towards NW, N, and NE
 */
BOOL PM_DoWalk(int pnum)
{
	int anim_len;
	BOOL rv;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoWalk: illegal player %d", pnum);
	}

#ifndef HELLFIRE
	if (Players[pnum]._pAnimFrame == 3
	    || (Players[pnum]._pWFrames == 8 && Players[pnum]._pAnimFrame == 7)
	    || (Players[pnum]._pWFrames != 8 && Players[pnum]._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, Players[pnum]._px, Players[pnum]._py);
	}
#else
	if (currlevel == 0 && jogging_opt) {
		if (Players[pnum]._pAnimFrame % 2 == 0) {
			Players[pnum]._pAnimFrame++;
			Players[pnum]._pVar8++;
		}
		if (Players[pnum]._pAnimFrame >= Players[pnum]._pWFrames) {
			Players[pnum]._pAnimFrame = 0;
		}
	}
#endif

	anim_len = 8;
	if (currlevel != 0) {
		anim_len = AnimLenFromClass[Players[pnum]._pClass];
	}

#ifndef HELLFIRE
	if (Players[pnum]._pVar8 == anim_len) {
#else
	if (Players[pnum]._pVar8 >= anim_len) {
#endif
		dPlayer[Players[pnum]._px][Players[pnum]._py] = 0;
		Players[pnum]._px += Players[pnum]._pVar1;
		Players[pnum]._py += Players[pnum]._pVar2;
		dPlayer[Players[pnum]._px][Players[pnum]._py] = pnum + 1;

		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(Players[pnum]._plid, Players[pnum]._px, Players[pnum]._py);
			ChangeVisionXY(Players[pnum]._pvid, Players[pnum]._px, Players[pnum]._py);
		}

		if (pnum == myplr && ScrollInfo._sdir) {
			ViewX = Players[pnum]._px - ScrollInfo._sdx;
			ViewY = Players[pnum]._py - ScrollInfo._sdy;
		}

		if (Players[pnum].walkpath[0] != WALK_NONE) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, Players[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);

		if (leveltype != DTYPE_TOWN) {
			ChangeLightOff(Players[pnum]._plid, 0, 0);
		}
		rv = TRUE;
	} else {
		PM_ChangeOffset(pnum);
		rv = FALSE;
	}

	return rv;
}

/**
 * @brief Movement towards SW, S, and SE
 */
BOOL PM_DoWalk2(int pnum)
{
	int anim_len;
	BOOL rv;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoWalk2: illegal player %d", pnum);
	}

#ifndef HELLFIRE
	if (Players[pnum]._pAnimFrame == 3
	    || (Players[pnum]._pWFrames == 8 && Players[pnum]._pAnimFrame == 7)
	    || (Players[pnum]._pWFrames != 8 && Players[pnum]._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, Players[pnum]._px, Players[pnum]._py);
	}
#else
	if (currlevel == 0 && jogging_opt) {
		if (Players[pnum]._pAnimFrame % 2 == 0) {
			Players[pnum]._pAnimFrame++;
			Players[pnum]._pVar8++;
		}
		if (Players[pnum]._pAnimFrame >= Players[pnum]._pWFrames) {
			Players[pnum]._pAnimFrame = 0;
		}
	}
#endif

	anim_len = 8;
	if (currlevel != 0) {
		anim_len = AnimLenFromClass[Players[pnum]._pClass];
	}

#ifndef HELLFIRE
	if (Players[pnum]._pVar8 == anim_len) {
#else
	if (Players[pnum]._pVar8 >= anim_len) {
#endif
		dPlayer[Players[pnum]._pVar1][Players[pnum]._pVar2] = 0;

		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(Players[pnum]._plid, Players[pnum]._px, Players[pnum]._py);
			ChangeVisionXY(Players[pnum]._pvid, Players[pnum]._px, Players[pnum]._py);
		}

		if (pnum == myplr && ScrollInfo._sdir) {
			ViewX = Players[pnum]._px - ScrollInfo._sdx;
			ViewY = Players[pnum]._py - ScrollInfo._sdy;
		}

		if (Players[pnum].walkpath[0] != WALK_NONE) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, Players[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);
		if (leveltype != DTYPE_TOWN) {
			ChangeLightOff(Players[pnum]._plid, 0, 0);
		}
		rv = TRUE;
	} else {
		PM_ChangeOffset(pnum);
		rv = FALSE;
	}

	return rv;
}

/**
 * @brief Movement towards W and E
 */
BOOL PM_DoWalk3(int pnum)
{
	int anim_len;
	BOOL rv;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoWalk3: illegal player %d", pnum);
	}

#ifndef HELLFIRE
	if (Players[pnum]._pAnimFrame == 3
	    || (Players[pnum]._pWFrames == 8 && Players[pnum]._pAnimFrame == 7)
	    || (Players[pnum]._pWFrames != 8 && Players[pnum]._pAnimFrame == 4)) {
		PlaySfxLoc(PS_WALK1, Players[pnum]._px, Players[pnum]._py);
	}
#else
	if (currlevel == 0 && jogging_opt) {
		if (Players[pnum]._pAnimFrame % 2 == 0) {
			Players[pnum]._pAnimFrame++;
			Players[pnum]._pVar8++;
		}
		if (Players[pnum]._pAnimFrame >= Players[pnum]._pWFrames) {
			Players[pnum]._pAnimFrame = 0;
		}
	}
#endif

	anim_len = 8;
	if (currlevel != 0) {
		anim_len = AnimLenFromClass[Players[pnum]._pClass];
	}

#ifndef HELLFIRE
	if (Players[pnum]._pVar8 == anim_len) {
#else
	if (Players[pnum]._pVar8 >= anim_len) {
#endif
		dPlayer[Players[pnum]._px][Players[pnum]._py] = 0;
		dFlags[Players[pnum]._pVar4][Players[pnum]._pVar5] &= ~BFLAG_PLAYERLR;
		Players[pnum]._px = Players[pnum]._pVar1;
		Players[pnum]._py = Players[pnum]._pVar2;
		dPlayer[Players[pnum]._px][Players[pnum]._py] = pnum + 1;

		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(Players[pnum]._plid, Players[pnum]._px, Players[pnum]._py);
			ChangeVisionXY(Players[pnum]._pvid, Players[pnum]._px, Players[pnum]._py);
		}

		if (pnum == myplr && ScrollInfo._sdir) {
			ViewX = Players[pnum]._px - ScrollInfo._sdx;
			ViewY = Players[pnum]._py - ScrollInfo._sdy;
		}

		if (Players[pnum].walkpath[0] != WALK_NONE) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, Players[pnum]._pVar3);
		}

		ClearPlrPVars(pnum);

		if (leveltype != DTYPE_TOWN) {
			ChangeLightOff(Players[pnum]._plid, 0, 0);
		}
		rv = TRUE;
	} else {
		PM_ChangeOffset(pnum);
		rv = FALSE;
	}

	return rv;
}

BOOL WeaponDur(int pnum, int durrnd)
{
	if (pnum != myplr) {
		return FALSE;
	}

#ifdef HELLFIRE
	if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && Players[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDamAcFlags & ISPLHF_DECAY) {
		Players[pnum].InvBody[INVLOC_HAND_LEFT]._iPLDam -= 5;
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iPLDam <= -100) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
		CalcPlrInv(pnum, TRUE);
	}

	if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDamAcFlags & ISPLHF_DECAY) {
		Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iPLDam -= 5;
		if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iPLDam <= -100) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT); // BUGFIX: INVLOC_HAND_RIGHT
			Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
		CalcPlrInv(pnum, TRUE);
	}

#endif
	if (random_(3, durrnd) != 0) {
		return FALSE;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("WeaponDur: illegal player %d", pnum);
	}

	if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_NONE && Players[pnum].InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON) {
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
#ifdef HELLFIRE
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability <= 0) {
#else
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
#endif
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_NONE && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON) {
		if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_NONE && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
			Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_NONE && Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return FALSE;
		}

		Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PlrHitMonst(int pnum, int m)
{
	BOOL rv, ret;
	int hit, hper, mind, maxd, ddp, dam, skdam, phanditype, tmac;
	hper = 0;
#ifdef HELLFIRE
	ret = FALSE;
	BOOL adjacentDamage = FALSE;
#endif

	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("PlrHitMonst: illegal monster %d", m);
	}

	if ((monster[m]._mhitpoints >> 6) <= 0) {
		return FALSE;
	}

	if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT) {
		return FALSE;
	}

	if (monster[m]._mmode == MM_CHARGE) {
		return FALSE;
	}

#ifdef HELLFIRE
	if (pnum < 0) {
		adjacentDamage = TRUE;
		pnum = -pnum;
		if (Players[pnum]._pLevel > 20)
			hper -= 30;
		else
			hper -= (35 - Players[pnum]._pLevel) * 2;
	}
#endif

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitMonst: illegal player %d", pnum);
	}

	rv = FALSE;

	hit = random_(4, 100);
	if (monster[m]._mmode == MM_STONE) {
		hit = 0;
	}

	tmac = monster[m].mArmorClass;
#ifdef HELLFIRE
	if (Players[pnum]._pIEnAc > 0) {
		int _pIEnAc = Players[pnum]._pIEnAc - 1;
		if (_pIEnAc > 0)
			tmac >>= _pIEnAc;
		else
			tmac -= tmac >> 2;

		if (Players[pnum]._pClass == PC_BARBARIAN) {
			tmac -= monster[m].mArmorClass / 8;
		}

		if (tmac < 0)
			tmac = 0;
	}
#else
	tmac -= Players[pnum]._pIEnAc;
#endif

	hper += (Players[pnum]._pDexterity >> 1) + Players[pnum]._pLevel + 50 - tmac;
	if (Players[pnum]._pClass == PC_WARRIOR) {
		hper += 20;
	}
	hper += Players[pnum]._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if (CheckMonsterHit(m, ret)) {
		return ret;
	}
#ifdef _DEBUG
	if (hit < hper || debug_mode_key_inverted_v || debug_mode_dollar_sign) {
#else
	if (hit < hper) {
#endif
#ifdef HELLFIRE
		if (Players[pnum]._pIFlags & ISPL_FIREDAM && Players[pnum]._pIFlags & ISPL_LIGHTDAM) {
			int midam = Players[pnum]._pIFMinDam + random_(3, Players[pnum]._pIFMaxDam - Players[pnum]._pIFMinDam);
			AddMissile(Players[pnum]._px, Players[pnum]._py, Players[pnum]._pVar1, Players[pnum]._pVar2, Players[pnum]._pdir, MIS_SPECARROW, TARGET_MONSTERS, pnum, midam, 0);
		}
#endif
		mind = Players[pnum]._pIMinDam;
		maxd = Players[pnum]._pIMaxDam;
		dam = random_(5, maxd - mind + 1) + mind;
		dam += dam * Players[pnum]._pIBonusDam / 100;
		dam += Players[pnum]._pIBonusDamMod;
#ifdef HELLFIRE
		int dam2 = dam << 6;
#endif
		dam += Players[pnum]._pDamageMod;
		if (Players[pnum]._pClass == PC_WARRIOR
#ifdef HELLFIRE
		    || Players[pnum]._pClass == PC_BARBARIAN
#endif
		) {
			ddp = Players[pnum]._pLevel;
			if (random_(6, 100) < ddp) {
				dam <<= 1;
			}
		}

		phanditype = ITYPE_NONE;
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD) {
			phanditype = ITYPE_SWORD;
		}
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE || Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE) {
			phanditype = ITYPE_MACE;
		}

		switch (monster[m].MData->mMonstClass) {
		case MC_UNDEAD:
			if (phanditype == ITYPE_SWORD) {
				dam -= dam >> 1;
			}
#ifdef HELLFIRE
			else
#endif
			    if (phanditype == ITYPE_MACE) {
				dam += dam >> 1;
			}
			break;
		case MC_ANIMAL:
			if (phanditype == ITYPE_MACE) {
				dam -= dam >> 1;
			}
#ifdef HELLFIRE
			else
#endif
			    if (phanditype == ITYPE_SWORD) {
				dam += dam >> 1;
			}
			break;
		}

		if (Players[pnum]._pIFlags & ISPL_3XDAMVDEM && monster[m].MData->mMonstClass == MC_DEMON) {
			dam *= 3;
		}

#ifdef HELLFIRE
		if (Players[pnum].pDamAcFlags & ISPLHF_DEVASTATION && random_(6, 100) < 5) {
			dam *= 3;
		}

		if (Players[pnum].pDamAcFlags & ISPLHF_DOPPELGANGER && monster[m].MType->mtype != MT_DIABLO && monster[m]._uniqtype == 0 && random_(6, 100) < 10) {
			AddDoppelganger(m);
		}
#endif

		dam <<= 6;
#ifdef HELLFIRE
		if (Players[pnum].pDamAcFlags & ISPLHF_JESTERS) {
			int r = random_(6, 201);
			if (r >= 100)
				r = 100 + (r - 100) * 5;
			dam = dam * r / 100;
		}

		if (adjacentDamage)
			dam >>= 2;
#endif

		if (pnum == myplr) {
#ifdef HELLFIRE
			if (Players[pnum].pDamAcFlags & ISPLHF_PERIL) {
				dam2 += Players[pnum]._pIGetHit << 6;
				if (dam2 >= 0) {
					if (Players[pnum]._pHitPoints > dam2) {
						Players[pnum]._pHitPoints -= dam2;
						Players[pnum]._pHPBase -= dam2;
					} else {
						dam2 = (1 << 6);
						Players[pnum]._pHPBase -= Players[pnum]._pHitPoints - dam2;
						Players[pnum]._pHitPoints = dam2;
					}
				}
				dam <<= 1;
			}
#endif
			monster[m]._mhitpoints -= dam;
		}

		if (Players[pnum]._pIFlags & ISPL_RNDSTEALLIFE) {
			skdam = random_(7, dam >> 3);
			Players[pnum]._pHitPoints += skdam;
			if (Players[pnum]._pHitPoints > Players[pnum]._pMaxHP) {
				Players[pnum]._pHitPoints = Players[pnum]._pMaxHP;
			}
			Players[pnum]._pHPBase += skdam;
			if (Players[pnum]._pHPBase > Players[pnum]._pMaxHPBase) {
				Players[pnum]._pHPBase = Players[pnum]._pMaxHPBase;
			}
			drawhpflag = TRUE;
		}
		if (Players[pnum]._pIFlags & (ISPL_STEALMANA_3 | ISPL_STEALMANA_5) && !(Players[pnum]._pIFlags & ISPL_NOMANA)) {
			if (Players[pnum]._pIFlags & ISPL_STEALMANA_3) {
				skdam = 3 * dam / 100;
			}
			if (Players[pnum]._pIFlags & ISPL_STEALMANA_5) {
				skdam = 5 * dam / 100;
			}
			Players[pnum]._pMana += skdam;
			if (Players[pnum]._pMana > Players[pnum]._pMaxMana) {
				Players[pnum]._pMana = Players[pnum]._pMaxMana;
			}
			Players[pnum]._pManaBase += skdam;
			if (Players[pnum]._pManaBase > Players[pnum]._pMaxManaBase) {
				Players[pnum]._pManaBase = Players[pnum]._pMaxManaBase;
			}
			drawmanaflag = TRUE;
		}
		if (Players[pnum]._pIFlags & (ISPL_STEALLIFE_3 | ISPL_STEALLIFE_5)) {
			if (Players[pnum]._pIFlags & ISPL_STEALLIFE_3) {
				skdam = 3 * dam / 100;
			}
			if (Players[pnum]._pIFlags & ISPL_STEALLIFE_5) {
				skdam = 5 * dam / 100;
			}
			Players[pnum]._pHitPoints += skdam;
			if (Players[pnum]._pHitPoints > Players[pnum]._pMaxHP) {
				Players[pnum]._pHitPoints = Players[pnum]._pMaxHP;
			}
			Players[pnum]._pHPBase += skdam;
			if (Players[pnum]._pHPBase > Players[pnum]._pMaxHPBase) {
				Players[pnum]._pHPBase = Players[pnum]._pMaxHPBase;
			}
			drawhpflag = TRUE;
		}
		if (Players[pnum]._pIFlags & ISPL_NOHEALPLR) {
			monster[m]._mFlags |= MFLAG_NOHEAL;
		}
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v) {
			monster[m]._mhitpoints = 0; /* double check */
		}
#endif
		if ((monster[m]._mhitpoints >> 6) <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, pnum);
				monster[m]._mmode = MM_STONE;
			} else {
				M_StartKill(m, pnum);
			}
		} else {
			if (monster[m]._mmode == MM_STONE) {
				M_StartHit(m, pnum, dam);
				monster[m]._mmode = MM_STONE;
			} else {
				if (Players[pnum]._pIFlags & ISPL_KNOCKBACK) {
					M_GetKnockback(m);
				}
				M_StartHit(m, pnum, dam);
			}
		}
		rv = TRUE;
	}

	return rv;
}

BOOL PlrHitPlr(int pnum, char p)
{
	BOOL rv;
	int hit, hper, blk, blkper, dir, mind, maxd, dam, lvl, skdam, tac;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal target player %d", p);
	}

	rv = FALSE;

	if (Players[p]._pInvincible) {
		return rv;
	}

	if (Players[p]._pSpellFlags & 1) {
		return rv;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal attacking player %d", pnum);
	}

	hit = random_(4, 100);

	hper = (Players[pnum]._pDexterity >> 1) + Players[pnum]._pLevel + 50 - (Players[p]._pIBonusAC + Players[p]._pIAC + Players[p]._pDexterity / 5);

	if (Players[pnum]._pClass == PC_WARRIOR) {
		hper += 20;
	}
	hper += Players[pnum]._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if ((Players[p]._pmode == PM_STAND || Players[p]._pmode == PM_ATTACK) && Players[p]._pBlockFlag) {
		blk = random_(5, 100);
	} else {
		blk = 100;
	}

	blkper = Players[p]._pDexterity + Players[p]._pBaseToBlk + (Players[p]._pLevel << 1) - (Players[pnum]._pLevel << 1);
	if (blkper < 0) {
		blkper = 0;
	}
	if (blkper > 100) {
		blkper = 100;
	}

	if (hit < hper) {
		if (blk < blkper) {
			dir = GetDirection(Players[p]._px, Players[p]._py, Players[pnum]._px, Players[pnum]._py);
			StartPlrBlock(p, dir);
		} else {
			mind = Players[pnum]._pIMinDam;
			maxd = Players[pnum]._pIMaxDam;
			dam = random_(5, maxd - mind + 1) + mind;
			dam += (dam * Players[pnum]._pIBonusDam) / 100;
			dam += Players[pnum]._pIBonusDamMod + Players[pnum]._pDamageMod;

			if (Players[pnum]._pClass == PC_WARRIOR
#ifdef HELLFIRE
			    || Players[pnum]._pClass == PC_BARBARIAN
#endif
			) {
				lvl = Players[pnum]._pLevel;
				if (random_(6, 100) < lvl) {
					dam <<= 1;
				}
			}
			skdam = dam << 6;
			if (Players[pnum]._pIFlags & ISPL_RNDSTEALLIFE) {
				tac = random_(7, skdam >> 3);
				Players[pnum]._pHitPoints += tac;
				if (Players[pnum]._pHitPoints > Players[pnum]._pMaxHP) {
					Players[pnum]._pHitPoints = Players[pnum]._pMaxHP;
				}
				Players[pnum]._pHPBase += tac;
				if (Players[pnum]._pHPBase > Players[pnum]._pMaxHPBase) {
					Players[pnum]._pHPBase = Players[pnum]._pMaxHPBase;
				}
				drawhpflag = TRUE;
			}
			if (pnum == myplr) {
				NetSendCmdDamage(TRUE, p, skdam);
			}
			StartPlrHit(p, skdam, FALSE);
		}

		rv = TRUE;
	}

	return rv;
}

BOOL PlrHitObj(int pnum, int mx, int my)
{
	int oi;

	if (dObject[mx][my] > 0) {
		oi = dObject[mx][my] - 1;
	} else {
		oi = -dObject[mx][my] - 1;
	}

	if (object[oi]._oBreak == 1) {
		BreakObject(pnum, oi);
		return TRUE;
	}

	return FALSE;
}

BOOL PM_DoAttack(int pnum)
{
	int frame, dir, dx, dy, m;
	BOOL didhit = FALSE;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoAttack: illegal player %d", pnum);
	}

	frame = Players[pnum]._pAnimFrame;
	if (Players[pnum]._pIFlags & ISPL_QUICKATTACK && frame == 1) {
		Players[pnum]._pAnimFrame++;
	}
	if (Players[pnum]._pIFlags & ISPL_FASTATTACK && (frame == 1 || frame == 3)) {
		Players[pnum]._pAnimFrame++;
	}
	if (Players[pnum]._pIFlags & ISPL_FASTERATTACK && (frame == 1 || frame == 3 || frame == 5)) {
		Players[pnum]._pAnimFrame++;
	}
	if (Players[pnum]._pIFlags & ISPL_FASTESTATTACK && (frame == 1 || frame == 4)) {
		Players[pnum]._pAnimFrame += 2;
	}
	if (Players[pnum]._pAnimFrame == Players[pnum]._pAFNum - 1) {
		PlaySfxLoc(PS_SWING, Players[pnum]._px, Players[pnum]._py);
	}

	if (Players[pnum]._pAnimFrame == Players[pnum]._pAFNum) {
		dx = Players[pnum]._px + offset_x[Players[pnum]._pdir];
		dy = Players[pnum]._py + offset_y[Players[pnum]._pdir];

		if (dMonster[dx][dy] != 0) {
			if (dMonster[dx][dy] > 0) {
				m = dMonster[dx][dy] - 1;
			} else {
				m = -(dMonster[dx][dy] + 1);
			}
			if (CanTalkToMonst(m)) {
				Players[pnum]._pVar1 = 0;
				return FALSE;
			}
		}

#ifdef HELLFIRE
		if (!(Players[pnum]._pIFlags & ISPL_FIREDAM) || !(Players[pnum]._pIFlags & ISPL_LIGHTDAM))
#endif
			if (Players[pnum]._pIFlags & ISPL_FIREDAM) {
				AddMissile(dx, dy, 1, 0, 0, MIS_WEAPEXP, TARGET_MONSTERS, pnum, 0, 0);
			}
#ifdef HELLFIRE
			else
#endif
			    if (Players[pnum]._pIFlags & ISPL_LIGHTDAM) {
				AddMissile(dx, dy, 2, 0, 0, MIS_WEAPEXP, TARGET_MONSTERS, pnum, 0, 0);
			}

		if (dMonster[dx][dy]) {
			m = dMonster[dx][dy];
			if (dMonster[dx][dy] > 0) {
				m = dMonster[dx][dy] - 1;
			} else {
				m = -(dMonster[dx][dy] + 1);
			}
			didhit = PlrHitMonst(pnum, m);
		} else if (dPlayer[dx][dy] != 0 && !FriendlyMode) {
			BYTE p = dPlayer[dx][dy];
			if (dPlayer[dx][dy] > 0) {
				p = dPlayer[dx][dy] - 1;
			} else {
				p = -(dPlayer[dx][dy] + 1);
			}
			didhit = PlrHitPlr(pnum, p);
		} else if (dObject[dx][dy] > 0) {
			didhit = PlrHitObj(pnum, dx, dy);
		}
#ifdef HELLFIRE
		if ((Players[pnum]._pClass == PC_MONK
		        && (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF || Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_STAFF))
		    || (Players[pnum]._pClass == PC_BARD
		        && Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD)
		    || (Players[pnum]._pClass == PC_BARBARIAN
		        && (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_AXE || Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_AXE
		            || (((Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE && Players[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND)
		                    || (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iLoc == ILOC_TWOHAND)
		                    || (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD && Players[pnum].InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND)
		                    || (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD && Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iLoc == ILOC_TWOHAND))
		                && !(Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD || Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD))))) {
			dx = Players[pnum]._px + offset_x[(Players[pnum]._pdir + 1) % 8];
			dy = Players[pnum]._py + offset_y[(Players[pnum]._pdir + 1) % 8];
			m = ((dMonster[dx][dy] > 0) ? dMonster[dx][dy] : -dMonster[dx][dy]) - 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) && monster[m]._moldx == dx && monster[m]._moldy == dy) {
				if (PlrHitMonst(-pnum, m))
					didhit = TRUE;
			}
			dx = Players[pnum]._px + offset_x[(Players[pnum]._pdir + 7) % 8];
			dy = Players[pnum]._py + offset_y[(Players[pnum]._pdir + 7) % 8];
			m = ((dMonster[dx][dy] > 0) ? dMonster[dx][dy] : -dMonster[dx][dy]) - 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) && monster[m]._moldx == dx && monster[m]._moldy == dy) {
				if (PlrHitMonst(-pnum, m))
					didhit = TRUE;
			}
		}
#endif

		if (didhit && WeaponDur(pnum, 30)) {
			StartStand(pnum, Players[pnum]._pdir);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	}

	if (Players[pnum]._pAnimFrame == Players[pnum]._pAFrames) {
		StartStand(pnum, Players[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL PM_DoRangeAttack(int pnum)
{
	int origFrame, mistype;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoRangeAttack: illegal player %d", pnum);
	}

	origFrame = Players[pnum]._pAnimFrame;
	if (Players[pnum]._pIFlags & ISPL_QUICKATTACK && origFrame == 1) {
		Players[pnum]._pAnimFrame++;
	}
	if (Players[pnum]._pIFlags & ISPL_FASTATTACK && (origFrame == 1 || origFrame == 3)) {
		Players[pnum]._pAnimFrame++;
	}

	if (Players[pnum]._pAnimFrame == Players[pnum]._pAFNum) {
		mistype = MIS_ARROW;
		if (Players[pnum]._pIFlags & ISPL_FIRE_ARROWS) {
			mistype = MIS_FARROW;
		}
		if (Players[pnum]._pIFlags & ISPL_LIGHT_ARROWS) {
			mistype = MIS_LARROW;
		}
		AddMissile(
		    Players[pnum]._px,
		    Players[pnum]._py,
		    Players[pnum]._pVar1,
		    Players[pnum]._pVar2,
		    Players[pnum]._pdir,
		    mistype,
		    TARGET_MONSTERS,
		    pnum,
		    4,
		    0);

		PlaySfxLoc(PS_BFIRE, Players[pnum]._px, Players[pnum]._py);

		if (WeaponDur(pnum, 40)) {
			StartStand(pnum, Players[pnum]._pdir);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	}

	if (Players[pnum]._pAnimFrame >= Players[pnum]._pAFrames) {
		StartStand(pnum, Players[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	} else {
		return FALSE;
	}
}

void ShieldDur(int pnum)
{
	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ShieldDur: illegal player %d", pnum);
	}

	if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return;
		}

		Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (Players[pnum].InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(TRUE, INVLOC_HAND_LEFT);
			Players[pnum].InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, TRUE);
		}
	}

	if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability != DUR_INDESTRUCTIBLE) {
			Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability--;
			if (Players[pnum].InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
				NetSendCmdDelItem(TRUE, INVLOC_HAND_RIGHT);
				Players[pnum].InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
				CalcPlrInv(pnum, TRUE);
			}
		}
	}
}

BOOL PM_DoBlock(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoBlock: illegal player %d", pnum);
	}

	if (Players[pnum]._pIFlags & ISPL_FASTBLOCK && Players[pnum]._pAnimFrame != 1) {
		Players[pnum]._pAnimFrame = Players[pnum]._pBFrames;
	}

	if (Players[pnum]._pAnimFrame >= Players[pnum]._pBFrames) {
		StartStand(pnum, Players[pnum]._pdir);
		ClearPlrPVars(pnum);

		if (random_(3, 10) == 0) {
			ShieldDur(pnum);
		}
		return TRUE;
	}

	return FALSE;
}

static void ArmorDur(int pnum)
{
	int a;
	Item *pi;
	Player *p;

	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ArmorDur: illegal player %d", pnum);
	}

	p = &Players[pnum];
	if (p->InvBody[INVLOC_CHEST]._itype == ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype == ITYPE_NONE) {
		return;
	}

	a = random_(8, 3);
	if (p->InvBody[INVLOC_CHEST]._itype != ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype == ITYPE_NONE) {
		a = 1;
	}
	if (p->InvBody[INVLOC_CHEST]._itype == ITYPE_NONE && p->InvBody[INVLOC_HEAD]._itype != ITYPE_NONE) {
		a = 0;
	}

	if (a != 0) {
		pi = &p->InvBody[INVLOC_CHEST];
	} else {
		pi = &p->InvBody[INVLOC_HEAD];
	}
	if (pi->_iDurability == DUR_INDESTRUCTIBLE) {
		return;
	}

	pi->_iDurability--;
	if (pi->_iDurability != 0) {
		return;
	}

	if (a != 0) {
		NetSendCmdDelItem(TRUE, INVLOC_CHEST);
	} else {
		NetSendCmdDelItem(TRUE, INVLOC_HEAD);
	}
	pi->_itype = ITYPE_NONE;
	CalcPlrInv(pnum, TRUE);
}

BOOL PM_DoSpell(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoSpell: illegal player %d", pnum);
	}

	if (Players[pnum]._pVar8 == Players[pnum]._pSFNum) {
		CastSpell(
		    pnum,
		    Players[pnum]._pSpell,
		    Players[pnum]._px,
		    Players[pnum]._py,
		    Players[pnum]._pVar1,
		    Players[pnum]._pVar2,
		    TARGET_MONSTERS,
		    Players[pnum]._pVar4);

		if (Players[pnum]._pSplFrom == 0) {
			if (Players[pnum]._pRSplType == RSPLTYPE_SCROLL) {
				if (!(Players[pnum]._pScrlSpells
				        & SPELLBIT(Players[pnum]._pRSpell))) {
					Players[pnum]._pRSpell = SPL_INVALID;
					Players[pnum]._pRSplType = RSPLTYPE_INVALID;
					force_redraw = 255;
				}
			}

			if (Players[pnum]._pRSplType == RSPLTYPE_CHARGES) {
				if (!(Players[pnum]._pISpells
				        & SPELLBIT(Players[pnum]._pRSpell))) {
					Players[pnum]._pRSpell = SPL_INVALID;
					Players[pnum]._pRSplType = RSPLTYPE_INVALID;
					force_redraw = 255;
				}
			}
		}
	}

	Players[pnum]._pVar8++;

	if (leveltype == DTYPE_TOWN) {
		if (Players[pnum]._pVar8 > Players[pnum]._pSFrames) {
			StartWalkStand(pnum);
			ClearPlrPVars(pnum);
			return TRUE;
		}
	} else if (Players[pnum]._pAnimFrame == Players[pnum]._pSFrames) {
		StartStand(pnum, Players[pnum]._pdir);
		ClearPlrPVars(pnum);
		return TRUE;
	}

	return FALSE;
}

BOOL PM_DoGotHit(int pnum)
{
	int frame;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoGotHit: illegal player %d", pnum);
	}

#ifdef HELLFIRE
	if (Players[pnum]._pIFlags & (ISPL_FASTRECOVER | ISPL_FASTERRECOVER | ISPL_FASTESTRECOVER)) {
		frame = 3;
		if (Players[pnum]._pIFlags & ISPL_FASTERRECOVER)
			frame = 4;
		if (Players[pnum]._pIFlags & ISPL_FASTESTRECOVER)
			frame = 5;
		if (Players[pnum]._pVar8 > 1 && Players[pnum]._pVar8 < frame) {
			Players[pnum]._pVar8 = frame;
		}
		if (Players[pnum]._pVar8 > Players[pnum]._pHFrames)
			Players[pnum]._pVar8 = Players[pnum]._pHFrames;
	}

	if (Players[pnum]._pVar8 == Players[pnum]._pHFrames) {
#else
	frame = Players[pnum]._pAnimFrame;
	if (Players[pnum]._pIFlags & ISPL_FASTRECOVER && frame == 3) {
		Players[pnum]._pAnimFrame++;
	}
	if (Players[pnum]._pIFlags & ISPL_FASTERRECOVER && (frame == 3 || frame == 5)) {
		Players[pnum]._pAnimFrame++;
	}
	if (Players[pnum]._pIFlags & ISPL_FASTESTRECOVER && (frame == 1 || frame == 3 || frame == 5)) {
		Players[pnum]._pAnimFrame++;
	}

	if (Players[pnum]._pAnimFrame >= Players[pnum]._pHFrames) {
#endif
		StartStand(pnum, Players[pnum]._pdir);
		ClearPlrPVars(pnum);
		if (random_(3, 4) != 0) {
			ArmorDur(pnum);
		}

		return TRUE;
	}

#ifdef HELLFIRE
	Players[pnum]._pVar8++;
#endif
	return FALSE;
}

BOOL PM_DoDeath(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoDeath: illegal player %d", pnum);
	}

	if (Players[pnum]._pVar8 >= 2 * Players[pnum]._pDFrames) {
		if (deathdelay > 1 && pnum == myplr) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = TRUE;
				if (gbMaxPlayers == 1) {
					gamemenu_on();
				}
			}
		}

		Players[pnum]._pAnimDelay = 10000;
		Players[pnum]._pAnimFrame = Players[pnum]._pAnimLen;
		dFlags[Players[pnum]._px][Players[pnum]._py] |= BFLAG_DEAD_PLAYER;
	}

	if (Players[pnum]._pVar8 < 100) {
		Players[pnum]._pVar8++;
	}

	return FALSE;
}

BOOL PM_DoNewLvl(int pnum)
{
	return FALSE;
}

void CheckNewPath(int pnum)
{
	int i, x, y, d;
	int xvel3, xvel, yvel;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("CheckNewPath: illegal player %d", pnum);
	}

	if (Players[pnum].destAction == ACTION_ATTACKMON) {
		i = Players[pnum].destParam1;
		MakePlrPath(pnum, monster[i]._mfutx, monster[i]._mfuty, FALSE);
	}

	if (Players[pnum].destAction == ACTION_ATTACKPLR) {
		i = Players[pnum].destParam1;
		MakePlrPath(pnum, Players[i]._pfutx, Players[i]._pfuty, FALSE);
	}

	if (Players[pnum].walkpath[0] != WALK_NONE) {
		if (Players[pnum]._pmode == PM_STAND) {
			if (pnum == myplr) {
				if (Players[pnum].destAction == ACTION_ATTACKMON || Players[pnum].destAction == ACTION_ATTACKPLR) {
					i = Players[pnum].destParam1;

					if (Players[pnum].destAction == ACTION_ATTACKMON) {
						x = abs(Players[pnum]._pfutx - monster[i]._mfutx);
						y = abs(Players[pnum]._pfuty - monster[i]._mfuty);
						d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
					} else {
						x = abs(Players[pnum]._pfutx - Players[i]._pfutx);
						y = abs(Players[pnum]._pfuty - Players[i]._pfuty);
						d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, Players[i]._pfutx, Players[i]._pfuty);
					}

					if (x < 2 && y < 2) {
						ClrPlrPath(pnum);
						// BUGFIX: missing check for `destAction == ACTION_ATTACKMON` in if-statement of `TalktoMonster` branch.
						if (monster[i].mtalkmsg && monster[i].mtalkmsg != TEXT_VILE14) {
							TalktoMonster(i);
						} else {
							StartAttack(pnum, d);
						}
						Players[pnum].destAction = ACTION_NONE;
					}
				}
			}

			if (currlevel != 0) {
				xvel3 = PWVel[Players[pnum]._pClass][0];
				xvel = PWVel[Players[pnum]._pClass][1];
				yvel = PWVel[Players[pnum]._pClass][2];
			} else {
				xvel3 = 2048;
				xvel = 1024;
				yvel = 512;
			}

			switch (Players[pnum].walkpath[0]) {
			case WALK_N:
				StartWalk(pnum, 0, -xvel, -1, -1, DIR_N, SDIR_N);
				break;
			case WALK_NE:
				StartWalk(pnum, xvel, -yvel, 0, -1, DIR_NE, SDIR_NE);
				break;
			case WALK_E:
				StartWalk3(pnum, xvel3, 0, -32, -16, 1, -1, 1, 0, DIR_E, SDIR_E);
				break;
			case WALK_SE:
				StartWalk2(pnum, xvel, yvel, -32, -16, 1, 0, DIR_SE, SDIR_SE);
				break;
			case WALK_S:
				StartWalk2(pnum, 0, xvel, 0, -32, 1, 1, DIR_S, SDIR_S);
				break;
			case WALK_SW:
				StartWalk2(pnum, -xvel, yvel, 32, -16, 0, 1, DIR_SW, SDIR_SW);
				break;
			case WALK_W:
				StartWalk3(pnum, -xvel3, 0, 32, -16, -1, 1, 0, 1, DIR_W, SDIR_W);
				break;
			case WALK_NW:
				StartWalk(pnum, -xvel, -yvel, -1, 0, DIR_NW, SDIR_NW);
				break;
			}

			for (i = 1; i < MAX_PATH_LENGTH; i++) {
				Players[pnum].walkpath[i - 1] = Players[pnum].walkpath[i];
			}

			Players[pnum].walkpath[MAX_PATH_LENGTH - 1] = WALK_NONE;

			if (Players[pnum]._pmode == PM_STAND) {
				StartStand(pnum, Players[pnum]._pdir);
				Players[pnum].destAction = ACTION_NONE;
			}
		}

		return;
	}
	if (Players[pnum].destAction == ACTION_NONE) {
		return;
	}

	if (Players[pnum]._pmode == PM_STAND) {
		switch (Players[pnum].destAction) {
		case ACTION_ATTACK:
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[pnum].destParam1, Players[pnum].destParam2);
			StartAttack(pnum, d);
			break;
		case ACTION_ATTACKMON:
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - monster[i]._mfutx);
			y = abs(Players[pnum]._py - monster[i]._mfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
				if (monster[i].mtalkmsg && monster[i].mtalkmsg != TEXT_VILE14) {
					TalktoMonster(i);
				} else {
					StartAttack(pnum, d);
				}
			}
			break;
		case ACTION_ATTACKPLR:
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - Players[i]._pfutx);
			y = abs(Players[pnum]._py - Players[i]._pfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, Players[i]._pfutx, Players[i]._pfuty);
				StartAttack(pnum, d);
			}
			break;
		case ACTION_RATTACK:
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[pnum].destParam1, Players[pnum].destParam2);
			StartRangeAttack(pnum, d, Players[pnum].destParam1, Players[pnum].destParam2);
			break;
		case ACTION_RATTACKMON:
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
			if (monster[i].mtalkmsg && monster[i].mtalkmsg != TEXT_VILE14) {
				TalktoMonster(i);
			} else {
				StartRangeAttack(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			}
			break;
		case ACTION_RATTACKPLR:
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, Players[i]._pfutx, Players[i]._pfuty);
			StartRangeAttack(pnum, d, Players[i]._pfutx, Players[i]._pfuty);
			break;
		case ACTION_SPELL:
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[pnum].destParam1, Players[pnum].destParam2);
			StartSpell(pnum, d, Players[pnum].destParam1, Players[pnum].destParam2);
			Players[pnum]._pVar4 = Players[pnum].destParam3;
			break;
		case ACTION_SPELLWALL:
			StartSpell(pnum, Players[pnum].destParam3, Players[pnum].destParam1, Players[pnum].destParam2);
			Players[pnum]._pVar3 = Players[pnum].destParam3;
			Players[pnum]._pVar4 = Players[pnum].destParam4;
			break;
		case ACTION_SPELLMON:
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
			StartSpell(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			Players[pnum]._pVar4 = Players[pnum].destParam2;
			break;
		case ACTION_SPELLPLR:
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[i]._pfutx, Players[i]._pfuty);
			StartSpell(pnum, d, Players[i]._pfutx, Players[i]._pfuty);
			Players[pnum]._pVar4 = Players[pnum].destParam2;
			break;
		case ACTION_OPERATE:
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - object[i]._ox);
			y = abs(Players[pnum]._py - object[i]._oy);
			if (y > 1 && dObject[object[i]._ox][object[i]._oy - 1] == -1 - i) {
				y = abs(Players[pnum]._py - object[i]._oy + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(Players[pnum]._px, Players[pnum]._py, object[i]._ox, object[i]._oy);
					StartAttack(pnum, d);
				} else {
					OperateObject(pnum, i, FALSE);
				}
			}
			break;
		case ACTION_DISARM:
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - object[i]._ox);
			y = abs(Players[pnum]._py - object[i]._oy);
			if (y > 1 && dObject[object[i]._ox][object[i]._oy - 1] == -1 - i) {
				y = abs(Players[pnum]._py - object[i]._oy + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(Players[pnum]._px, Players[pnum]._py, object[i]._ox, object[i]._oy);
					StartAttack(pnum, d);
				} else {
					TryDisarm(pnum, i);
					OperateObject(pnum, i, FALSE);
				}
			}
			break;
		case ACTION_OPERATETK:
			i = Players[pnum].destParam1;
			if (object[i]._oBreak != 1) {
				OperateObject(pnum, i, TRUE);
			}
			break;
		case ACTION_PICKUPITEM:
			if (pnum == myplr) {
				i = Players[pnum].destParam1;
				x = abs(Players[pnum]._px - GroundItems[i]._ix);
				y = abs(Players[pnum]._py - GroundItems[i]._iy);
				if (x <= 1 && y <= 1 && pcurs == CURSOR_HAND && !GroundItems[i]._iRequest) {
					NetSendCmdGItem(TRUE, CMD_REQUESTGITEM, myplr, myplr, i);
					GroundItems[i]._iRequest = TRUE;
				}
			}
			break;
		case ACTION_PICKUPAITEM:
			if (pnum == myplr) {
				i = Players[pnum].destParam1;
				x = abs(Players[pnum]._px - GroundItems[i]._ix);
				y = abs(Players[pnum]._py - GroundItems[i]._iy);
				if (x <= 1 && y <= 1 && pcurs == CURSOR_HAND) {
					NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, myplr, myplr, i);
				}
			}
			break;
		case ACTION_TALK:
			if (pnum == myplr) {
				TalkToTowner(pnum, Players[pnum].destParam1);
			}
			break;
		}

		FixPlayerLocation(pnum, Players[pnum]._pdir);
		Players[pnum].destAction = ACTION_NONE;

		return;
	}

	if (Players[pnum]._pmode == PM_ATTACK && Players[pnum]._pAnimFrame > Players[myplr]._pAFNum) {
		if (Players[pnum].destAction == ACTION_ATTACK) {
			d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, Players[pnum].destParam1, Players[pnum].destParam2);
			StartAttack(pnum, d);
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_ATTACKMON) {
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - monster[i]._mfutx);
			y = abs(Players[pnum]._py - monster[i]._mfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, monster[i]._mfutx, monster[i]._mfuty);
				StartAttack(pnum, d);
			}
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_ATTACKPLR) {
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - Players[i]._pfutx);
			y = abs(Players[pnum]._py - Players[i]._pfuty);
			if (x <= 1 && y <= 1) {
				d = GetDirection(Players[pnum]._pfutx, Players[pnum]._pfuty, Players[i]._pfutx, Players[i]._pfuty);
				StartAttack(pnum, d);
			}
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_OPERATE) {
			i = Players[pnum].destParam1;
			x = abs(Players[pnum]._px - object[i]._ox);
			y = abs(Players[pnum]._py - object[i]._oy);
			if (y > 1 && dObject[object[i]._ox][object[i]._oy - 1] == -1 - i) {
				y = abs(Players[pnum]._py - object[i]._oy + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(Players[pnum]._px, Players[pnum]._py, object[i]._ox, object[i]._oy);
					StartAttack(pnum, d);
				} else {
					OperateObject(pnum, i, FALSE);
				}
			}
		}
	}

	if (Players[pnum]._pmode == PM_RATTACK && Players[pnum]._pAnimFrame > Players[myplr]._pAFNum) {
		if (Players[pnum].destAction == ACTION_RATTACK) {
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[pnum].destParam1, Players[pnum].destParam2);
			StartRangeAttack(pnum, d, Players[pnum].destParam1, Players[pnum].destParam2);
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_RATTACKMON) {
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
			StartRangeAttack(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_RATTACKPLR) {
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[i]._pfutx, Players[i]._pfuty);
			StartRangeAttack(pnum, d, Players[i]._pfutx, Players[i]._pfuty);
			Players[pnum].destAction = ACTION_NONE;
		}
	}

	if (Players[pnum]._pmode == PM_SPELL && Players[pnum]._pAnimFrame > Players[pnum]._pSFNum) {
		if (Players[pnum].destAction == ACTION_SPELL) {
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[pnum].destParam1, Players[pnum].destParam2);
			StartSpell(pnum, d, Players[pnum].destParam1, Players[pnum].destParam2);
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_SPELLMON) {
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, monster[i]._mfutx, monster[i]._mfuty);
			StartSpell(pnum, d, monster[i]._mfutx, monster[i]._mfuty);
			Players[pnum].destAction = ACTION_NONE;
		} else if (Players[pnum].destAction == ACTION_SPELLPLR) {
			i = Players[pnum].destParam1;
			d = GetDirection(Players[pnum]._px, Players[pnum]._py, Players[i]._pfutx, Players[i]._pfuty);
			StartSpell(pnum, d, Players[i]._pfutx, Players[i]._pfuty);
			Players[pnum].destAction = ACTION_NONE;
		}
	}
}

BOOL PlrDeathModeOK(int p)
{
	if (p != myplr) {
		return TRUE;
	}

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrDeathModeOK: illegal player %d", p);
	}

	if (Players[p]._pmode == PM_DEATH) {
		return TRUE;
	} else if (Players[p]._pmode == PM_QUIT) {
		return TRUE;
	} else if (Players[p]._pmode == PM_NEWLVL) {
		return TRUE;
	}

	return FALSE;
}

void ValidatePlayer()
{
	__int64 msk;
	int gt, pc, i, b;

	msk = 0;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("ValidatePlayer: illegal player %d", myplr);
	}
	if (Players[myplr]._pLevel > MAXCHARLEVEL - 1)
		Players[myplr]._pLevel = MAXCHARLEVEL - 1;
	if (Players[myplr]._pExperience > Players[myplr]._pNextExper)
		Players[myplr]._pExperience = Players[myplr]._pNextExper;

	gt = 0;
	for (i = 0; i < Players[myplr]._pNumInv; i++) {
		if (Players[myplr].InvList[i]._itype == ITYPE_GOLD) {
#ifdef HELLFIRE
			if (Players[myplr].InvList[i]._ivalue > auricGold) {
				Players[myplr].InvList[i]._ivalue = auricGold;
#else
			if (Players[myplr].InvList[i]._ivalue > GOLD_MAX_LIMIT) {
				Players[myplr].InvList[i]._ivalue = GOLD_MAX_LIMIT;
#endif
			}
			gt += Players[myplr].InvList[i]._ivalue;
		}
	}
	if (gt != Players[myplr]._pGold)
		Players[myplr]._pGold = gt;

	pc = Players[myplr]._pClass;
	if (Players[myplr]._pBaseStr > MaxStats[pc][ATTRIB_STR]) {
		Players[myplr]._pBaseStr = MaxStats[pc][ATTRIB_STR];
	}
	if (Players[myplr]._pBaseMag > MaxStats[pc][ATTRIB_MAG]) {
		Players[myplr]._pBaseMag = MaxStats[pc][ATTRIB_MAG];
	}
	if (Players[myplr]._pBaseDex > MaxStats[pc][ATTRIB_DEX]) {
		Players[myplr]._pBaseDex = MaxStats[pc][ATTRIB_DEX];
	}
	if (Players[myplr]._pBaseVit > MaxStats[pc][ATTRIB_VIT]) {
		Players[myplr]._pBaseVit = MaxStats[pc][ATTRIB_VIT];
	}

	for (b = 1; b < MAX_SPELLS; b++) {
		if (spelldata[b].sBookLvl != -1) {
			msk |= SPELLBIT(b);
			if (Players[myplr]._pSplLvl[b] > MAX_SPELL_LEVEL)
				Players[myplr]._pSplLvl[b] = MAX_SPELL_LEVEL;
		}
	}

	Players[myplr]._pMemSpells &= msk;
}

static void CheckCheatStats(int pnum)
{
	if (Players[pnum]._pStrength > 750) {
		Players[pnum]._pStrength = 750;
	}

	if (Players[pnum]._pDexterity > 750) {
		Players[pnum]._pDexterity = 750;
	}

	if (Players[pnum]._pMagic > 750) {
		Players[pnum]._pMagic = 750;
	}

	if (Players[pnum]._pVitality > 750) {
		Players[pnum]._pVitality = 750;
	}

	if (Players[pnum]._pHitPoints > 128000) {
		Players[pnum]._pHitPoints = 128000;
	}

	if (Players[pnum]._pMana > 128000) {
		Players[pnum]._pMana = 128000;
	}
}

void ProcessPlayers()
{
	int pnum;
	BOOL tplayer;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("ProcessPlayers: illegal player %d", myplr);
	}

	if (Players[myplr].pLvlLoad > 0) {
		Players[myplr].pLvlLoad--;
	}

	if (sfxdelay > 0) {
		sfxdelay--;
		if (sfxdelay == 0) {
#ifdef HELLFIRE
			switch (sfxdnum) {
			case USFX_DEFILER1:
				InitQTextMsg(286);
				break;
			case USFX_DEFILER2:
				InitQTextMsg(287);
				break;
			case USFX_DEFILER3:
				InitQTextMsg(288);
				break;
			case USFX_DEFILER4:
				InitQTextMsg(289);
				break;
			default:
#endif
				PlaySFX(sfxdnum);
#ifdef HELLFIRE
			}
#endif
		}
	}

	ValidatePlayer();

	for (pnum = 0; pnum < MAX_PLRS; pnum++) {
		if (Players[pnum].plractive && currlevel == Players[pnum].plrlevel && (pnum == myplr || !Players[pnum]._pLvlChanging)) {
			CheckCheatStats(pnum);

			if (!PlrDeathModeOK(pnum) && (Players[pnum]._pHitPoints >> 6) <= 0) {
				SyncPlrKill(pnum, -1);
			}

			if (pnum == myplr) {
				if ((Players[pnum]._pIFlags & ISPL_DRAINLIFE) && currlevel != 0) {
					Players[pnum]._pHitPoints -= 4;
					Players[pnum]._pHPBase -= 4;
					if ((Players[pnum]._pHitPoints >> 6) <= 0) {
						SyncPlrKill(pnum, 0);
					}
					drawhpflag = TRUE;
				}
				if (Players[pnum]._pIFlags & ISPL_NOMANA && Players[pnum]._pManaBase > 0) {
					Players[pnum]._pManaBase -= Players[pnum]._pMana;
					Players[pnum]._pMana = 0;
					drawmanaflag = TRUE;
				}
			}

			tplayer = FALSE;
			do {
				switch (Players[pnum]._pmode) {
				case PM_STAND:
					tplayer = PM_DoStand(pnum);
					break;
				case PM_WALK:
					tplayer = PM_DoWalk(pnum);
					break;
				case PM_WALK2:
					tplayer = PM_DoWalk2(pnum);
					break;
				case PM_WALK3:
					tplayer = PM_DoWalk3(pnum);
					break;
				case PM_ATTACK:
					tplayer = PM_DoAttack(pnum);
					break;
				case PM_RATTACK:
					tplayer = PM_DoRangeAttack(pnum);
					break;
				case PM_BLOCK:
					tplayer = PM_DoBlock(pnum);
					break;
				case PM_SPELL:
					tplayer = PM_DoSpell(pnum);
					break;
				case PM_GOTHIT:
					tplayer = PM_DoGotHit(pnum);
					break;
				case PM_DEATH:
					tplayer = PM_DoDeath(pnum);
					break;
				case PM_NEWLVL:
					tplayer = PM_DoNewLvl(pnum);
					break;
				}
				CheckNewPath(pnum);
			} while (tplayer);

			Players[pnum]._pAnimCnt++;
			if (Players[pnum]._pAnimCnt > Players[pnum]._pAnimDelay) {
				Players[pnum]._pAnimCnt = 0;
				Players[pnum]._pAnimFrame++;
				if (Players[pnum]._pAnimFrame > Players[pnum]._pAnimLen) {
					Players[pnum]._pAnimFrame = 1;
				}
			}
		}
	}
}

void ClrPlrPath(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ClrPlrPath: illegal player %d", pnum);
	}

	memset(Players[pnum].walkpath, WALK_NONE, sizeof(Players[pnum].walkpath));
}

BOOL PosOkPlayer(int pnum, int x, int y)
{
	BOOL PosOK;
	DWORD p;
	char bv;

#ifndef HELLFIRE
	PosOK = FALSE;
	if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && !SolidLoc(x, y) && dPiece[x][y] != 0) {
#else
	if (dPiece[x][y] == 0)
		return FALSE;
	if (SolidLoc(x, y))
		return FALSE;
#endif
		if (dPlayer[x][y] != 0) {
			if (dPlayer[x][y] > 0) {
				p = dPlayer[x][y] - 1;
			} else {
				p = -(dPlayer[x][y] + 1);
			}
			if (p != pnum
#ifndef HELLFIRE
			    && p < MAX_PLRS
#endif
			    && Players[p]._pHitPoints != 0) {
				return FALSE;
			}
		}

		if (dMonster[x][y] != 0) {
			if (currlevel == 0) {
				return FALSE;
			}
			if (dMonster[x][y] <= 0) {
				return FALSE;
			}
			if ((monster[dMonster[x][y] - 1]._mhitpoints >> 6) > 0) {
				return FALSE;
			}
		}

		if (dObject[x][y] != 0) {
			if (dObject[x][y] > 0) {
				bv = dObject[x][y] - 1;
			} else {
				bv = -(dObject[x][y] + 1);
			}
			if (object[bv]._oSolidFlag) {
				return FALSE;
			}
		}

#ifndef HELLFIRE
		PosOK = TRUE;
	}

	if (!PosOK)
		return FALSE;
#endif
	return TRUE;
}

void MakePlrPath(int pnum, int xx, int yy, BOOL endspace)
{
	int path;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("MakePlrPath: illegal player %d", pnum);
	}

	Players[pnum]._ptargx = xx;
	Players[pnum]._ptargy = yy;
	if (Players[pnum]._pfutx == xx && Players[pnum]._pfuty == yy) {
		return;
	}

	path = FindPath(PosOkPlayer, pnum, Players[pnum]._pfutx, Players[pnum]._pfuty, xx, yy, Players[pnum].walkpath);
	if (!path) {
		return;
	}

	if (!endspace) {
		path--;

		switch (Players[pnum].walkpath[path]) {
		case WALK_NE:
			yy++;
			break;
		case WALK_NW:
			xx++;
			break;
		case WALK_SE:
			xx--;
			break;
		case WALK_SW:
			yy--;
			break;
		case WALK_N:
			xx++;
			yy++;
			break;
		case WALK_E:
			xx--;
			yy++;
			break;
		case WALK_S:
			xx--;
			yy--;
			break;
		case WALK_W:
			xx++;
			yy--;
			break;
		}

		Players[pnum]._ptargx = xx;
		Players[pnum]._ptargy = yy;
	}

	Players[pnum].walkpath[path] = WALK_NONE;
}

void CheckPlrSpell()
{
	BOOL addflag = FALSE;
	int rspell, sd, sl;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("CheckPlrSpell: illegal player %d", myplr);
	}

	rspell = Players[myplr]._pRSpell;
	if (rspell == SPL_INVALID) {
		if (Players[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR34);
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE34);
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE34);
#endif
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			PlaySFX(PS_MONK34);
		} else if (Players[myplr]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE34);
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR34);
#endif
		}
		return;
	}

	if (leveltype == DTYPE_TOWN && !spelldata[rspell].sTownSpell) {
		if (Players[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR27);
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE27);
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE27);
#endif
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			PlaySFX(PS_MONK27);
		} else if (Players[myplr]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE27);
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR27);
#endif
		}
		return;
	}

	if (pcurs != CURSOR_HAND)
		return;

	if (((MouseY >= PANEL_TOP)
	        || (chrflag && MouseX < SPANEL_WIDTH) || (invflag && MouseX > RIGHT_PANEL))
	    && ((MouseY >= PANEL_TOP) || (rspell != SPL_HEAL && rspell != SPL_IDENTIFY && rspell != SPL_REPAIR && rspell != SPL_INFRA && rspell != SPL_RECHARGE))) {
		return;
	}

	switch (Players[myplr]._pRSplType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_SPELL:
		addflag = CheckSpell(myplr, rspell, Players[myplr]._pRSplType, FALSE);
		break;
	case RSPLTYPE_SCROLL:
		addflag = UseScroll();
		break;
	case RSPLTYPE_CHARGES:
		addflag = UseStaff();
		break;
	}

	if (addflag) {
		if (Players[myplr]._pRSpell == SPL_FIREWALL
#ifdef HELLFIRE
		    || Players[myplr]._pRSpell == SPL_LIGHTWALL
#endif
		) {
			sd = GetDirection(Players[myplr]._px, Players[myplr]._py, cursmx, cursmy);
			sl = GetSpellLevel(myplr, Players[myplr]._pRSpell);
			NetSendCmdLocParam3(TRUE, CMD_SPELLXYD, cursmx, cursmy, Players[myplr]._pRSpell, sd, sl);
		} else if (pcursmonst != -1) {
			sl = GetSpellLevel(myplr, Players[myplr]._pRSpell);
			NetSendCmdParam3(TRUE, CMD_SPELLID, pcursmonst, Players[myplr]._pRSpell, sl);
		} else if (pcursplr != -1) {
			sl = GetSpellLevel(myplr, Players[myplr]._pRSpell);
			NetSendCmdParam3(TRUE, CMD_SPELLPID, pcursplr, Players[myplr]._pRSpell, sl);
		} else { // 145
			sl = GetSpellLevel(myplr, Players[myplr]._pRSpell);
			NetSendCmdLocParam2(TRUE, CMD_SPELLXY, cursmx, cursmy, Players[myplr]._pRSpell, sl);
		}
		return;
	}

	if (Players[myplr]._pRSplType == RSPLTYPE_SPELL) {
		if (Players[myplr]._pClass == PC_WARRIOR) {
			PlaySFX(PS_WARR35);
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			PlaySFX(PS_ROGUE35);
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			PlaySFX(PS_MAGE35);
#endif
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			PlaySFX(PS_MONK35);
		} else if (Players[myplr]._pClass == PC_BARD) {
			PlaySFX(PS_ROGUE35);
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			PlaySFX(PS_WARR35);
#endif
		}
	}
}

void SyncPlrAnim(int pnum)
{
	int dir, sType;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SyncPlrAnim: illegal player %d", pnum);
	}

	dir = Players[pnum]._pdir;
	switch (Players[pnum]._pmode) {
	case PM_STAND:
		Players[pnum]._pAnimData = Players[pnum]._pNAnim[dir];
		break;
	case PM_WALK:
	case PM_WALK2:
	case PM_WALK3:
		Players[pnum]._pAnimData = Players[pnum]._pWAnim[dir];
		break;
	case PM_ATTACK:
		Players[pnum]._pAnimData = Players[pnum]._pAAnim[dir];
		break;
	case PM_RATTACK:
		Players[pnum]._pAnimData = Players[pnum]._pAAnim[dir];
		break;
	case PM_BLOCK:
		Players[pnum]._pAnimData = Players[pnum]._pBAnim[dir];
		break;
	case PM_SPELL:
		if (pnum == myplr)
			sType = spelldata[Players[pnum]._pSpell].sType;
		else
			sType = STYPE_FIRE;
		if (sType == STYPE_FIRE)
			Players[pnum]._pAnimData = Players[pnum]._pFAnim[dir];
		if (sType == STYPE_LIGHTNING)
			Players[pnum]._pAnimData = Players[pnum]._pLAnim[dir];
		if (sType == STYPE_MAGIC)
			Players[pnum]._pAnimData = Players[pnum]._pTAnim[dir];
		break;
	case PM_GOTHIT:
		Players[pnum]._pAnimData = Players[pnum]._pHAnim[dir];
		break;
	case PM_NEWLVL:
		Players[pnum]._pAnimData = Players[pnum]._pNAnim[dir];
		break;
	case PM_DEATH:
		Players[pnum]._pAnimData = Players[pnum]._pDAnim[dir];
		break;
	case PM_QUIT:
		Players[pnum]._pAnimData = Players[pnum]._pNAnim[dir];
		break;
	default:
		app_fatal("SyncPlrAnim");
		break;
	}
}

void SyncInitPlrPos(int pnum)
{
	int x, y, xx, yy, range;
	DWORD i;
	BOOL posOk;

	Players[pnum]._ptargx = Players[pnum]._px;
	Players[pnum]._ptargy = Players[pnum]._py;

	if (gbMaxPlayers == 1 || Players[pnum].plrlevel != currlevel) {
		return;
	}

	for (i = 0; i < 8; i++) {
		x = Players[pnum]._px + plrxoff2[i];
		y = Players[pnum]._py + plryoff2[i];
		if (PosOkPlayer(pnum, x, y)) {
			break;
		}
	}

#ifdef HELLFIRE
	Players[pnum]._px += plrxoff2[i];
	Players[pnum]._py += plryoff2[i];
	dPlayer[Players[pnum]._px][Players[pnum]._py] = pnum + 1;
#else
	if (!PosOkPlayer(pnum, x, y)) {
		posOk = FALSE;
		for (range = 1; range < 50 && !posOk; range++) {
			for (yy = -range; yy <= range && !posOk; yy++) {
				y = yy + Players[pnum]._py;
				for (xx = -range; xx <= range && !posOk; xx++) {
					x = xx + Players[pnum]._px;
					if (PosOkPlayer(pnum, x, y) && !PosOkPortal(currlevel, x, y)) {
						posOk = TRUE;
					}
				}
			}
		}
	}

	Players[pnum]._px = x;
	Players[pnum]._py = y;
	dPlayer[x][y] = pnum + 1;

	if (pnum == myplr) {
		Players[pnum]._pfutx = x;
		Players[pnum]._pfuty = y;
		Players[pnum]._ptargx = x;
		Players[pnum]._ptargy = y;
		ViewX = x;
		ViewY = y;
	}
#endif
}

void SyncInitPlr(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SyncInitPlr: illegal player %d", pnum);
	}

	SetPlrAnims(pnum);
	SyncInitPlrPos(pnum);
}

void CheckStats(int p)
{
	int c, i;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("CheckStats: illegal player %d", p);
	}

	if (Players[p]._pClass == PC_WARRIOR) {
		c = PC_WARRIOR;
	} else if (Players[p]._pClass == PC_ROGUE) {
		c = PC_ROGUE;
	} else if (Players[p]._pClass == PC_SORCERER) {
		c = PC_SORCERER;
	}
#ifdef HELLFIRE
	else if (Players[p]._pClass == PC_MONK) {
		c = PC_MONK;
	} else if (Players[p]._pClass == PC_BARD) {
		c = PC_BARD;
	} else if (Players[p]._pClass == PC_BARBARIAN) {
		c = PC_BARBARIAN;
	}
#endif

	for (i = 0; i < 4; i++) {
		switch (i) {
		case ATTRIB_STR:
			if (Players[p]._pBaseStr > MaxStats[c][ATTRIB_STR]) {
				Players[p]._pBaseStr = MaxStats[c][ATTRIB_STR];
			} else if (Players[p]._pBaseStr < 0) {
				Players[p]._pBaseStr = 0;
			}
			break;
		case ATTRIB_MAG:
			if (Players[p]._pBaseMag > MaxStats[c][ATTRIB_MAG]) {
				Players[p]._pBaseMag = MaxStats[c][ATTRIB_MAG];
			} else if (Players[p]._pBaseMag < 0) {
				Players[p]._pBaseMag = 0;
			}
			break;
		case ATTRIB_DEX:
			if (Players[p]._pBaseDex > MaxStats[c][ATTRIB_DEX]) {
				Players[p]._pBaseDex = MaxStats[c][ATTRIB_DEX];
			} else if (Players[p]._pBaseDex < 0) {
				Players[p]._pBaseDex = 0;
			}
			break;
		case ATTRIB_VIT:
			if (Players[p]._pBaseVit > MaxStats[c][ATTRIB_VIT]) {
				Players[p]._pBaseVit = MaxStats[c][ATTRIB_VIT];
			} else if (Players[p]._pBaseVit < 0) {
				Players[p]._pBaseVit = 0;
			}
			break;
		}
	}
}

void ModifyPlrStr(int p, int l)
{
	int max;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrStr: illegal player %d", p);
	}

	max = MaxStats[Players[p]._pClass][ATTRIB_STR];
	if (Players[p]._pBaseStr + l > max) {
		l = max - Players[p]._pBaseStr;
	}

	Players[p]._pStrength += l;
	Players[p]._pBaseStr += l;

#ifndef HELLFIRE
	if (Players[p]._pClass == PC_ROGUE) {
		Players[p]._pDamageMod = Players[p]._pLevel * (Players[p]._pStrength + Players[p]._pDexterity) / 200;
	} else {
		Players[p]._pDamageMod = Players[p]._pLevel * Players[p]._pStrength / 100;
	}
#endif

	CalcPlrInv(p, TRUE);

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETSTR, Players[p]._pBaseStr);
	}
}

void ModifyPlrMag(int p, int l)
{
	int max, ms;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrMag: illegal player %d", p);
	}

	max = MaxStats[Players[p]._pClass][ATTRIB_MAG];
	if (Players[p]._pBaseMag + l > max) {
		l = max - Players[p]._pBaseMag;
	}

	Players[p]._pMagic += l;
	Players[p]._pBaseMag += l;

	ms = l << 6;
	if (Players[p]._pClass == PC_SORCERER) {
		ms <<= 1;
	}
#ifdef HELLFIRE
	else if (Players[p]._pClass == PC_BARD) {
		ms += ms >> 1;
	}
#endif

	Players[p]._pMaxManaBase += ms;
	Players[p]._pMaxMana += ms;
	if (!(Players[p]._pIFlags & ISPL_NOMANA)) {
		Players[p]._pManaBase += ms;
		Players[p]._pMana += ms;
	}

	CalcPlrInv(p, TRUE);

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETMAG, Players[p]._pBaseMag);
	}
}

void ModifyPlrDex(int p, int l)
{
	int max;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrDex: illegal player %d", p);
	}

	max = MaxStats[Players[p]._pClass][ATTRIB_DEX];
	if (Players[p]._pBaseDex + l > max) {
		l = max - Players[p]._pBaseDex;
	}

	Players[p]._pDexterity += l;
	Players[p]._pBaseDex += l;
	CalcPlrInv(p, TRUE);

#ifndef HELLFIRE
	if (Players[p]._pClass == PC_ROGUE) {
		Players[p]._pDamageMod = Players[p]._pLevel * (Players[p]._pDexterity + Players[p]._pStrength) / 200;
	}
#endif

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETDEX, Players[p]._pBaseDex);
	}
}

void ModifyPlrVit(int p, int l)
{
	int max, ms;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrVit: illegal player %d", p);
	}

	max = MaxStats[Players[p]._pClass][ATTRIB_VIT];
	if (Players[p]._pBaseVit + l > max) {
		l = max - Players[p]._pBaseVit;
	}

	Players[p]._pVitality += l;
	Players[p]._pBaseVit += l;

	ms = l << 6;
	if (Players[p]._pClass == PC_WARRIOR) {
		ms <<= 1;
#ifdef HELLFIRE
	} else if (Players[p]._pClass == PC_BARBARIAN) {
		ms <<= 1;
#endif
	}

	Players[p]._pHPBase += ms;
	Players[p]._pMaxHPBase += ms;
	Players[p]._pHitPoints += ms;
	Players[p]._pMaxHP += ms;

	CalcPlrInv(p, TRUE);

	if (p == myplr) {
		NetSendCmdParam1(FALSE, CMD_SETVIT, Players[p]._pBaseVit);
	}
}

void SetPlayerHitPoints(int pnum, int val)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlayerHitPoints: illegal player %d", pnum);
	}

	Players[pnum]._pHitPoints = val;
	Players[pnum]._pHPBase = val + Players[pnum]._pMaxHPBase - Players[pnum]._pMaxHP;

	if (pnum == myplr) {
		drawhpflag = TRUE;
	}
}

void SetPlrStr(int p, int v)
{
	int dm;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrStr: illegal player %d", p);
	}

	Players[p]._pBaseStr = v;
	CalcPlrInv(p, TRUE);

#ifndef HELLFIRE
	if (Players[p]._pClass == PC_ROGUE) {
		dm = Players[p]._pLevel * (Players[p]._pStrength + Players[p]._pDexterity) / 200;
	} else {
		dm = Players[p]._pLevel * Players[p]._pStrength / 100;
	}

	Players[p]._pDamageMod = dm;
#endif
}

void SetPlrMag(int p, int v)
{
	int m;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrMag: illegal player %d", p);
	}

	Players[p]._pBaseMag = v;

	m = v << 6;
	if (Players[p]._pClass == PC_SORCERER) {
		m <<= 1;
#ifdef HELLFIRE
	} else if (Players[p]._pClass == PC_BARD) {
		m += m >> 1;
#endif
	}

	Players[p]._pMaxManaBase = m;
	Players[p]._pMaxMana = m;
	CalcPlrInv(p, TRUE);
}

void SetPlrDex(int p, int v)
{
	int dm;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrDex: illegal player %d", p);
	}

	Players[p]._pBaseDex = v;
	CalcPlrInv(p, TRUE);

#ifndef HELLFIRE
	if (Players[p]._pClass == PC_ROGUE) {
		dm = Players[p]._pLevel * (Players[p]._pStrength + Players[p]._pDexterity) / 200;
	} else {
		dm = Players[p]._pStrength * Players[p]._pLevel / 100;
	}

	Players[p]._pDamageMod = dm;
#endif
}

void SetPlrVit(int p, int v)
{
	int hp;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrVit: illegal player %d", p);
	}

	Players[p]._pBaseVit = v;

	hp = v << 6;
	if (Players[p]._pClass == PC_WARRIOR) {
		hp <<= 1;
	}
#ifdef HELLFIRE
	else if (Players[p]._pClass == PC_BARBARIAN) {
		hp <<= 1;
	}
#endif

	Players[p]._pHPBase = hp;
	Players[p]._pMaxHPBase = hp;
	CalcPlrInv(p, TRUE);
}

void InitDungMsgs(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitDungMsgs: illegal player %d", pnum);
	}

	Players[pnum].pDungMsgs = 0;
#ifdef HELLFIRE
	Players[pnum].pDungMsgs2 = 0;
#endif
}

void PlayDungMsgs()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("PlayDungMsgs: illegal player %d", myplr);
	}

	if (currlevel == 1 && !Players[myplr]._pLvlVisited[1] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs & DMSG_CATHEDRAL)) {
		sfxdelay = 40;
		if (Players[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR97;
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE97;
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE97;
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK97;
		} else if (Players[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE97;
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR97;
#endif
#endif
		}
		Players[myplr].pDungMsgs = Players[myplr].pDungMsgs | DMSG_CATHEDRAL;
	} else if (currlevel == 5 && !Players[myplr]._pLvlVisited[5] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs & DMSG_CATACOMBS)) {
		sfxdelay = 40;
		if (Players[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR96B;
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE96;
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE96;
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK96;
		} else if (Players[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE96;
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR96B;
#endif
#endif
		}
		Players[myplr].pDungMsgs |= DMSG_CATACOMBS;
	} else if (currlevel == 9 && !Players[myplr]._pLvlVisited[9] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs & DMSG_CAVES)) {
		sfxdelay = 40;
		if (Players[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR98;
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE98;
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE98;
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK98;
		} else if (Players[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE98;
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR98;
#endif
#endif
		}
		Players[myplr].pDungMsgs |= DMSG_CAVES;
	} else if (currlevel == 13 && !Players[myplr]._pLvlVisited[13] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs & DMSG_HELL)) {
		sfxdelay = 40;
		if (Players[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR99;
#ifndef SPAWN
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE99;
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE99;
#ifdef HELLFIRE
		} else if (Players[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK99;
		} else if (Players[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE99;
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR99;
#endif
#endif
		}
		Players[myplr].pDungMsgs |= DMSG_HELL;
	} else if (currlevel == 16 && !Players[myplr]._pLvlVisited[15] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs & DMSG_DIABLO)) { // BUGFIX: _pLvlVisited should check 16 or this message will never play
		sfxdelay = 40;
#ifndef SPAWN
#ifdef HELLFIRE
		if (Players[myplr]._pClass == PC_WARRIOR || Players[myplr]._pClass == PC_ROGUE || Players[myplr]._pClass == PC_SORCERER || Players[myplr]._pClass == PC_MONK || Players[myplr]._pClass == PC_BARD || Players[myplr]._pClass == PC_BARBARIAN) {
#else
		if (Players[myplr]._pClass == PC_WARRIOR || Players[myplr]._pClass == PC_ROGUE || Players[myplr]._pClass == PC_SORCERER) {
#endif
			sfxdnum = PS_DIABLVLINT;
		}
#endif
		Players[myplr].pDungMsgs |= DMSG_DIABLO;
#ifdef HELLFIRE
	} else if (currlevel == 17 && !Players[myplr]._pLvlVisited[17] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs2 & 1)) {
		sfxdelay = 10;
		sfxdnum = USFX_DEFILER1;
		quests[Q_DEFILER]._qactive = QUEST_ACTIVE;
		quests[Q_DEFILER]._qlog = TRUE;
		quests[Q_DEFILER]._qmsg = 286;
		Players[myplr].pDungMsgs2 |= 1;
	} else if (currlevel == 19 && !Players[myplr]._pLvlVisited[19] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs2 & 4)) {
		sfxdelay = 10;
		sfxdnum = USFX_DEFILER3;
		Players[myplr].pDungMsgs2 |= 4;
	} else if (currlevel == 21 && !Players[myplr]._pLvlVisited[21] && gbMaxPlayers == 1 && !(Players[myplr].pDungMsgs & 32)) {
		sfxdelay = 30;
#ifndef SPAWN
		if (Players[myplr]._pClass == PC_WARRIOR) {
			sfxdnum = PS_WARR92;
		} else if (Players[myplr]._pClass == PC_ROGUE) {
			sfxdnum = PS_ROGUE92;
		} else if (Players[myplr]._pClass == PC_SORCERER) {
			sfxdnum = PS_MAGE92;
		} else
#endif
		    if (Players[myplr]._pClass == PC_MONK) {
			sfxdnum = PS_MONK92;
		}
#ifndef SPAWN
		else if (Players[myplr]._pClass == PC_BARD) {
			sfxdnum = PS_ROGUE92;
		} else if (Players[myplr]._pClass == PC_BARBARIAN) {
			sfxdnum = PS_WARR92;
		}
#endif
		Players[myplr].pDungMsgs |= 32;
#endif
	} else {
		sfxdelay = 0;
	}
}

#ifdef HELLFIRE
int get_max_strength(int i)
{
	return MaxStats[i][ATTRIB_STR];
}

int get_max_magic(int i)
{
	return MaxStats[i][ATTRIB_MAG];
}

int get_max_dexterity(int i)
{
	return MaxStats[i][ATTRIB_DEX];
}
#endif
