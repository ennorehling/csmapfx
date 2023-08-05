#pragma once

#include <fx.h>

long UploadFile(const FXString &filename, const FXString &username,
        const FXString &password, FXString &outBody);
bool SavePNG(const FXString &filename, const class FXCSMap &map, FXint height, class FXImage &image, class FXProgressDialog &win);
