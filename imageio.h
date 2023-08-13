#pragma once

#include "map.h"

bool SaveMapPNG(const FXString &filename, const class FXCSMap &map, const FXCSMap::IslandInfo &islands, FXApp *app, FXProgressDialog *progress = nullptr);
