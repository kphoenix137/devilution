#include "item.h"

#include <iostream>

#include "../items.h"
#include "../monster.h"
#include "../objects.h"

static void CreateItemsFromObject(int oid)
{
	switch (object[oid]._otype) {
	case OBJ_CHEST1:
	case OBJ_CHEST2:
	case OBJ_CHEST3:
	case OBJ_TCHEST1:
	case OBJ_TCHEST2:
	case OBJ_TCHEST3:
		SetRndSeed(object[oid]._oRndSeed);
		if (setlevel) {
			for (int j = 0; j < object[oid]._oVar1; j++) {
				CreateRndItem(object[oid]._ox, object[oid]._oy, TRUE, TRUE, FALSE);
			}
		} else {
			for (int j = 0; j < object[oid]._oVar1; j++) {
				if (object[oid]._oVar2 != 0)
					CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
				else
					CreateRndUseful(0, object[oid]._ox, object[oid]._oy, TRUE);
			}
		}
		break;
	case OBJ_SARC:
		SetRndSeed(object[oid]._oRndSeed);
		if (object[oid]._oVar1 <= 2)
			CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
		break;
	case OBJ_DECAP:
		SetRndSeed(object[oid]._oRndSeed);
		CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
		break;
	case OBJ_BARREL:
		SetRndSeed(object[oid]._oRndSeed);
		if (object[oid]._oVar2 <= 1) {
			if (object[oid]._oVar3 == 0)
				CreateRndUseful(0, object[oid]._ox, object[oid]._oy, TRUE);
			else
				CreateRndItem(object[oid]._ox, object[oid]._oy, FALSE, TRUE, FALSE);
		}
		break;
	case OBJ_SKELBOOK:
	case OBJ_BOOKSTAND:
		SetRndSeed(object[oid]._oRndSeed);
		if (random_(161, 5) != 0)
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_MISC, IMISC_SCROLL, TRUE, FALSE);
		else
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		break;
	case OBJ_BOOKCASEL:
	case OBJ_BOOKCASER:
		SetRndSeed(object[oid]._oRndSeed);
		CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_MISC, IMISC_BOOK, TRUE, FALSE);
		break;
	case OBJ_ARMORSTAND:
	case OBJ_WARARMOR: {
		SetRndSeed(object[oid]._oRndSeed);
		BOOL uniqueRnd = random_(0, 2);
		if (currlevel <= 5) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, TRUE, ITYPE_LARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 6 && currlevel <= 9) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, uniqueRnd, ITYPE_MARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 10 && currlevel <= 12) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, ITYPE_HARMOR, IMISC_NONE, TRUE, FALSE);
		} else if (currlevel >= 13 && currlevel <= 16) {
			CreateTypeItem(object[oid]._ox, object[oid]._oy, TRUE, ITYPE_HARMOR, IMISC_NONE, TRUE, FALSE);
		}
	} break;
	case OBJ_WARWEAP:
	case OBJ_WEAPONRACK: {
		SetRndSeed(object[oid]._oRndSeed);
		int weaponType = ITYPE_MISC;

		switch (random_(0, 4) + ITYPE_SWORD) {
		case ITYPE_SWORD:
			weaponType = ITYPE_SWORD;
			break;
		case ITYPE_AXE:
			weaponType = ITYPE_AXE;
			break;
		case ITYPE_BOW:
			weaponType = ITYPE_BOW;
			break;
		case ITYPE_MACE:
			weaponType = ITYPE_MACE;
			break;
		}

		if (leveltype > 1)
			CreateTypeItem(object[oid]._ox, object[oid]._oy, TRUE, weaponType, IMISC_NONE, TRUE, FALSE);
		else
			CreateTypeItem(object[oid]._ox, object[oid]._oy, FALSE, weaponType, IMISC_NONE, TRUE, FALSE);
	} break;
	}
}

static void DropAllItems()
{
	MonsterItems = numitems;
	for (int i = 0; i < nummonsters; i++) {
		int mid = monstactive[i];
		if (monster[mid].MType->mtype == MT_GOLEM)
			continue;
		SetRndSeed(monster[mid]._mRndSeed);
		SpawnItem(mid, monster[mid]._mx, monster[mid]._my, TRUE);
	}

	ObjectItems = numitems;
	for (int i = 0; i < nobjects; i++) {
		int oid = objectactive[i];
		CreateItemsFromObject(oid);
	}
}

bool ScannerItem::skipLevel(int level)
{
	if (level == 0)
		return false;
	return level != *Config.target;
}

bool LocateItem()
{
	DropAllItems();

	POI = { -1, -1 };

	for (int i = 0; i < numitems; i++) {
		int ii = itemactive[i];
		ItemStruct &searchItem = item[ii];

		if (Config.targetStr.compare(searchItem._iIName)) {
			POI = { searchItem._ix, searchItem._iy };
			return true;
		}
	}

	return false;
}

bool ScannerItem::levelMatches(std::optional<uint32_t> levelSeed)
{
	if (!LocateItem())
		return false;

	if (Config.verbose) {
		std::cerr << "Monster Count: " << nummonsters << std::endl;
		for (int i = 0; i < nummonsters; i++) {
			std::cerr << "Monster " << i << ": " << monster[monstactive[i]].mName << " (" << monster[monstactive[i]]._mRndSeed << ")" << std::endl;
		}
		std::cerr << std::endl;
		std::cerr << "Object Count: " << nobjects << std::endl;
		for (int i = 0; i < nobjects; i++) {
			int oid = objectactive[i];
			char objstr[50];
			GetObjectStr(oid, objstr);
			std::cerr << "Object " << i << ": " << objstr << " (" << object[oid]._oRndSeed << ")" << std::endl;
		}
		std::cerr << std::endl;
		std::cerr << "Item Count: " << numitems << std::endl;
		for (int i = 0; i < numitems; i++) {
			std::string prefix = "";
			if (i >= ObjectItems)
				prefix = "Object ";
			else if (i >= MonsterItems)
				prefix = "Monster ";
			std::cerr << prefix << "Item " << i << ": " << item[itemactive[i]]._iIName << " (" << item[itemactive[i]]._iSeed << ")" << std::endl;
		}
	}

	std::cout << sgGameInitInfo.dwSeed << std::endl;

	return true;
}
