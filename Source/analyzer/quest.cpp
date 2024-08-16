#include "quest.h"

#include <iostream>

#include "../../types.h"

#include "../funkMapGen.h"
#include "../quests.h"

bool ScannerQuest::skipSeed()
{
	if (quests[Q_LTBANNER]._qactive != QUEST_NOTAVAIL) {
		if (Config.verbose)
			std::cerr << "Game Seed: " << sgGameInitInfo.dwSeed << " thrown out: Sign Quest" << std::endl;
		return true;
	}
	if (quests[Q_WARLORD]._qactive != QUEST_NOTAVAIL) {
		if (Config.verbose)
			std::cerr << "Game Seed: " << sgGameInitInfo.dwSeed << " thrown out: Warlord" << std::endl;
		return true;
	}

	std::cout << sgGameInitInfo.dwSeed << std::endl;

	return true;
}
