#pragma once

#include "../funkMapGen.h"

class ScannerPattern : public Scanner {
public:
	DungeonMode getDungeonMode() override;
	bool skipLevel(int level) override;
	bool levelMatches(std::optional<uint32_t> levelSeed) override;
};
