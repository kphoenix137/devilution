#pragma once

#include "../funkMapGen.h"

class ScannerPath : public Scanner {
public:
	bool skipSeed() override;
	bool skipLevel(int level) override;
	bool levelMatches(std::optional<uint32_t> levelSeed) override;
};
