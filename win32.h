#pragma once

#include <windows.h>
#include <fx.h>

#include "map.h"

#define MAX_BITMAP_SIZE (16 * 16 * 1024 * 1024)
#define ENCRYPTED_PASSWORDS

long UploadFile(const FXString &filename, const FXString &username,
        const FXString &password, FXString &outBody);
bool SaveMapImage(const FXString &filename, const FXCSMap &map, const FXCSMap::IslandInfo &islands, 
    CLSID encoderClsid, FXApp *app, FXProgressDialog *dlg = nullptr);

FXString GetMimeType(const FXString &extension);
int GetEncoderClsid(const FXString &format, CLSID *pClsid);
int GetImagePatternList(const FXString &defaultType, FXString& patterns);

FXString EncryptString(const FXString &input);
FXString DecryptString(const FXString &input);
