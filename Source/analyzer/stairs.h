#pragma once

#include "../funkMapGen.h"

class ScannerStairs : public Scanner {
public:
	DungeonMode getDungeonMode() override;
	bool skipSeed() override;
	bool skipLevel(int level) override;
	bool levelMatches(std::optional<uint32_t> levelSeed) override;
};
