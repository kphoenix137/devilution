/**
 * @file spells.cpp
 *
 * Implementation of functionality for casting player spells.
 */
#include "all.h"

int GetManaAmount(int id, int sn)
{
	int ma; // mana amount

	// mana adjust
	int adj = 0;

	// spell level
	int sl = Players[id]._pSplLvl[sn] + Players[id]._pISplLvlAdd - 1;

	if (sl < 0) {
		sl = 0;
	}

	if (sl > 0) {
		adj = sl * spelldata[sn].sManaAdj;
	}
	if (sn == SPL_FIREBOLT) {
		adj >>= 1;
	}
	if (sn == SPL_RESURRECT && sl > 0) {
		adj = sl * (spelldata[SPL_RESURRECT].sManaCost / 8);
	}

	if (spelldata[sn].sManaCost == 255) {
		ma = ((BYTE)Players[id]._pMaxManaBase - adj);
	} else {
		ma = (spelldata[sn].sManaCost - adj);
	}

	ma <<= 6;

	if (sn == SPL_HEAL) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * Players[id]._pLevel - adj) << 6;
	}
	if (sn == SPL_HEALOTHER) {
		ma = (spelldata[SPL_HEAL].sManaCost + 2 * Players[id]._pLevel - adj) << 6;
	}

#ifdef HELLFIRE
	if (Players[id]._pClass == PC_SORCERER) {
		ma >>= 1;
	} else if (Players[id]._pClass == PC_ROGUE || Players[id]._pClass == PC_MONK || Players[id]._pClass == PC_BARD) {
		ma -= ma >> 2;
	}
#else
	if (Players[id]._pClass == PC_ROGUE) {
		ma -= ma >> 2;
	}
#endif

	if (spelldata[sn].sMinMana > ma >> 6) {
		ma = spelldata[sn].sMinMana << 6;
	}

	return ma * (100 - Players[id]._pISplCost) / 100;
}

void UseMana(int id, int sn)
{
	int ma; // mana cost

	if (id == myplr) {
		switch (Players[id]._pSplType) {
		case RSPLTYPE_SKILL:
		case RSPLTYPE_INVALID:
			break;
		case RSPLTYPE_SCROLL:
			RemoveScroll(id);
			break;
		case RSPLTYPE_CHARGES:
			UseStaffCharge(id);
			break;
		case RSPLTYPE_SPELL:
#ifdef _DEBUG
			if (!debug_mode_key_inverted_v) {
#endif
				ma = GetManaAmount(id, sn);
				Players[id]._pMana -= ma;
				Players[id]._pManaBase -= ma;
				drawmanaflag = TRUE;
#ifdef _DEBUG
			}
#endif
			break;
		}
	}
}

BOOL CheckSpell(int id, int sn, char st, BOOL manaonly)
{
	BOOL result;

#ifdef _DEBUG
	if (debug_mode_key_inverted_v)
		return TRUE;
#endif

	result = TRUE;
	if (!manaonly && pcurs != CURSOR_HAND) {
		result = FALSE;
	} else {
		if (st != RSPLTYPE_SKILL) {
			if (GetSpellLevel(id, sn) <= 0) {
				result = FALSE;
			} else {
				result = Players[id]._pMana >= GetManaAmount(id, sn);
			}
		}
	}

	return result;
}

void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int caster, int spllvl)
{
	int i;
	int dir; // missile direction

	switch (caster) {
	case TARGET_PLAYERS:
		dir = monster[id]._mdir;
		break;
	case TARGET_MONSTERS:
		// caster must be 0 already in this case, but oh well,
		// it's needed to generate the right code
		caster = TARGET_MONSTERS;
		dir = Players[id]._pdir;

#ifdef HELLFIRE
		if (spl == SPL_FIREWALL || spl == SPL_LIGHTWALL) {
#else
		if (spl == SPL_FIREWALL) {
#endif
			dir = Players[id]._pVar3;
		}
		break;
	}

	for (i = 0; spelldata[spl].sMissiles[i] != MIS_ARROW && i < 3; i++) {
		AddMissile(sx, sy, dx, dy, dir, spelldata[spl].sMissiles[i], caster, id, 0, spllvl);
	}

	if (spelldata[spl].sMissiles[0] == MIS_TOWN) {
		UseMana(id, SPL_TOWN);
	}
	if (spelldata[spl].sMissiles[0] == MIS_CBOLT) {
		UseMana(id, SPL_CBOLT);

		for (i = (spllvl >> 1) + 3; i > 0; i--) {
			AddMissile(sx, sy, dx, dy, dir, MIS_CBOLT, caster, id, 0, spllvl);
		}
	}
}

static void PlacePlayer(int pnum)
{
	int nx, ny, max, min, x, y;
	DWORD i;
	BOOL done;

	if (Players[pnum].plrlevel == currlevel) {
		for (i = 0; i < 8; i++) {
			nx = Players[pnum]._px + plrxoff2[i];
			ny = Players[pnum]._py + plryoff2[i];

			if (PosOkPlayer(pnum, nx, ny)) {
				break;
			}
		}

		if (!PosOkPlayer(pnum, nx, ny)) {
			done = FALSE;

			for (max = 1, min = -1; min > -50 && !done; max++, min--) {
				for (y = min; y <= max && !done; y++) {
					ny = Players[pnum]._py + y;

					for (x = min; x <= max && !done; x++) {
						nx = Players[pnum]._px + x;

						if (PosOkPlayer(pnum, nx, ny)) {
							done = TRUE;
						}
					}
				}
			}
		}

		Players[pnum]._px = nx;
		Players[pnum]._py = ny;

		dPlayer[nx][ny] = pnum + 1;

		if (pnum == myplr) {
			ViewX = nx;
			ViewY = ny;
		}
	}
}

/**
 * @param pnum player index
 * @param rid target player index
 */
void DoResurrect(int pnum, int rid)
{
	int hp;

	if ((char)rid != -1) {
		AddMissile(Players[rid]._px, Players[rid]._py, Players[rid]._px, Players[rid]._py, 0, MIS_RESURRECTBEAM, TARGET_MONSTERS, pnum, 0, 0);
	}

	if (pnum == myplr) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && Players[rid]._pHitPoints == 0) {
		if (rid == myplr) {
			deathflag = FALSE;
			gamemenu_off();
			drawhpflag = TRUE;
			drawmanaflag = TRUE;
		}

		ClrPlrPath(rid);
		Players[rid].destAction = ACTION_NONE;
		Players[rid]._pInvincible = FALSE;
#ifndef HELLFIRE
		PlacePlayer(rid);
#endif

		hp = 10 << 6;
#ifndef HELLFIRE
		if (Players[rid]._pMaxHPBase < (10 << 6)) {
			hp = Players[rid]._pMaxHPBase;
		}
#endif
		SetPlayerHitPoints(rid, hp);

		Players[rid]._pHPBase = Players[rid]._pHitPoints + (Players[rid]._pMaxHPBase - Players[rid]._pMaxHP); // CODEFIX: does the same stuff as SetPlayerHitPoints above, can be removed
		Players[rid]._pMana = 0;
		Players[rid]._pManaBase = Players[rid]._pMana + (Players[rid]._pMaxManaBase - Players[rid]._pMaxMana);

		CalcPlrInv(rid, TRUE);

		if (Players[rid].plrlevel == currlevel) {
			StartStand(rid, Players[rid]._pdir);
		} else {
			Players[rid]._pmode = PM_STAND;
		}
	}
}

void DoHealOther(int pnum, int rid)
{
	int i, j, hp;

	if (pnum == myplr) {
		NewCursor(CURSOR_HAND);
	}

	if ((char)rid != -1 && (Players[rid]._pHitPoints >> 6) > 0) {
		hp = (random_(57, 10) + 1) << 6;

		for (i = 0; i < Players[pnum]._pLevel; i++) {
			hp += (random_(57, 4) + 1) << 6;
		}

		for (j = 0; j < GetSpellLevel(pnum, SPL_HEALOTHER); ++j) {
			hp += (random_(57, 6) + 1) << 6;
		}

#ifdef HELLFIRE
		if (Players[pnum]._pClass == PC_WARRIOR || Players[pnum]._pClass == PC_BARBARIAN) {
			hp <<= 1;
		} else if (Players[pnum]._pClass == PC_ROGUE || Players[pnum]._pClass == PC_BARD) {
			hp += hp >> 1;
		} else if (Players[pnum]._pClass == PC_MONK) {
			hp *= 3;
		}
#else
		if (Players[pnum]._pClass == PC_WARRIOR) {
			hp <<= 1;
		}

		if (Players[pnum]._pClass == PC_ROGUE) {
			hp += hp >> 1;
		}
#endif

		Players[rid]._pHitPoints += hp;

		if (Players[rid]._pHitPoints > Players[rid]._pMaxHP) {
			Players[rid]._pHitPoints = Players[rid]._pMaxHP;
		}

		Players[rid]._pHPBase += hp;

		if (Players[rid]._pHPBase > Players[rid]._pMaxHPBase) {
			Players[rid]._pHPBase = Players[rid]._pMaxHPBase;
		}

		drawhpflag = TRUE;
	}
}
