#pragma once

#include "../funkMapGen.h"

class ScannerWarp : public Scanner {
public:
	bool skipLevel(int level) override;
	bool levelMatches(std::optional<uint32_t> levelSeed) override;
};
