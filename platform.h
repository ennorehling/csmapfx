#pragma once

#include <fx.h>

long UploadFile(const FXString &filename, const FXString &username,
        const FXString &password, FXString &outBody);
#ifdef WITH_PNG_EXPORT
bool SavePNG(const FXString &filename, const class FXCSMap &map, class FXProgressDialog &win, const void* islandMap = nullptr);
#endif
