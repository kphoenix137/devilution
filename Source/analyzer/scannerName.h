#pragma once

#include <optional>
#include <string>
#include <string_view>

enum class Scanners {
	None,
	Path,
	Quest,
	Stairs,
	Warp,
	Pattern,
	GameSeed,
	Item,
};

std::optional<Scanners> Scanners_FromDisplayName(std::string_view name);
std::optional<std::string> Scanners_ToDisplayName(Scanners scanner);
