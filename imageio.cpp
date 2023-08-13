#include "imageio.h"
#include "map.h"

#include "png.h"

#include <fx.h>

#ifdef _MSC_VER
#pragma warning(disable : 4611)
// interaction between '_setjmp' and C++ object destruction is non-portable
#endif

// Custom write function, which will write to the stream in our case
static void user_write_fn(png_structp png_ptr, png_bytep buffer, png_size_t size){
  FXStream *store=(FXStream*)png_get_io_ptr(png_ptr);
  store->save((FXchar*)buffer, size);
  }


// Custom output flush function, a no-op in our case
static void user_flush_fn(png_structp ){ }


// Custom error handler; this is unrecoverable
static void user_error_fn(png_structp png_ptr, png_const_charp message){
  FXStream* store=(FXStream*)png_get_error_ptr(png_ptr);
  store->setError(FXStreamFormat);                      // Flag this as a format error in FXStream
  FXTRACE((100, "Error in png: %s\n", message));
  longjmp(png_jmpbuf(png_ptr), 1);                           // Bail out
  }


// Custom warning handler; we assume this is recoverable
static void user_warning_fn(png_structp, png_const_charp message){
  //FXStream* store=(FXStream*)png_get_error_ptr(png_ptr);
  FXTRACE((100, "Warning in png: %s\n", message));
  }

// Save a PNG image
bool SaveMapPNG(const FXString& filename, const FXCSMap& map, const FXCSMap::IslandInfo &islands, FXApp * app, FXProgressDialog * dlg)
{
    FXint width = map.getImageWidth();
    FXint height = map.getImageHeight();
    FXint tileSize = 2048;
    bool bSuccess = true;
    FXImage image(app, nullptr, 0, tileSize, tileSize);
    image.create();

    // Resources that must be released:
    FXFileStream store;
    png_bytep *row_pointers = nullptr;
    png_bytep rows = nullptr;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;

    store.open(filename, FXStreamSave);
    bSuccess = store.status() == FXStreamOK;
    // Row pointers
    if (bSuccess) {
        bSuccess = FXMALLOC(&row_pointers, png_bytep, tileSize);
        if (bSuccess) {
            bSuccess = FXMALLOC(&rows, FXColor, width * tileSize);
            if (bSuccess) {
                for (int i = 0; i != tileSize; ++i) {
                    row_pointers[i] = rows + i * width * sizeof(FXColor);
                }
            }
        }
    }

    if (bSuccess) {
        // Create and initialize the png_struct with the desired error handler functions.
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, &store, user_error_fn, user_warning_fn);
        if (png_ptr) {
            // Allocate/initialize the image information data.
            info_ptr = png_create_info_struct(png_ptr);
        }
    }
	// Set error handling.
    if (info_ptr)
        bSuccess = setjmp(png_jmpbuf(png_ptr)) == 0;

    if (bSuccess) {
        // Using replacement read functions
        png_set_write_fn(png_ptr, (void *)&store, user_write_fn, user_flush_fn);

        // Set the header
        png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(png_ptr, info_ptr);

        if (dlg) {
            dlg->setTotal(height);
        }
        FXPoint mapOffset = map.getMapLeftTop();
        FXRectangle tile(mapOffset.x, mapOffset.y, tileSize, tileSize);
        // paint it slice by slice
        for (FXint y = 0; y < height && !(dlg && dlg->isCancelled()); y += tileSize)
        {
            // update progress bar
            if (dlg) {
                dlg->setProgress(y);
                app->runModalWhileEvents(dlg);
            }
            tile.y = mapOffset.y + y;
            tile.h = tileSize;
            if (y + tile.h > height)
                tile.h = height - y;

            for (FXint x = 0; bSuccess && x < width && !(dlg && dlg->isCancelled()); x += tileSize)
            {
                tile.x = mapOffset.x + x;
                map.drawSlice(image, tile, &islands);

                FXColor *data = image.getData();
                if (data) {
                    tile.w = tileSize;
                    if (x + tile.w > width)
                        tile.w = width - x;
                    // Set up row pointers
                    for (int i = 0; i < tile.h; ++i) {
                        memcpy(row_pointers[i] + x * sizeof(FXColor), data + i * tileSize, tile.w * sizeof(FXColor));
                    }
                }
                else {
                    bSuccess = false;
                }
            }
            if (!bSuccess) break;
            png_write_rows(png_ptr, row_pointers, tile.h);
        }
        // Wrap up
        if (bSuccess)
            png_write_end(png_ptr, info_ptr);
    }

	// clean up after the write, and free any memory allocated
	png_destroy_write_struct(&png_ptr, &info_ptr);

	// Get rid of it
    FXFREE(&rows);
    FXFREE(&row_pointers);

	return bSuccess;
}
