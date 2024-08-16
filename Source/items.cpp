/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "all.h"
#ifdef HELLFIRE
#include "../3rdParty/Storm/Source/storm.h"
#endif

int itemactive[MAXITEMS];
BOOL uitemflag;
int itemavail[MAXITEMS];
ItemStruct curruitem;
ItemGetRecordStruct itemrecord[MAXITEMS];
/** Contains the items on ground in the current game. */
ItemStruct item[MAXITEMS + 1];
BOOL itemhold[3][3];
#ifdef HELLFIRE
CornerStoneStruct CornerStone;
#endif
BYTE *itemanims[ITEMTYPES];
BOOL UniqueItemFlag[128];
#ifdef HELLFIRE
int auricGold = GOLD_MAX_LIMIT * 2;
#endif
int numitems;
int gnNumGetRecords;
ItemStruct golditem;

/* data */

#ifdef HELLFIRE
int OilLevels[] = { 1, 10, 1, 10, 4, 1, 5, 17, 1, 10 };
int OilValues[] = { 500, 2500, 500, 2500, 1500, 100, 2500, 15000, 500, 2500 };
int OilMagic[] = {
	IMISC_OILACC,
	IMISC_OILMAST,
	IMISC_OILSHARP,
	IMISC_OILDEATH,
	IMISC_OILSKILL,
	IMISC_OILBSMTH,
	IMISC_OILFORT,
	IMISC_OILPERM,
	IMISC_OILHARD,
	IMISC_OILIMP,
};
char OilNames[10][25] = {
	"Oil of Accuracy",
	"Oil of Mastery",
	"Oil of Sharpness",
	"Oil of Death",
	"Oil of Skill",
	"Blacksmith Oil",
	"Oil of Fortitude",
	"Oil of Permanence",
	"Oil of Hardening",
	"Oil of Imperviousness"
};
int MaxGold = GOLD_MAX_LIMIT;
#endif

/** Maps from item_cursor_graphic to in-memory item type. */
BYTE ItemCAnimTbl[] = {
#ifndef HELLFIRE
	20, 16, 16, 16, 4, 4, 4, 12, 12, 12,
	12, 12, 12, 12, 12, 21, 21, 25, 12, 28,
	28, 28, 0, 0, 0, 32, 0, 0, 0, 24,
	24, 26, 2, 25, 22, 23, 24, 25, 27, 27,
	29, 0, 0, 0, 12, 12, 12, 12, 12, 0,
	8, 8, 0, 8, 8, 8, 8, 8, 8, 6,
	8, 8, 8, 6, 8, 8, 6, 8, 8, 6,
	6, 6, 8, 8, 8, 5, 9, 13, 13, 13,
	5, 5, 5, 15, 5, 5, 18, 18, 18, 30,
	5, 5, 14, 5, 14, 13, 16, 18, 5, 5,
	7, 1, 3, 17, 1, 15, 10, 14, 3, 11,
	8, 0, 1, 7, 0, 7, 15, 7, 3, 3,
	3, 6, 6, 11, 11, 11, 31, 14, 14, 14,
	6, 6, 7, 3, 8, 14, 0, 14, 14, 0,
	33, 1, 1, 1, 1, 1, 7, 7, 7, 14,
	14, 17, 17, 17, 0, 34, 1, 0, 3, 17,
	8, 8, 6, 1, 3, 3, 11, 3, 4
#else
	20, 16, 16, 16, 4, 4, 4, 12, 12, 12,
	12, 12, 12, 12, 12, 21, 21, 25, 12, 28,
	28, 28, 38, 38, 38, 32, 38, 38, 38, 24,
	24, 26, 2, 25, 22, 23, 24, 25, 27, 27,
	29, 0, 0, 0, 12, 12, 12, 12, 12, 0,
	8, 8, 0, 8, 8, 8, 8, 8, 8, 6,
	8, 8, 8, 6, 8, 8, 6, 8, 8, 6,
	6, 6, 8, 8, 8, 5, 9, 13, 13, 13,
	5, 5, 5, 15, 5, 5, 18, 18, 18, 30,
	5, 5, 14, 5, 14, 13, 16, 18, 5, 5,
	7, 1, 3, 17, 1, 15, 10, 14, 3, 11,
	8, 0, 1, 7, 0, 7, 15, 7, 3, 3,
	3, 6, 6, 11, 11, 11, 31, 14, 14, 14,
	6, 6, 7, 3, 8, 14, 0, 14, 14, 0,
	33, 1, 1, 1, 1, 1, 7, 7, 7, 14,
	14, 17, 17, 17, 0, 34, 1, 0, 3, 17,
	8, 8, 6, 1, 3, 3, 11, 3, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 35, 39, 36,
	36, 36, 37, 38, 38, 38, 38, 38, 41, 42,
	8, 8, 8, 17, 0, 6, 8, 11, 11, 3,
	3, 1, 6, 6, 6, 1, 8, 6, 11, 3,
	6, 8, 1, 6, 6, 17, 40, 0, 0
#endif
};
/** Map of item type .cel file names. */
const char *const ItemDropNames[] = {
	"Armor2",
	"Axe",
	"FBttle",
	"Bow",
	"GoldFlip",
	"Helmut",
	"Mace",
	"Shield",
	"SwrdFlip",
	"Rock",
	"Cleaver",
	"Staff",
	"Ring",
	"CrownF",
	"LArmor",
	"WShield",
	"Scroll",
	"FPlateAr",
	"FBook",
	"Food",
	"FBttleBB",
	"FBttleDY",
	"FBttleOR",
	"FBttleBR",
	"FBttleBL",
	"FBttleBY",
	"FBttleWH",
	"FBttleDB",
	"FEar",
	"FBrain",
	"FMush",
	"Innsign",
	"Bldstn",
	"Fanvil",
	"FLazStaf",
#ifdef HELLFIRE
	"bombs1",
	"halfps1",
	"wholeps1",
	"runes1",
	"teddys1",
	"cows1",
	"donkys1",
	"mooses1",
#endif
};
/** Maps of item drop animation length. */
BYTE ItemAnimLs[] = {
	15,
	13,
	16,
	13,
	10,
	13,
	13,
	13,
	13,
	10,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	1,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	13,
	12,
	12,
	13,
	13,
	13,
	8,
#ifdef HELLFIRE
	10,
	16,
	16,
	10,
	10,
	15,
	15,
	15,
#endif
};
/** Maps of drop sounds effect of dropping the item on ground. */
int ItemDropSnds[] = {
	IS_FHARM,
	IS_FAXE,
	IS_FPOT,
	IS_FBOW,
	IS_GOLD,
	IS_FCAP,
	IS_FSWOR,
	IS_FSHLD,
	IS_FSWOR,
	IS_FROCK,
	IS_FAXE,
	IS_FSTAF,
	IS_FRING,
	IS_FCAP,
	IS_FLARM,
	IS_FSHLD,
	IS_FSCRL,
	IS_FHARM,
	IS_FBOOK,
	IS_FLARM,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FBODY,
	IS_FBODY,
	IS_FMUSH,
	IS_ISIGN,
	IS_FBLST,
	IS_FANVL,
	IS_FSTAF,
#ifdef HELLFIRE
	IS_FROCK,
	IS_FSCRL,
	IS_FSCRL,
	IS_FROCK,
	IS_FMUSH,
	IS_FHARM,
	IS_FLARM,
	IS_FLARM,
#endif
};
/** Maps of drop sounds effect of placing the item in the inventory. */
int ItemInvSnds[] = {
	IS_IHARM,
	IS_IAXE,
	IS_IPOT,
	IS_IBOW,
	IS_GOLD,
	IS_ICAP,
	IS_ISWORD,
	IS_ISHIEL,
	IS_ISWORD,
	IS_IROCK,
	IS_IAXE,
	IS_ISTAF,
	IS_IRING,
	IS_ICAP,
	IS_ILARM,
	IS_ISHIEL,
	IS_ISCROL,
	IS_IHARM,
	IS_IBOOK,
	IS_IHARM,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IBODY,
	IS_IBODY,
	IS_IMUSH,
	IS_ISIGN,
	IS_IBLST,
	IS_IANVL,
	IS_ISTAF,
#ifdef HELLFIRE
	IS_IROCK,
	IS_ISCROL,
	IS_ISCROL,
	IS_IROCK,
	IS_IMUSH,
	IS_IHARM,
	IS_ILARM,
	IS_ILARM,
#endif
};
#ifdef HELLFIRE
char *CornerStoneRegKey = "SItem";
#endif
/** Specifies the current Y-coordinate used for validation of items on ground. */
int idoppely = 16;
/** Maps from Griswold premium item number to a quality level delta as added to the base quality level. */
int premiumlvladd[SMITH_PREMIUM_ITEMS] = {
	// clang-format off
	-1,
	-1,
#ifdef HELLFIRE
	-1,
#endif
	 0,
	 0,
#ifdef HELLFIRE
	 0,
	 0,
	 1,
	 1,
	 1,
#endif
	 1,
	 2,
#ifdef HELLFIRE
	 2,
	 3,
	 3,
#endif
	// clang-format on
};

BOOL ItemPlace(int xp, int yp)
{
	if (dMonster[xp][yp] != 0)
		return FALSE;
	if (dPlayer[xp][yp] != 0)
		return FALSE;
	if (dItem[xp][yp] != 0)
		return FALSE;
	if (dObject[xp][yp] != 0)
		return FALSE;
	if (dFlags[xp][yp] & BFLAG_POPULATED)
		return FALSE;
	if (nSolidTable[dPiece[xp][yp]])
		return FALSE;

	return TRUE;
}

void AddInitItems()
{
	int x, y, i, j, rnd;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	rnd = random_(11, 3) + 3;
	for (j = 0; j < rnd; j++) {
		i = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = i;
		x = random_(12, 80) + 16;
		y = random_(12, 80) + 16;
		while (!ItemPlace(x, y)) {
			x = random_(12, 80) + 16;
			y = random_(12, 80) + 16;
		}
		item[i]._ix = x;
		item[i]._iy = y;
		dItem[x][y] = i + 1;
		item[i]._iSeed = GetRndSeed();
		SetRndSeed(item[i]._iSeed);
#ifdef HELLFIRE
		if (random_(12, 2) != 0)
			GetItemAttrs(i, IDI_HEAL, curlv);
		else
			GetItemAttrs(i, IDI_MANA, curlv);
		item[i]._iCreateInfo = curlv + CF_PREGEN;
#else
		if (random_(12, 2) != 0)
			GetItemAttrs(i, IDI_HEAL, currlevel);
		else
			GetItemAttrs(i, IDI_MANA, currlevel);
		item[i]._iCreateInfo = currlevel + CF_PREGEN;
#endif
		SetupItem(i);
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 1;
		numitems++;
	}
}

void InitItems()
{
	int i;
	long s;

	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = item[0];
	golditem._iStatFlag = TRUE;
	numitems = 0;

	for (i = 0; i < MAXITEMS; i++) {
		item[i]._itype = ITYPE_MISC; // BUGFIX Should be ITYPE_NONE
		item[i]._ix = 0;
		item[i]._iy = 0;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 0;
		item[i]._iIdentified = FALSE;
		item[i]._iPostDraw = FALSE;
	}

	for (i = 0; i < MAXITEMS; i++) {
		itemavail[i] = i;
		itemactive[i] = 0;
	}

	if (!setlevel) {
		s = GetRndSeed(); /* unused */
		if (QuestStatus(Q_ROCK))
			SpawnRock();
		if (QuestStatus(Q_ANVIL))
			SpawnQuestItem(IDI_ANVIL, 2 * setpc_x + 27, 2 * setpc_y + 27, 0, 1);
#ifdef HELLFIRE
		if (UseCowFarmer && currlevel == 20)
			SpawnQuestItem(IDI_BROWNSUIT, 25, 25, 3, 1);
		if (UseCowFarmer && currlevel == 19)
			SpawnQuestItem(IDI_GREYSUIT, 25, 25, 3, 1);
#endif
		if (currlevel > 0 && currlevel < 16)
			AddInitItems();
#ifdef HELLFIRE
		if (currlevel >= 21 && currlevel <= 23)
			SpawnNote();
#endif
	}

	uitemflag = FALSE;

	// BUGFIX: item get records not reset when resetting items.
}

void GetPlrHandSeed(ItemStruct *h)
{
	h->_iSeed = GetRndSeed();
}

void GetGoldSeed(int pnum, ItemStruct *h)
{
	int i, ii, s;
	BOOL doneflag;

	do {
		doneflag = TRUE;
		s = GetRndSeed();
		for (i = 0; i < numitems; i++) {
			ii = itemactive[i];
			if (item[ii]._iSeed == s)
				doneflag = FALSE;
		}
		if (pnum == myplr) {
			for (i = 0; i < plr[pnum]._pNumInv; i++) {
				if (plr[pnum].InvList[i]._iSeed == s)
					doneflag = FALSE;
			}
		}
	} while (!doneflag);

	h->_iSeed = s;
}

void SetPlrHandSeed(ItemStruct *h, int iseed)
{
	h->_iSeed = iseed;
}

void SetPlrHandGoldCurs(ItemStruct *h)
{
	if (h->_ivalue >= GOLD_MEDIUM_LIMIT)
		h->_iCurs = ICURS_GOLD_LARGE;
	else if (h->_ivalue <= GOLD_SMALL_LIMIT)
		h->_iCurs = ICURS_GOLD_SMALL;
	else
		h->_iCurs = ICURS_GOLD_MEDIUM;
}

BOOL ItemSpaceOk(int i, int j)
{
	int oi;

	if (i < 0 || i >= MAXDUNX || j < 0 || j >= MAXDUNY)
		return FALSE;

	if (dMonster[i][j] != 0)
		return FALSE;

	if (dPlayer[i][j] != 0)
		return FALSE;

	if (dItem[i][j] != 0)
		return FALSE;

	if (dObject[i][j] != 0) {
		oi = dObject[i][j] > 0 ? dObject[i][j] - 1 : -(dObject[i][j] + 1);
		if (object[oi]._oSolidFlag)
			return FALSE;
	}

	if (dObject[i + 1][j + 1] > 0 && object[dObject[i + 1][j + 1] - 1]._oSelFlag != 0) /// BUGFIX: check for dObject OOB
		return FALSE;

	if (dObject[i + 1][j + 1] < 0 && object[-(dObject[i + 1][j + 1] + 1)]._oSelFlag != 0) /// BUGFIX: check for dObject OOB
		return FALSE;

	if (dObject[i + 1][j] > 0 /// BUGFIX: check for dObject OOB
	    && dObject[i][j + 1] > 0 /// BUGFIX: check for dObject OOB
	    && object[dObject[i + 1][j] - 1]._oSelFlag != 0
	    && object[dObject[i][j + 1] - 1]._oSelFlag != 0) {
		return FALSE;
	}

	return !nSolidTable[dPiece[i][j]];
}

BOOL GetItemSpace(int x, int y, char inum)
{
	int i, j, rs;
	int xx, yy;
	BOOL savail;

	yy = 0;
	for (j = y - 1; j <= y + 1; j++) {
		xx = 0;
		for (i = x - 1; i <= x + 1; i++) {
			itemhold[xx][yy] = ItemSpaceOk(i, j);
			xx++;
		}
		yy++;
	}

	savail = FALSE;
	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
			if (itemhold[i][j])
				savail = TRUE;
		}
	}

	rs = random_(13, 15) + 1;

	if (!savail)
		return FALSE;

	xx = 0;
	yy = 0;
	while (rs > 0) {
		if (itemhold[xx][yy])
			rs--;
		if (rs > 0) {
			xx++;
			if (xx == 3) {
				xx = 0;
				yy++;
				if (yy == 3)
					yy = 0;
			}
		}
	}

	xx += x - 1;
	yy += y - 1;
	item[inum]._ix = xx;
	item[inum]._iy = yy;
	dItem[xx][yy] = inum + 1;

	return TRUE;
}

void GetSuperItemSpace(int x, int y, char inum)
{
	int xx, yy;
	int i, j, k;

	if (!GetItemSpace(x, y, inum)) {
		for (k = 2; k < 50; k++) {
			for (j = -k; j <= k; j++) {
				yy = y + j;
				for (i = -k; i <= k; i++) {
					xx = i + x;
					if (ItemSpaceOk(xx, yy)) {
						item[inum]._ix = xx;
						item[inum]._iy = yy;
						dItem[xx][yy] = inum + 1;
						return;
					}
				}
			}
		}
	}
}

void GetSuperItemLoc(int x, int y, int &xx, int &yy)
{
	int i, j, k;

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			yy = y + j;
			for (i = -k; i <= k; i++) {
				xx = i + x;
				if (ItemSpaceOk(xx, yy)) {
					return;
				}
			}
		}
	}
}

void CalcItemValue(int i)
{
	int v;

	v = item[i]._iVMult1 + item[i]._iVMult2;
	if (v > 0) {
		v *= item[i]._ivalue;
	}
	if (v < 0) {
		v = item[i]._ivalue / v;
	}
	v = item[i]._iVAdd1 + item[i]._iVAdd2 + v;
	if (v <= 0) {
		v = 1;
	}
	item[i]._iIvalue = v;
}

void GetBookSpell(int i, int lvl)
{
	int rv, s, bs;

	if (lvl == 0)
		lvl = 1;
	rv = random_(14, MAX_SPELLS) + 1;
#ifdef SPAWN
	if (lvl > 5)
		lvl = 5;
#endif
	s = SPL_FIREBOLT;
#ifdef HELLFIRE
	bs = SPL_FIREBOLT;
#endif
	while (rv > 0) {
		if (spelldata[s].sBookLvl != -1 && lvl >= spelldata[s].sBookLvl) {
			rv--;
			bs = s;
		}
		s++;
		if (gbMaxPlayers == 1) {
			if (s == SPL_RESURRECT)
				s = SPL_TELEKINESIS;
		}
		if (gbMaxPlayers == 1) {
			if (s == SPL_HEALOTHER)
				s = SPL_FLARE;
		}
		if (s == MAX_SPELLS)
			s = 1;
	}
	strcat(item[i]._iName, spelldata[bs].sNameText);
	strcat(item[i]._iIName, spelldata[bs].sNameText);
	item[i]._iSpell = bs;
	item[i]._iMinMag = spelldata[bs].sMinInt;
	item[i]._ivalue += spelldata[bs].sBookCost;
	item[i]._iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == STYPE_FIRE)
		item[i]._iCurs = ICURS_BOOK_RED;
#ifdef HELLFIRE
	else
#endif
	    if (spelldata[bs].sType == STYPE_LIGHTNING)
		item[i]._iCurs = ICURS_BOOK_BLUE;
#ifdef HELLFIRE
	else
#endif
	    if (spelldata[bs].sType == STYPE_MAGIC)
		item[i]._iCurs = ICURS_BOOK_GREY;
}

void GetStaffPower(int i, int lvl, int bs, BOOL onlygood)
{
	int l[256];
	char istr[128];
	int nl, j, preidx;
	BOOL addok;
	int tmp;

	tmp = random_(15, 10);
	preidx = -1;
	if (tmp == 0 || onlygood) {
		nl = 0;
		for (j = 0; PL_Prefix[j].PLPower != IPL_INVALID; j++) {
			if (PL_Prefix[j].PLIType & PLT_STAFF && PL_Prefix[j].PLMinLvl <= lvl) {
				addok = TRUE;
				if (onlygood && !PL_Prefix[j].PLOk)
					addok = FALSE;
				if (addok) {
					l[nl] = j;
					nl++;
					if (PL_Prefix[j].PLDouble) {
						l[nl] = j;
						nl++;
					}
				}
			}
		}
		if (nl != 0) {
			preidx = l[random_(16, nl)];
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			item[i]._iPrePower = PL_Prefix[preidx].PLPower;
		}
	}
	CalcItemValue(i);
}

void GetStaffSpell(int i, int lvl, BOOL onlygood)
{
	int l, rv, s, minc, maxc, v, bs;
	char istr[64];

#ifndef HELLFIRE
	if (random_(17, 4) == 0) {
		GetItemPower(i, lvl >> 1, lvl, PLT_STAFF, onlygood);
	} else
#endif
	{
		l = lvl >> 1;
		if (l == 0)
			l = 1;
		rv = random_(18, MAX_SPELLS) + 1;
#ifdef SPAWN
		if (lvl > 10)
			lvl = 10;
#endif
		s = SPL_FIREBOLT;
		while (rv > 0) {
			if (spelldata[s].sStaffLvl != -1 && l >= spelldata[s].sStaffLvl) {
				rv--;
				bs = s;
			}
			s++;
			if (gbMaxPlayers == 1 && s == SPL_RESURRECT)
				s = SPL_TELEKINESIS;
			if (gbMaxPlayers == 1 && s == SPL_HEALOTHER)
				s = SPL_FLARE;
			if (s == MAX_SPELLS)
				s = SPL_FIREBOLT;
		}
		sprintf(istr, "%s of %s", item[i]._iName, spelldata[bs].sNameText);
		strcpy(item[i]._iName, istr);
		strcpy(item[i]._iIName, istr);

		minc = spelldata[bs].sStaffMin;
		maxc = spelldata[bs].sStaffMax - minc + 1;
		item[i]._iSpell = bs;
		item[i]._iCharges = minc + random_(19, maxc);
		item[i]._iMaxCharges = item[i]._iCharges;

		item[i]._iMinMag = spelldata[bs].sMinInt;
		v = item[i]._iCharges * spelldata[bs].sStaffCost / 5;
		item[i]._ivalue += v;
		item[i]._iIvalue += v;
		GetStaffPower(i, lvl, bs, onlygood);
	}
}

void GetItemAttrs(int i, int idata, int lvl)
{
	int rndv;
#ifdef HELLFIRE
	int itemlevel;
#endif

	item[i]._itype = AllItemsList[idata].itype;
	item[i]._iCurs = AllItemsList[idata].iCurs;
	strcpy(item[i]._iName, AllItemsList[idata].iName);
	strcpy(item[i]._iIName, AllItemsList[idata].iName);
	item[i]._iLoc = AllItemsList[idata].iLoc;
	item[i]._iClass = AllItemsList[idata].iClass;
	item[i]._iMinDam = AllItemsList[idata].iMinDam;
	item[i]._iMaxDam = AllItemsList[idata].iMaxDam;
	item[i]._iAC = AllItemsList[idata].iMinAC + random_(20, AllItemsList[idata].iMaxAC - AllItemsList[idata].iMinAC + 1);
#ifndef HELLFIRE
	item[i]._iFlags = AllItemsList[idata].iFlags;
#endif
	item[i]._iMiscId = AllItemsList[idata].iMiscId;
	item[i]._iSpell = AllItemsList[idata].iSpell;
	item[i]._iMagical = ITEM_QUALITY_NORMAL;
	item[i]._ivalue = AllItemsList[idata].iValue;
	item[i]._iIvalue = AllItemsList[idata].iValue;
	item[i]._iVAdd1 = 0;
	item[i]._iVMult1 = 0;
	item[i]._iVAdd2 = 0;
	item[i]._iVMult2 = 0;
	item[i]._iPLDam = 0;
	item[i]._iPLToHit = 0;
	item[i]._iPLAC = 0;
	item[i]._iPLStr = 0;
	item[i]._iPLMag = 0;
	item[i]._iPLDex = 0;
	item[i]._iPLVit = 0;
	item[i]._iCharges = 0;
	item[i]._iMaxCharges = 0;
	item[i]._iDurability = AllItemsList[idata].iDurability;
	item[i]._iMaxDur = AllItemsList[idata].iDurability;
	item[i]._iMinStr = AllItemsList[idata].iMinStr;
	item[i]._iMinMag = AllItemsList[idata].iMinMag;
	item[i]._iMinDex = AllItemsList[idata].iMinDex;
	item[i]._iPLFR = 0;
	item[i]._iPLLR = 0;
	item[i]._iPLMR = 0;
	item[i].IDidx = idata;
	item[i]._iPLDamMod = 0;
	item[i]._iPLGetHit = 0;
	item[i]._iPLLight = 0;
	item[i]._iSplLvlAdd = 0;
	item[i]._iRequest = FALSE;
	item[i]._iFMinDam = 0;
	item[i]._iFMaxDam = 0;
	item[i]._iLMinDam = 0;
	item[i]._iLMaxDam = 0;
	item[i]._iPLEnAc = 0;
	item[i]._iPLMana = 0;
	item[i]._iPLHP = 0;
	item[i]._iPrePower = -1;
	item[i]._iSufPower = -1;

#ifndef HELLFIRE
	if (item[i]._iMiscId == IMISC_BOOK)
		GetBookSpell(i, lvl);

	if (item[i]._itype == ITYPE_GOLD) {
		if (gnDifficulty == DIFF_NORMAL)
			rndv = 5 * currlevel + random_(21, 10 * currlevel);
		if (gnDifficulty == DIFF_NIGHTMARE)
			rndv = 5 * (currlevel + 16) + random_(21, 10 * (currlevel + 16));
		if (gnDifficulty == DIFF_HELL)
			rndv = 5 * (currlevel + 32) + random_(21, 10 * (currlevel + 32));
#else
	item[i]._iFlags = 0;
	item[i]._iDamAcFlags = 0;

	if (item[i]._iMiscId == IMISC_BOOK)
		GetBookSpell(i, lvl);

	if (item[i]._iMiscId == IMISC_OILOF)
		GetOilType(i, lvl);

	itemlevel = items_get_currlevel();
	if (item[i]._itype == ITYPE_GOLD) {
		if (gnDifficulty == DIFF_NORMAL)
			rndv = 5 * itemlevel + random_(21, 10 * itemlevel);
		else if (gnDifficulty == DIFF_NIGHTMARE)
			rndv = 5 * (itemlevel + 16) + random_(21, 10 * (itemlevel + 16));
		else if (gnDifficulty == DIFF_HELL)
			rndv = 5 * (itemlevel + 32) + random_(21, 10 * (itemlevel + 32));
#endif
		if (leveltype == DTYPE_HELL)
			rndv += rndv >> 3;
		if (rndv > GOLD_MAX_LIMIT)
			rndv = GOLD_MAX_LIMIT;

		item[i]._ivalue = rndv;

		if (rndv >= GOLD_MEDIUM_LIMIT)
			item[i]._iCurs = ICURS_GOLD_LARGE;
		else
			item[i]._iCurs = (rndv > GOLD_SMALL_LIMIT) + 4;
	}
}

int RndPL(int param1, int param2)
{
	return param1 + random_(22, param2 - param1 + 1);
}

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2)
		return minv;
	if (minv == maxv)
		return minv;
	return minv + (maxv - minv) * (100 * (pv - p1) / (p2 - p1)) / 100;
}

void SaveItemPower(int i, int power, int param1, int param2, int minval, int maxval, int multval)
{
	int r, r2;

	r = RndPL(param1, param2);
	switch (power) {
	case IPL_TOHIT:
		item[i]._iPLToHit += r;
		break;
	case IPL_TOHIT_CURSE:
		item[i]._iPLToHit -= r;
		break;
	case IPL_DAMP:
		item[i]._iPLDam += r;
		break;
	case IPL_DAMP_CURSE:
		item[i]._iPLDam -= r;
		break;
#ifdef HELLFIRE
	case IPL_DOPPELGANGER:
		item[i]._iDamAcFlags |= ISPLHF_DOPPELGANGER;
		// no break
#endif
	case IPL_TOHIT_DAMP:
		r = RndPL(param1, param2);
		item[i]._iPLDam += r;
		if (param1 == 20)
			r2 = RndPL(1, 5);
		if (param1 == 36)
			r2 = RndPL(6, 10);
		if (param1 == 51)
			r2 = RndPL(11, 15);
		if (param1 == 66)
			r2 = RndPL(16, 20);
		if (param1 == 81)
			r2 = RndPL(21, 30);
		if (param1 == 96)
			r2 = RndPL(31, 40);
		if (param1 == 111)
			r2 = RndPL(41, 50);
		if (param1 == 126)
			r2 = RndPL(51, 75);
		if (param1 == 151)
			r2 = RndPL(76, 100);
		item[i]._iPLToHit += r2;
		break;
	case IPL_TOHIT_DAMP_CURSE:
		item[i]._iPLDam -= r;
		if (param1 == 25)
			r2 = RndPL(1, 5);
		if (param1 == 50)
			r2 = RndPL(6, 10);
		item[i]._iPLToHit -= r2;
		break;
	case IPL_ACP:
		item[i]._iPLAC += r;
		break;
	case IPL_ACP_CURSE:
		item[i]._iPLAC -= r;
		break;
	case IPL_SETAC:
		item[i]._iAC = r;
		break;
	case IPL_AC_CURSE:
		item[i]._iAC -= r;
		break;
	case IPL_FIRERES:
		item[i]._iPLFR += r;
		break;
	case IPL_LIGHTRES:
		item[i]._iPLLR += r;
		break;
	case IPL_MAGICRES:
		item[i]._iPLMR += r;
		break;
	case IPL_ALLRES:
		item[i]._iPLFR += r;
		item[i]._iPLLR += r;
		item[i]._iPLMR += r;
		if (item[i]._iPLFR < 0)
			item[i]._iPLFR = 0;
		if (item[i]._iPLLR < 0)
			item[i]._iPLLR = 0;
		if (item[i]._iPLMR < 0)
			item[i]._iPLMR = 0;
		break;
	case IPL_SPLLVLADD:
		item[i]._iSplLvlAdd = r;
		break;
	case IPL_CHARGES:
		item[i]._iCharges *= param1;
		item[i]._iMaxCharges = item[i]._iCharges;
		break;
	case IPL_SPELL:
		item[i]._iSpell = param1;
#ifdef HELLFIRE
		item[i]._iCharges = param2;
#else
		item[i]._iCharges = param1; // BUGFIX: should be param2. This code was correct in v1.04, and the bug was introduced between 1.04 and 1.09b.
#endif
		item[i]._iMaxCharges = param2;
		break;
	case IPL_FIREDAM:
		item[i]._iFlags |= ISPL_FIREDAM;
#ifdef HELLFIRE
		item[i]._iFlags &= ~ISPL_LIGHTDAM;
#endif
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
#ifdef HELLFIRE
		item[i]._iLMinDam = 0;
		item[i]._iLMaxDam = 0;
#endif
		break;
	case IPL_LIGHTDAM:
		item[i]._iFlags |= ISPL_LIGHTDAM;
#ifdef HELLFIRE
		item[i]._iFlags &= ~ISPL_FIREDAM;
#endif
		item[i]._iLMinDam = param1;
		item[i]._iLMaxDam = param2;
#ifdef HELLFIRE
		item[i]._iFMinDam = 0;
		item[i]._iFMaxDam = 0;
#endif
		break;
	case IPL_STR:
		item[i]._iPLStr += r;
		break;
	case IPL_STR_CURSE:
		item[i]._iPLStr -= r;
		break;
	case IPL_MAG:
		item[i]._iPLMag += r;
		break;
	case IPL_MAG_CURSE:
		item[i]._iPLMag -= r;
		break;
	case IPL_DEX:
		item[i]._iPLDex += r;
		break;
	case IPL_DEX_CURSE:
		item[i]._iPLDex -= r;
		break;
	case IPL_VIT:
		item[i]._iPLVit += r;
		break;
	case IPL_VIT_CURSE:
		item[i]._iPLVit -= r;
		break;
	case IPL_ATTRIBS:
		item[i]._iPLStr += r;
		item[i]._iPLMag += r;
		item[i]._iPLDex += r;
		item[i]._iPLVit += r;
		break;
	case IPL_ATTRIBS_CURSE:
		item[i]._iPLStr -= r;
		item[i]._iPLMag -= r;
		item[i]._iPLDex -= r;
		item[i]._iPLVit -= r;
		break;
	case IPL_GETHIT_CURSE:
		item[i]._iPLGetHit += r;
		break;
	case IPL_GETHIT:
		item[i]._iPLGetHit -= r;
		break;
	case IPL_LIFE:
		item[i]._iPLHP += r << 6;
		break;
	case IPL_LIFE_CURSE:
		item[i]._iPLHP -= r << 6;
		break;
	case IPL_MANA:
		item[i]._iPLMana += r << 6;
		break;
	case IPL_MANA_CURSE:
		item[i]._iPLMana -= r << 6;
		break;
	case IPL_DUR:
		r2 = r * item[i]._iMaxDur / 100;
		item[i]._iMaxDur += r2;
		item[i]._iDurability += r2;
		break;
#ifdef HELLFIRE
	case IPL_CRYSTALLINE:
		item[i]._iPLDam += 140 + r * 2;
		// no break
#endif
	case IPL_DUR_CURSE:
		item[i]._iMaxDur -= r * item[i]._iMaxDur / 100;
		if (item[i]._iMaxDur < 1)
			item[i]._iMaxDur = 1;
		item[i]._iDurability = item[i]._iMaxDur;
		break;
	case IPL_INDESTRUCTIBLE:
		item[i]._iDurability = DUR_INDESTRUCTIBLE;
		item[i]._iMaxDur = DUR_INDESTRUCTIBLE;
		break;
	case IPL_LIGHT:
		item[i]._iPLLight += param1;
		break;
	case IPL_LIGHT_CURSE:
		item[i]._iPLLight -= param1;
		break;
#ifdef HELLFIRE
	case IPL_MULT_ARROWS:
		item[i]._iFlags |= ISPL_MULT_ARROWS;
		break;
#endif
	case IPL_FIRE_ARROWS:
		item[i]._iFlags |= ISPL_FIRE_ARROWS;
#ifdef HELLFIRE
		item[i]._iFlags &= ~ISPL_LIGHT_ARROWS;
#endif
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
#ifdef HELLFIRE
		item[i]._iLMinDam = 0;
		item[i]._iLMaxDam = 0;
#endif
		break;
	case IPL_LIGHT_ARROWS:
		item[i]._iFlags |= ISPL_LIGHT_ARROWS;
#ifdef HELLFIRE
		item[i]._iFlags &= ~ISPL_FIRE_ARROWS;
#endif
		item[i]._iLMinDam = param1;
		item[i]._iLMaxDam = param2;
#ifdef HELLFIRE
		item[i]._iFMinDam = 0;
		item[i]._iFMaxDam = 0;
#endif
		break;
#ifdef HELLFIRE
	case IPL_FIREBALL:
		item[i]._iFlags |= (ISPL_LIGHT_ARROWS | ISPL_FIRE_ARROWS);
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
		item[i]._iLMinDam = 0;
		item[i]._iLMaxDam = 0;
		break;
#endif
	case IPL_THORNS:
		item[i]._iFlags |= ISPL_THORNS;
		break;
	case IPL_NOMANA:
		item[i]._iFlags |= ISPL_NOMANA;
		break;
	case IPL_NOHEALPLR:
		item[i]._iFlags |= ISPL_NOHEALPLR;
		break;
	case IPL_ABSHALFTRAP:
		item[i]._iFlags |= ISPL_ABSHALFTRAP;
		break;
	case IPL_KNOCKBACK:
		item[i]._iFlags |= ISPL_KNOCKBACK;
		break;
	case IPL_3XDAMVDEM:
		item[i]._iFlags |= ISPL_3XDAMVDEM;
		break;
	case IPL_ALLRESZERO:
		item[i]._iFlags |= ISPL_ALLRESZERO;
		break;
	case IPL_NOHEALMON:
		item[i]._iFlags |= ISPL_NOHEALMON;
		break;
	case IPL_STEALMANA:
		if (param1 == 3)
			item[i]._iFlags |= ISPL_STEALMANA_3;
		if (param1 == 5)
			item[i]._iFlags |= ISPL_STEALMANA_5;
		break;
	case IPL_STEALLIFE:
		if (param1 == 3)
			item[i]._iFlags |= ISPL_STEALLIFE_3;
		if (param1 == 5)
			item[i]._iFlags |= ISPL_STEALLIFE_5;
		break;
	case IPL_TARGAC:
#ifdef HELLFIRE
		item[i]._iPLEnAc = param1;
#else
		item[i]._iPLEnAc += r;
#endif
		break;
	case IPL_FASTATTACK:
		if (param1 == 1)
			item[i]._iFlags |= ISPL_QUICKATTACK;
		if (param1 == 2)
			item[i]._iFlags |= ISPL_FASTATTACK;
		if (param1 == 3)
			item[i]._iFlags |= ISPL_FASTERATTACK;
		if (param1 == 4)
			item[i]._iFlags |= ISPL_FASTESTATTACK;
		break;
	case IPL_FASTRECOVER:
		if (param1 == 1)
			item[i]._iFlags |= ISPL_FASTRECOVER;
		if (param1 == 2)
			item[i]._iFlags |= ISPL_FASTERRECOVER;
		if (param1 == 3)
			item[i]._iFlags |= ISPL_FASTESTRECOVER;
		break;
	case IPL_FASTBLOCK:
		item[i]._iFlags |= ISPL_FASTBLOCK;
		break;
	case IPL_DAMMOD:
		item[i]._iPLDamMod += r;
		break;
	case IPL_RNDARROWVEL:
		item[i]._iFlags |= ISPL_RNDARROWVEL;
		break;
	case IPL_SETDAM:
		item[i]._iMinDam = param1;
		item[i]._iMaxDam = param2;
		break;
	case IPL_SETDUR:
		item[i]._iDurability = param1;
		item[i]._iMaxDur = param1;
		break;
	case IPL_FASTSWING:
		item[i]._iFlags |= ISPL_FASTERATTACK;
		break;
	case IPL_ONEHAND:
		item[i]._iLoc = ILOC_ONEHAND;
		break;
	case IPL_DRAINLIFE:
		item[i]._iFlags |= ISPL_DRAINLIFE;
		break;
	case IPL_RNDSTEALLIFE:
		item[i]._iFlags |= ISPL_RNDSTEALLIFE;
		break;
	case IPL_INFRAVISION:
		item[i]._iFlags |= ISPL_INFRAVISION;
		break;
	case IPL_NOMINSTR:
		item[i]._iMinStr = 0;
		break;
	case IPL_INVCURS:
		item[i]._iCurs = param1;
		break;
	case IPL_ADDACLIFE:
#ifdef HELLFIRE
		item[i]._iFlags |= (ISPL_LIGHT_ARROWS | ISPL_FIRE_ARROWS);
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
		item[i]._iLMinDam = 1;
		item[i]._iLMaxDam = 0;
#else
		item[i]._iPLHP = (plr[myplr]._pIBonusAC + plr[myplr]._pIAC + plr[myplr]._pDexterity / 5) << 6;
#endif
		break;
	case IPL_ADDMANAAC:
#ifdef HELLFIRE
		item[i]._iFlags |= (ISPL_LIGHTDAM | ISPL_FIREDAM);
		item[i]._iFMinDam = param1;
		item[i]._iFMaxDam = param2;
		item[i]._iLMinDam = 2;
		item[i]._iLMaxDam = 0;
#else
		item[i]._iAC += (plr[myplr]._pMaxManaBase >> 6) / 10;
#endif
		break;
	case IPL_FIRERESCLVL:
		item[i]._iPLFR = 30 - plr[myplr]._pLevel;
		if (item[i]._iPLFR < 0)
			item[i]._iPLFR = 0;
		break;
#ifdef HELLFIRE
	case IPL_FIRERES_CURSE:
		item[i]._iPLFR -= r;
		break;
	case IPL_LIGHTRES_CURSE:
		item[i]._iPLLR -= r;
		break;
	case IPL_MAGICRES_CURSE:
		item[i]._iPLMR -= r;
		break;
	case IPL_ALLRES_CURSE:
		item[i]._iPLFR -= r;
		item[i]._iPLLR -= r;
		item[i]._iPLMR -= r;
		break;
	case IPL_DEVASTATION:
		item[i]._iDamAcFlags |= ISPLHF_DEVASTATION;
		break;
	case IPL_DECAY:
		item[i]._iDamAcFlags |= ISPLHF_DECAY;
		item[i]._iPLDam += r;
		break;
	case IPL_PERIL:
		item[i]._iDamAcFlags |= ISPLHF_PERIL;
		break;
	case IPL_JESTERS:
		item[i]._iDamAcFlags |= ISPLHF_JESTERS;
		break;
	case IPL_ACDEMON:
		item[i]._iDamAcFlags |= ISPLHF_ACDEMON;
		break;
	case IPL_ACUNDEAD:
		item[i]._iDamAcFlags |= ISPLHF_ACUNDEAD;
		break;
	case IPL_MANATOLIFE:
		r2 = ((plr[myplr]._pMaxManaBase >> 6) * 50 / 100);
		item[i]._iPLMana -= (r2 << 6);
		item[i]._iPLHP += (r2 << 6);
		break;
	case IPL_LIFETOMANA:
		r2 = ((plr[myplr]._pMaxHPBase >> 6) * 40 / 100);
		item[i]._iPLHP -= (r2 << 6);
		item[i]._iPLMana += (r2 << 6);
		break;
#endif
	}
	if (item[i]._iVAdd1 || item[i]._iVMult1) {
		item[i]._iVAdd2 = PLVal(r, param1, param2, minval, maxval);
		item[i]._iVMult2 = multval;
	} else {
		item[i]._iVAdd1 = PLVal(r, param1, param2, minval, maxval);
		item[i]._iVMult1 = multval;
	}
}

void GetItemPower(int i, int minlvl, int maxlvl, int flgs, BOOL onlygood)
{
	int pre, post, nt, nl, j, preidx, sufidx;
	int l[256];
	char istr[128];
	BYTE goe;

	pre = random_(23, 4);
	post = random_(23, 3);
	if (pre != 0 && post == 0) {
		if (random_(23, 2) != 0)
			post = 1;
		else
			pre = 0;
	}
	preidx = -1;
	sufidx = -1;
	goe = GOE_ANY;
	if (!onlygood && random_(0, 3) != 0)
		onlygood = TRUE;
	if (pre == 0) {
		nt = 0;
		for (j = 0; PL_Prefix[j].PLPower != IPL_INVALID; j++) {
			if (flgs & PL_Prefix[j].PLIType) {
				if (PL_Prefix[j].PLMinLvl >= minlvl && PL_Prefix[j].PLMinLvl <= maxlvl && (!onlygood || PL_Prefix[j].PLOk) && (flgs != PLT_STAFF || PL_Prefix[j].PLPower != IPL_CHARGES)) {
					l[nt] = j;
					nt++;
					if (PL_Prefix[j].PLDouble) {
						l[nt] = j;
						nt++;
					}
				}
			}
		}
		if (nt != 0) {
			preidx = l[random_(23, nt)];
			sprintf(istr, "%s %s", PL_Prefix[preidx].PLName, item[i]._iIName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			item[i]._iPrePower = PL_Prefix[preidx].PLPower;
			goe = PL_Prefix[preidx].PLGOE;
		}
	}
	if (post != 0) {
		nl = 0;
		for (j = 0; PL_Suffix[j].PLPower != IPL_INVALID; j++) {
			if (PL_Suffix[j].PLIType & flgs
			    && PL_Suffix[j].PLMinLvl >= minlvl && PL_Suffix[j].PLMinLvl <= maxlvl
			    && (goe | PL_Suffix[j].PLGOE) != (GOE_GOOD | GOE_EVIL)
			    && (!onlygood || PL_Suffix[j].PLOk)) {
				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			sufidx = l[random_(23, nl)];
			sprintf(istr, "%s of %s", item[i]._iIName, PL_Suffix[sufidx].PLName);
			strcpy(item[i]._iIName, istr);
			item[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Suffix[sufidx].PLPower,
			    PL_Suffix[sufidx].PLParam1,
			    PL_Suffix[sufidx].PLParam2,
			    PL_Suffix[sufidx].PLMinVal,
			    PL_Suffix[sufidx].PLMaxVal,
			    PL_Suffix[sufidx].PLMultVal);
			item[i]._iSufPower = PL_Suffix[sufidx].PLPower;
		}
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue(i);
}

#ifdef HELLFIRE
void GetItemBonus(int i, int idata, int minlvl, int maxlvl, BOOL onlygood, BOOLEAN allowspells)
#else
void GetItemBonus(int i, int idata, int minlvl, int maxlvl, BOOL onlygood)
#endif
{
	if (item[i]._iClass != ICLASS_GOLD) {
		if (minlvl > 25)
			minlvl = 25;

		switch (item[i]._itype) {
		case ITYPE_SWORD:
		case ITYPE_AXE:
		case ITYPE_MACE:
			GetItemPower(i, minlvl, maxlvl, PLT_WEAP, onlygood);
			break;
		case ITYPE_BOW:
			GetItemPower(i, minlvl, maxlvl, PLT_BOW, onlygood);
			break;
		case ITYPE_SHIELD:
			GetItemPower(i, minlvl, maxlvl, PLT_SHLD, onlygood);
			break;
		case ITYPE_LARMOR:
		case ITYPE_HELM:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR:
			GetItemPower(i, minlvl, maxlvl, PLT_ARMO, onlygood);
			break;
		case ITYPE_STAFF:
#ifdef HELLFIRE
			if (allowspells)
#endif
				GetStaffSpell(i, maxlvl, onlygood);
#ifdef HELLFIRE
			else
				GetItemPower(i, minlvl, maxlvl, PLT_STAFF, onlygood);
#endif
			break;
		case ITYPE_RING:
		case ITYPE_AMULET:
			GetItemPower(i, minlvl, maxlvl, PLT_MISC, onlygood);
			break;
		}
	}
}

void SetupItem(int i)
{
	int it;

	it = ItemCAnimTbl[item[i]._iCurs];
	item[i]._iAnimData = itemanims[it];
	item[i]._iAnimLen = ItemAnimLs[it];
	item[i]._iAnimWidth = 96;
	item[i]._iAnimWidth2 = 16;
	item[i]._iIdentified = FALSE;
	item[i]._iPostDraw = FALSE;

	if (!plr[myplr].pLvlLoad) {
		item[i]._iAnimFrame = 1;
		item[i]._iAnimFlag = TRUE;
		item[i]._iSelFlag = 0;
	} else {
		item[i]._iAnimFrame = item[i]._iAnimLen;
		item[i]._iAnimFlag = FALSE;
		item[i]._iSelFlag = 1;
	}
}

int RndItem(int m)
{
	int i, ri, r;
	int ril[512];

	if ((monster[m].MData->mTreasure & 0x8000) != 0)
		return -1 - (monster[m].MData->mTreasure & 0xFFF);

	if (monster[m].MData->mTreasure & 0x4000)
		return 0;

	if (random_(24, 100) > 40)
		return 0;

	if (random_(24, 100) > 25)
		return IDI_GOLD + 1;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd == IDROP_DOUBLE && monster[m].mLevel >= AllItemsList[i].iMinMLvl
#ifdef HELLFIRE
		    && ri < 512
#endif
		) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iRnd != IDROP_NEVER && monster[m].mLevel >= AllItemsList[i].iMinMLvl
#ifdef HELLFIRE
		    && ri < 512
#endif
		) {
			ril[ri] = i;
			ri++;
		}
		// BUGFIX: ri decremented even for IDROP_NEVER, thus Scroll of Resurrect
		// (IDI_RESURRECT) decrements ri, unintentionally removing gold drop in
		// Single Player (gold drop is still valid in Multi Player).
		if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1)
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1)
			ri--;
	}

	r = random_(24, ri);
	return ril[r] + 1;
}

int RndUItem(int m)
{
	int i, ri;
	int ril[512];
	BOOL okflag;

	if (m != -1 && (monster[m].MData->mTreasure & 0x8000) != 0 && gbMaxPlayers == 1)
		return -1 - (monster[m].MData->mTreasure & 0xFFF);

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		okflag = TRUE;
		if (AllItemsList[i].iRnd == IDROP_NEVER)
			okflag = FALSE;
		if (m != -1) {
			if (monster[m].mLevel < AllItemsList[i].iMinMLvl)
				okflag = FALSE;
		} else {
#ifdef HELLFIRE
			if (2 * curlv < AllItemsList[i].iMinMLvl)
#else
			if (2 * currlevel < AllItemsList[i].iMinMLvl)
#endif
				okflag = FALSE;
		}
		if (AllItemsList[i].itype == ITYPE_MISC)
			okflag = FALSE;
		if (AllItemsList[i].itype == ITYPE_GOLD)
			okflag = FALSE;
		if (AllItemsList[i].itype == ITYPE_FOOD)
			okflag = FALSE;
		if (AllItemsList[i].iMiscId == IMISC_BOOK)
			okflag = TRUE;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1)
			okflag = FALSE;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1)
			okflag = FALSE;
#ifdef HELLFIRE
		if (okflag && ri < 512) {
#else
		if (okflag) {
#endif
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(25, ri)];
}

int RndAllItems()
{
	int i, ri;
	int ril[512];

	if (random_(26, 100) > 25)
		return 0;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
#ifdef HELLFIRE
		if (AllItemsList[i].iRnd != IDROP_NEVER && 2 * curlv >= AllItemsList[i].iMinMLvl && ri < 512) {
#else
		if (AllItemsList[i].iRnd != IDROP_NEVER && 2 * currlevel >= AllItemsList[i].iMinMLvl) {
#endif
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SPL_RESURRECT && gbMaxPlayers == 1)
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && gbMaxPlayers == 1)
			ri--;
	}

	return ril[random_(26, ri)];
}

#ifdef HELLFIRE
int RndTypeItems(int itype, int imid, int lvl)
#else
int RndTypeItems(int itype, int imid)
#endif
{
	int i, ri;
	BOOL okflag;
	int ril[512];

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		okflag = TRUE;
		if (AllItemsList[i].iRnd == IDROP_NEVER)
			okflag = FALSE;
#ifdef HELLFIRE
		if (lvl << 1 < AllItemsList[i].iMinMLvl)
#else
		if (currlevel << 1 < AllItemsList[i].iMinMLvl)
#endif
			okflag = FALSE;
		if (AllItemsList[i].itype != itype)
			okflag = FALSE;
		if (imid != -1 && AllItemsList[i].iMiscId != imid)
			okflag = FALSE;
#ifdef HELLFIRE
		if (okflag && ri < 512) {
#else
		if (okflag) {
#endif
			ril[ri] = i;
			ri++;
		}
	}

	return ril[random_(27, ri)];
}

int CheckUnique(int i, int lvl, int uper, BOOL recreate)
{
	int j, idata, numu;
	BOOLEAN uok[128];

	if (random_(28, 100) > uper)
		return UITYPE_INVALID;

	numu = 0;
	memset(uok, 0, sizeof(uok));
	for (j = 0; UniqueItemList[j].UIItemId != UITYPE_INVALID; j++) {
		if (UniqueItemList[j].UIItemId == AllItemsList[item[i].IDidx].iItemId
		    && lvl >= UniqueItemList[j].UIMinLvl
		    && (recreate || !UniqueItemFlag[j] || gbMaxPlayers != 1)) {
			uok[j] = TRUE;
			numu++;
		}
	}

	if (numu == 0)
		return UITYPE_INVALID;

	random_(29, 10); /// BUGFIX: unused, last unique in array always gets chosen
	idata = 0;
	while (numu > 0) {
		if (uok[idata])
			numu--;
		if (numu > 0) {
			idata++;
			if (idata == 128)
				idata = 0;
		}
	}

	return idata;
}

void GetUniqueItem(int i, int uid)
{
	UniqueItemFlag[uid] = TRUE;
	SaveItemPower(i, UniqueItemList[uid].UIPower1, UniqueItemList[uid].UIParam1, UniqueItemList[uid].UIParam2, 0, 0, 1);

	if (UniqueItemList[uid].UINumPL > 1)
		SaveItemPower(i, UniqueItemList[uid].UIPower2, UniqueItemList[uid].UIParam3, UniqueItemList[uid].UIParam4, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 2)
		SaveItemPower(i, UniqueItemList[uid].UIPower3, UniqueItemList[uid].UIParam5, UniqueItemList[uid].UIParam6, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 3)
		SaveItemPower(i, UniqueItemList[uid].UIPower4, UniqueItemList[uid].UIParam7, UniqueItemList[uid].UIParam8, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 4)
		SaveItemPower(i, UniqueItemList[uid].UIPower5, UniqueItemList[uid].UIParam9, UniqueItemList[uid].UIParam10, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 5)
		SaveItemPower(i, UniqueItemList[uid].UIPower6, UniqueItemList[uid].UIParam11, UniqueItemList[uid].UIParam12, 0, 0, 1);

	strcpy(item[i]._iIName, UniqueItemList[uid].UIName);
	item[i]._iIvalue = UniqueItemList[uid].UIValue;

	if (item[i]._iMiscId == IMISC_UNIQUE)
		item[i]._iSeed = uid;

	item[i]._iUid = uid;
	item[i]._iMagical = ITEM_QUALITY_UNIQUE;
	item[i]._iCreateInfo |= CF_UNIQUE;
}

void SpawnUnique(int uid, int x, int y)
{
	int ii, itype;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (numitems >= MAXITEMS)
		return;

	ii = itemavail[0];
	GetSuperItemSpace(x, y, ii);
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = ii;

	itype = 0;
	while (AllItemsList[itype].iItemId != UniqueItemList[uid].UIItemId) {
		itype++;
	}

#ifdef HELLFIRE
	GetItemAttrs(ii, itype, curlv);
#else
	GetItemAttrs(ii, itype, currlevel);
#endif
	GetUniqueItem(ii, uid);
	SetupItem(ii);
	numitems++;
}

void ItemRndDur(int ii)
{
	if (item[ii]._iDurability && item[ii]._iDurability != DUR_INDESTRUCTIBLE)
		item[ii]._iDurability = random_(0, item[ii]._iMaxDur >> 1) + (item[ii]._iMaxDur >> 2) + 1;
}

void SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, BOOL onlygood, BOOL recreate, BOOL pregen)
{
	int iblvl, uid;

	item[ii]._iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(ii, idx, lvl >> 1);
	item[ii]._iCreateInfo = lvl;

	if (pregen)
		item[ii]._iCreateInfo = lvl | CF_PREGEN;
	if (onlygood)
		item[ii]._iCreateInfo |= CF_ONLYGOOD;

	if (uper == 15)
		item[ii]._iCreateInfo |= CF_UPER15;
	else if (uper == 1)
		item[ii]._iCreateInfo |= CF_UPER1;

	if (item[ii]._iMiscId != IMISC_UNIQUE) {
		iblvl = -1;
		if (random_(32, 100) <= 10 || random_(33, 100) <= lvl) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_STAFF) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_RING) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item[ii]._iMiscId == IMISC_AMULET) {
			iblvl = lvl;
		}
		if (onlygood)
			iblvl = lvl;
		if (uper == 15)
			iblvl = lvl + 4;
		if (iblvl != -1) {
			uid = CheckUnique(ii, iblvl, uper, recreate);
			if (uid == UITYPE_INVALID) {
#ifdef HELLFIRE
				GetItemBonus(ii, idx, iblvl >> 1, iblvl, onlygood, TRUE);
#else
				GetItemBonus(ii, idx, iblvl >> 1, iblvl, onlygood);
#endif
			} else {
				GetUniqueItem(ii, uid);
				item[ii]._iCreateInfo |= CF_UNIQUE;
			}
		}
		if (item[ii]._iMagical != ITEM_QUALITY_UNIQUE)
			ItemRndDur(ii);
	} else {
		if (item[ii]._iLoc != ILOC_UNEQUIPABLE) {
			//uid = CheckUnique(ii, iblvl, uper, recreate);
			//if (uid != UITYPE_INVALID) {
			//	GetUniqueItem(ii, uid);
			//}
			GetUniqueItem(ii, iseed); // BUG: the second argument to GetUniqueItem should be uid.
		}
	}
	SetupItem(ii);
}

void SpawnItem(int m, int x, int y, BOOL sendmsg)
{
	int ii, idx;
	// BUGFIX: onlygood may be used uninitialized in call to SetupAllItems.
	BOOL onlygood;

	if (monster[m]._uniqtype || ((monster[m].MData->mTreasure & 0x8000) && gbMaxPlayers != 1)) {
		idx = RndUItem(m);
		if (idx < 0) {
			SpawnUnique(-(idx + 1), x, y);
			return;
		}
		onlygood = TRUE;
	} else if (quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE || quests[Q_MUSHROOM]._qvar1 != QS_MUSHGIVEN) {
		idx = RndItem(m);
		if (!idx)
			return;
		if (idx > 0) {
			idx--;
			onlygood = FALSE;
		} else {
			SpawnUnique(-(idx + 1), x, y);
			return;
		}
	} else {
		idx = IDI_BRAIN;
		quests[Q_MUSHROOM]._qvar1 = QS_BRAINSPAWNED;
	}

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		if (monster[m]._uniqtype) {
			SetupAllItems(ii, idx, GetRndSeed(), monster[m].MData->mLevel, 15, onlygood, FALSE, FALSE);
		} else {
			SetupAllItems(ii, idx, GetRndSeed(), monster[m].MData->mLevel, 1, onlygood, FALSE, FALSE);
		}
		numitems++;
	}
}

void CreateItem(int uid, int x, int y)
{
	int ii, idx;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		idx = 0;
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;

		while (AllItemsList[idx].iItemId != UniqueItemList[uid].UIItemId) {
			idx++;
		}

#ifdef HELLFIRE
		GetItemAttrs(ii, idx, curlv);
#else
		GetItemAttrs(ii, idx, currlevel);
#endif
		GetUniqueItem(ii, uid);
		SetupItem(ii);
		item[ii]._iMagical = ITEM_QUALITY_UNIQUE;
		numitems++;
	}
}

void CreateRndItem(int x, int y, BOOL onlygood, BOOL sendmsg, BOOL delta)
{
	int idx, ii;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (onlygood)
		idx = RndUItem(-1);
	else
		idx = RndAllItems();

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#ifdef HELLFIRE
		SetupAllItems(ii, idx, GetRndSeed(), 2 * curlv, 1, onlygood, FALSE, delta);
#else
		SetupAllItems(ii, idx, GetRndSeed(), 2 * currlevel, 1, onlygood, FALSE, delta);
#endif
		numitems++;
	}
}

void SetupAllUseful(int ii, int iseed, int lvl)
{
	int idx;

	item[ii]._iSeed = iseed;
	SetRndSeed(iseed);

#ifdef HELLFIRE
	idx = random_(34, 7);
	switch (idx) {
	case 0:
		idx = IDI_PORTAL;
		if ((lvl <= 1))
			idx = IDI_HEAL;
		break;
	case 1:
	case 2:
		idx = IDI_HEAL;
		break;
	case 3:
		idx = IDI_PORTAL;
		if ((lvl <= 1))
			idx = IDI_MANA;
		break;
	case 4:
	case 5:
		idx = IDI_MANA;
		break;
	case 6:
		idx = IDI_OIL;
		break;
	default:
		idx = IDI_OIL;
		break;
	}
#else
	if (random_(34, 2) != 0)
		idx = IDI_HEAL;
	else
		idx = IDI_MANA;

	if (lvl > 1 && random_(34, 3) == 0)
		idx = IDI_PORTAL;
#endif

	GetItemAttrs(ii, idx, lvl);
	item[ii]._iCreateInfo = lvl + CF_USEFUL;
	SetupItem(ii);
}

void CreateRndUseful(int pnum, int x, int y, BOOL sendmsg)
{
	int ii;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#ifdef HELLFIRE
		SetupAllUseful(ii, GetRndSeed(), curlv);
#else
		SetupAllUseful(ii, GetRndSeed(), currlevel);
#endif
		numitems++;
	}
}

void CreateTypeItem(int x, int y, BOOL onlygood, int itype, int imisc, BOOL sendmsg, BOOL delta)
{
	int idx, ii;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
	if (itype != ITYPE_GOLD)
		idx = RndTypeItems(itype, imisc, curlv);
#else
	if (itype != ITYPE_GOLD)
		idx = RndTypeItems(itype, imisc);
#endif
	else
		idx = IDI_GOLD;

	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#ifdef HELLFIRE
		SetupAllItems(ii, idx, GetRndSeed(), 2 * curlv, 1, onlygood, FALSE, delta);
#else
		SetupAllItems(ii, idx, GetRndSeed(), 2 * currlevel, 1, onlygood, FALSE, delta);
#endif

		numitems++;
	}
}

void SpawnQuestItem(int itemid, int x, int y, int randarea, int selflag)
{
	BOOL failed;
	int i, j, tries;

#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (randarea) {
		tries = 0;
		while (1) {
			tries++;
			if (tries > 1000 && randarea > 1)
				randarea--;
			x = random_(0, MAXDUNX);
			y = random_(0, MAXDUNY);
			failed = FALSE;
			for (i = 0; i < randarea && !failed; i++) {
				for (j = 0; j < randarea && !failed; j++) {
					failed = !ItemSpaceOk(i + x, j + y);
				}
			}
			if (!failed)
				break;
		}
	}

	if (numitems < MAXITEMS) {
		i = itemavail[0];
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = i;
		item[i]._ix = x;
		item[i]._iy = y;
		dItem[x][y] = i + 1;
#ifdef HELLFIRE
		GetItemAttrs(i, itemid, curlv);
#else
		GetItemAttrs(i, itemid, currlevel);
#endif
		SetupItem(i);
		item[i]._iPostDraw = TRUE;
		if (selflag) {
			item[i]._iSelFlag = selflag;
			item[i]._iAnimFrame = item[i]._iAnimLen;
			item[i]._iAnimFlag = FALSE;
		}
		numitems++;
	}
}

void SpawnRock()
{
	int i, ii;
	int xx, yy;
	int ostand;

	ostand = FALSE;
	for (i = 0; i < nobjects && !ostand; i++) {
		ii = objectactive[i];
		ostand = object[ii]._otype == OBJ_STAND;
	}
#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (ostand) {
		i = itemavail[0];
		itemavail[0] = itemavail[127 - numitems - 1];
		itemactive[numitems] = i;
		xx = object[ii]._ox;
		yy = object[ii]._oy;
		item[i]._ix = xx;
		item[i]._iy = yy;
		dItem[xx][item[i]._iy] = i + 1;
#ifdef HELLFIRE
		GetItemAttrs(i, IDI_ROCK, curlv);
#else
		GetItemAttrs(i, IDI_ROCK, currlevel);
#endif
		SetupItem(i);
		item[i]._iSelFlag = 2;
		item[i]._iPostDraw = TRUE;
		item[i]._iAnimFrame = 11;
		numitems++;
	}
}

BOOL StoreStatOk(ItemStruct *h)
{
	BOOL sf;

	sf = TRUE;
	if (plr[myplr]._pStrength < h->_iMinStr)
		sf = FALSE;
	if (plr[myplr]._pMagic < h->_iMinMag)
		sf = FALSE;
	if (plr[myplr]._pDexterity < h->_iMinDex)
		sf = FALSE;

	return sf;
}

BOOL SmithItemOk(int i)
{
	BOOL rv;

	rv = TRUE;
	if (AllItemsList[i].itype == ITYPE_MISC)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_FOOD)
		rv = FALSE;
#ifdef HELLFIRE
	if (AllItemsList[i].itype == ITYPE_STAFF && AllItemsList[i].iSpell)
#else
	if (AllItemsList[i].itype == ITYPE_STAFF)
#endif
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_RING)
		rv = FALSE;
	if (AllItemsList[i].itype == ITYPE_AMULET)
		rv = FALSE;

	return rv;
}

int RndSmithItem(int lvl)
{
	int i, ri;
	int ril[512];

	ri = 0;
	for (i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (AllItemsList[i].iRnd != IDROP_NEVER && SmithItemOk(i) && lvl >= AllItemsList[i].iMinMLvl
#ifdef HELLFIRE
		    && ri < 512
#endif
		) {
			ril[ri] = i;
			ri++;
			if (AllItemsList[i].iRnd == IDROP_DOUBLE
#ifdef HELLFIRE
			    && ri < 512
#endif
			) {
				ril[ri] = i;
				ri++;
			}
		}
	}

	return ril[random_(50, ri)] + 1;
}

void BubbleSwapItem(ItemStruct *a, ItemStruct *b)
{
	ItemStruct h;

	h = *a;
	*a = *b;
	*b = h;
}

//void SortSmith()
//{
//	int j, k;
//	BOOL sorted;
//
//	j = 0;
//	while (smithitem[j + 1]._itype != ITYPE_NONE) {
//		j++;
//	}
//
//	sorted = FALSE;
//	while (j > 0 && !sorted) {
//		sorted = TRUE;
//		for (k = 0; k < j; k++) {
//			if (smithitem[k].IDidx > smithitem[k + 1].IDidx) {
//				BubbleSwapItem(&smithitem[k], &smithitem[k + 1]);
//				sorted = FALSE;
//			}
//		}
//		j--;
//	}
//}

int ItemNoFlippy()
{
	int r;

	r = itemactive[numitems - 1];
	item[r]._iAnimFrame = item[r]._iAnimLen;
	item[r]._iAnimFlag = FALSE;
	item[r]._iSelFlag = 1;

	return r;
}

void CreateSpellBook(int x, int y, int ispell, BOOL sendmsg, BOOL delta)
{
	int ii, idx;
	BOOL done;

	done = FALSE;
#ifdef HELLFIRE
	int lvl = spelldata[ispell].sBookLvl + 1;
	if (lvl < 1) {
		return;
	}
	idx = RndTypeItems(ITYPE_MISC, IMISC_BOOK, lvl);
#else
	idx = RndTypeItems(ITYPE_MISC, IMISC_BOOK);
#endif
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
		while (!done) {
#ifdef HELLFIRE
			SetupAllItems(ii, idx, GetRndSeed(), 2 * lvl, 1, TRUE, FALSE, delta);
#else
			SetupAllItems(ii, idx, GetRndSeed(), 2 * currlevel, 1, TRUE, FALSE, delta);
#endif
			if (item[ii]._iMiscId == IMISC_BOOK && item[ii]._iSpell == ispell)
				done = TRUE;
		}
		numitems++;
	}
}

void CreateMagicArmor(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta)
{
	int ii, idx;
	BOOL done;

	done = FALSE;
#ifdef HELLFIRE
	int curlv = items_get_currlevel();
#endif
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#ifdef HELLFIRE
		idx = RndTypeItems(imisc, IMISC_NONE, curlv);
#else
		idx = RndTypeItems(imisc, IMISC_NONE);
#endif
		while (!done) {
#ifdef HELLFIRE
			SetupAllItems(ii, idx, GetRndSeed(), 2 * curlv, 1, TRUE, FALSE, delta);
#else
			SetupAllItems(ii, idx, GetRndSeed(), 2 * currlevel, 1, TRUE, FALSE, delta);
#endif
			if (item[ii]._iCurs == icurs)
				done = TRUE;
			else
#ifdef HELLFIRE
				idx = RndTypeItems(imisc, IMISC_NONE, curlv);
#else
				idx = RndTypeItems(imisc, IMISC_NONE);
#endif
		}
		numitems++;
	}
}

void CreateMagicWeapon(int x, int y, int imisc, int icurs, BOOL sendmsg, BOOL delta)
{
	int ii, idx;
	BOOL done;

	done = FALSE;
#ifdef HELLFIRE
	int imid;
	if (imisc == ITYPE_STAFF)
		imid = IMISC_STAFF;
	else
		imid = IMISC_NONE;
	int curlv = items_get_currlevel();
#endif
	if (numitems < MAXITEMS) {
		ii = itemavail[0];
		GetSuperItemSpace(x, y, ii);
		itemavail[0] = itemavail[MAXITEMS - numitems - 1];
		itemactive[numitems] = ii;
#ifdef HELLFIRE
		idx = RndTypeItems(imisc, imid, curlv);
#else
		idx = RndTypeItems(imisc, IMISC_NONE);
#endif
		while (!done) {
#ifdef HELLFIRE
			SetupAllItems(ii, idx, GetRndSeed(), 2 * curlv, 1, TRUE, FALSE, delta);
#else
			SetupAllItems(ii, idx, GetRndSeed(), 2 * currlevel, 1, TRUE, FALSE, delta);
#endif
			if (item[ii]._iCurs == icurs)
				done = TRUE;
			else
#ifdef HELLFIRE
				idx = RndTypeItems(imisc, imid, curlv);
#else
				idx = RndTypeItems(imisc, IMISC_NONE);
#endif
		}
		numitems++;
	}
}
