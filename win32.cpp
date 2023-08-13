#ifndef WIN32
#error "This code only for use with Windows targets"
#endif

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "gdiplus.lib")

#include "win32.h"
#include "version.h"
#include "map.h"
#include <windows.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <wininet.h>

#include <memory>

long UploadFile(const FXString &filename, const FXString &username, const FXString &password, FXString &outBody)
{
    DWORD dwStatusCode = 0;
    DWORD dwDownloaded = 0;
    BOOL  bResults = FALSE;
    HINTERNET hConnect = NULL, hRequest = NULL;

    // Use InternetOpen to obtain a session handle.
    HINTERNET hSession = InternetOpen(TEXT(CSMAP_APP_TITLE_VERSION),
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL,
        NULL, 0);

    // Specify an HTTP server.
    if (hSession) {
        WCHAR wszUserName[MAX_PATH];
        WCHAR wszPassword[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, username.text(), -1, wszUserName, MAX_PATH);
        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, password.text(), -1, wszPassword, MAX_PATH);
        hConnect = InternetConnect(hSession, TEXT("www.eressea.kn-bremen.de"),
            INTERNET_DEFAULT_HTTPS_PORT,
            wszUserName, wszPassword,
            INTERNET_SERVICE_HTTP, 0, NULL);
    }
    // Create an HTTPS request handle.
    if (hConnect) {
        DWORD dwOpenRequestFlags = INTERNET_FLAG_SECURE |
            INTERNET_FLAG_KEEP_CONNECTION |
            INTERNET_FLAG_NO_COOKIES |
            INTERNET_FLAG_NO_CACHE_WRITE |
            INTERNET_FLAG_NO_UI |
            INTERNET_FLAG_RELOAD;
        hRequest = HttpOpenRequest(hConnect, TEXT("POST"), TEXT("/eressea/orders-php/upload.php"),
            NULL, NULL, NULL, dwOpenRequestFlags, NULL);
    }

    // Send a request.
    if (hRequest) {
        WCHAR wszFilename[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, filename.text(), -1, wszFilename, MAX_PATH);
        HANDLE hMap = NULL;
        HANDLE hFile = CreateFile(
            wszFilename,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hFile) {
            hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        }
        if (hMap) {
            LPVOID lpvFile = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
            DWORD dwFileSize = GetFileSize(hFile, NULL);

            bResults = HttpAddRequestHeaders(hRequest, TEXT("Content-Type: application/octet-stream\r\n"), -1, HTTP_ADDREQ_FLAG_REPLACE);
            if (bResults) {
                bResults = HttpSendRequest(
                    hRequest,
                    NULL,
                    0,
                    lpvFile,
                    dwFileSize);
            }
        }
    }

    // End the request.
    if (bResults) {
        DWORD dwHeaderBuffSize = sizeof(dwStatusCode);
        bResults = HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwHeaderBuffSize, NULL);
    }
    // Keep checking for data until there is nothing left.
    if (bResults) {
        DWORD dwSize = 0;
        do
        {
            dwSize = 0;
            // Check for available data.
            if (!InternetQueryDataAvailable(hRequest, &dwSize, 0, NULL)) {
                dwStatusCode = GetLastError() | 0x10000;
                break;
            }

            // Read the Data.
            DWORD dwRemain = dwSize;
            do {
                const DWORD dwBuffSize = 4096;
                CHAR szOutBuffer[dwBuffSize];
                DWORD dwBytes = (dwRemain > dwBuffSize - 1) ? (dwBuffSize - 1): dwRemain;
                ZeroMemory(szOutBuffer, dwBytes + 1);
                if (!InternetReadFile(hRequest, (LPVOID)szOutBuffer, dwBytes, &dwDownloaded))
                {
                    dwStatusCode = GetLastError() | 0x10000;
                    break;
                }
                outBody.append(szOutBuffer, dwBytes);
                dwRemain -= dwBytes;
            } while (dwRemain > 0);
        } while (dwSize > 0);
    }

    // Report any errors.
    if (!bResults) {
        dwStatusCode = GetLastError() | 0x10000;
    }
    // Close any open handles.
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hSession) InternetCloseHandle(hSession);

    return dwStatusCode;
}

#define MAX_IMAGE_SIZE (32 * 1024 * 1024)

static bool SaveImage(const FXString &filename, const FXCSMap &map, const FXCSMap::IslandInfo *islands, FXImage &image, const FXPoint &mapOffset, FXint bmpHeight, CLSID encoderClsid, FXProgressDialog *dlg = nullptr)
{
    bool bSuccess = true;
    FXint bmpWidth = map.getImageWidth();
    Gdiplus::Bitmap bitmap(bmpWidth, bmpHeight, PixelFormat32bppRGB);
    Gdiplus::Status status = Gdiplus::Ok;
    for (FXint tileY = 0; tileY < bmpHeight; tileY += image.getHeight()) {
        FXint height = image.getHeight();
        if (height + tileY > bmpHeight) {
            height = bmpHeight - tileY;
        }
        if (dlg) {
            dlg->setProgress(tileY);
            dlg->getApp()->runModalWhileEvents(dlg);
        }
        for (FXint tileX = 0; tileX < bmpWidth; tileX += image.getWidth()) {
            if (dlg && dlg->isCancelled()) {
                bSuccess = false;
                break;
            }
            FXint width = image.getWidth();
            if (width + tileX > bmpWidth) {
                width = bmpWidth - tileX;
            }
            FXRectangle slice(mapOffset.x + tileX, mapOffset.y + tileY, width, height);
            map.drawSlice(image, slice, islands);
            FXColor *pixels = image.getData();
            for (FXint i = 0; i != width * height; ++i) {
                FXColor rgba = pixels[i];
                pixels[i] = FXBLUEVAL(rgba) + (FXGREENVAL(rgba) << 8) + (FXREDVAL(rgba) << 16);
            }
            Gdiplus::Rect tile(tileX, tileY, width, height);
            Gdiplus::BitmapData bitmapData;
            ZeroMemory(&bitmapData, sizeof(bitmapData));
            bitmapData.Scan0 = image.getData();
            bitmapData.Stride = image.getWidth() * 4;
            bitmapData.Width = width;
            bitmapData.Height = height;
            bitmapData.PixelFormat = PixelFormat32bppRGB;
            status = bitmap.LockBits(&tile, Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
                bitmapData.PixelFormat, &bitmapData);
            if (status == Gdiplus::Ok) {
                status = bitmap.UnlockBits(&bitmapData);
            }
            if (status != Gdiplus::Ok) {
                bSuccess = false;
                break;
            }
        }
        if (status != Gdiplus::Ok) {
            break;
        }
    }
    if (bSuccess) {
        WCHAR wszFilename[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, filename.text(), -1, wszFilename, MAX_PATH);
        status = bitmap.Save(wszFilename, &encoderClsid);
        bSuccess = status == Gdiplus::Ok;
    }
    else {
        throw std::runtime_error("Ein Fehler ist aufgetreten, die Datei konnte nicht erzeugt werden.");
    }
    return bSuccess;
}

bool SaveMapImage(const FXString &filename, const FXCSMap &map, const FXCSMap::IslandInfo &islands, CLSID encoderClsid, FXApp *app, FXProgressDialog * dlg)
{
    FXival mapWidth = map.getImageWidth();
    FXival mapHeight = map.getImageHeight();
    // Get the CLSID of the PNG encoder.
    FXint tileHeight = mapHeight;
    if (tileHeight > 512)
        tileHeight = 512;
    FXint tileWidth = mapWidth;
    if (tileWidth * tileHeight > MAX_IMAGE_SIZE)
        tileWidth = MAX_IMAGE_SIZE / tileHeight;
    if (dlg) {
        dlg->setTotal(mapHeight);
    }
    FXPoint mapOffset = map.getMapLeftTop();
    FXImage image(app, nullptr, 0, tileWidth, tileHeight);
    image.create();
    return SaveImage(filename, map, &islands, image, mapOffset, mapHeight, encoderClsid);
}
