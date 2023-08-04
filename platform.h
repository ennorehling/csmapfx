#pragma once

#ifndef WIN32
#error "This code only for use with Windows targets"
#endif

#include <fx.h>

#include <windows.h>

long UploadFile(const FXString &filename, const FXString &username, const FXString &password, FXString& outBody);
