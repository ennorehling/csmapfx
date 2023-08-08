#pragma once

#include <fx.h>

long UploadFile(const FXString &filename, const FXString &username,
        const FXString &password, FXString &outBody);
