#include <fx.h>
#include "map.h"

#include "png.h"

#ifdef _MSC_VER
#pragma warning(disable : 4611)
// interaction between '_setjmp' and C++ object destruction is non-portable
#endif

// Custom read function, which will read from the stream in our case
/* not used
static void user_read_fn(png_structp png_ptr, png_bytep buffer, png_size_t size){
  FXStream *store=(FXStream*)png_get_io_ptr(png_ptr);
  store->load((FXchar*)buffer,size);
  }
*/

// Custom write function, which will write to the stream in our case
static void user_write_fn(png_structp png_ptr, png_bytep buffer, png_size_t size){
  FXStream *store=(FXStream*)png_get_io_ptr(png_ptr);
  store->save((FXchar*)buffer,size);
  }


// Custom output flush function, a no-op in our case
static void user_flush_fn(png_structp ){ }


// Custom error handler; this is unrecoverable
static void user_error_fn(png_structp png_ptr,png_const_charp message){
  FXStream* store=(FXStream*)png_get_error_ptr(png_ptr);
  store->setError(FXStreamFormat);                      // Flag this as a format error in FXStream
  FXTRACE((100,"Error in png: %s\n",message));
  longjmp(png_jmpbuf(png_ptr), 1);                           // Bail out
  }


// Custom warning handler; we assume this is recoverable
static void user_warning_fn(png_structp,png_const_charp message){
  //FXStream* store=(FXStream*)png_get_error_ptr(png_ptr);
  FXTRACE((100,"Warning in png: %s\n",message));
  }

// Save a PNG image
FXbool csmap_savePNG(FXStream& store, FXCSMap& map, FXImage& image, FXProgressDialog& dlg)
{
	png_structp png_ptr = nullptr;
	png_infop info_ptr = nullptr;
	png_bytep *row_pointers = nullptr;

	// what size the image is?
    FXint width = map.getContentWidth();
    FXint height = map.getContentHeight();
	FXint stepsize = image.getHeight();

	LeftTop mapOffset = map.getMapLeftTop();
	std::map<FXString, IslandPos> islands = map.collectIslandNames();

	// Create and initialize the png_struct with the desired error handler functions.
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,&store,user_error_fn,user_warning_fn);
	if(!png_ptr) return FALSE;

	// Allocate/initialize the image information data.
	info_ptr=png_create_info_struct(png_ptr);
	if(!info_ptr){
		png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
		return FALSE;
	}

	// Set error handling.
	if(setjmp(png_jmpbuf(png_ptr))){
		png_destroy_write_struct(&png_ptr,&info_ptr);
		return FALSE;
	}

	// Using replacement read functions
	png_set_write_fn(png_ptr,(void *)&store,user_write_fn,user_flush_fn);

	// Set the header
	png_set_IHDR(png_ptr,info_ptr,width,height,8,PNG_COLOR_TYPE_RGB_ALPHA,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);

	// Row pointers
	FXMALLOC(&row_pointers,png_bytep,stepsize);
	if(!row_pointers)
	{
		png_destroy_write_struct(&png_ptr,&info_ptr);
		return FALSE;
	}

	dlg.setTotal(height);
	dlg.getApp()->runModalWhileEvents(&dlg);

    FXDCWindow dc(&image);
    // paint it slice by slice
	for (FXint y = 0; y < height && !dlg.isCancelled(); y+=stepsize)
	{
        FXPoint tl(mapOffset.left, FXshort(mapOffset.top + y));
        FXPoint br(FXshort(tl.x + image.getWidth()), FXshort(tl.y + stepsize));
        dc.setForeground(map.getBackColor());
        dc.fillRectangle(0, 0, image.getWidth(), image.getHeight());
        map.paintMapLines(dc, tl, br);
        map.paintIslandNames(dc, tl, br, islands);
		image.restore();

		FXColor* data = image.getData();
		if (!data)
		{
			png_destroy_write_struct(&png_ptr,&info_ptr);
			FXFREE(&row_pointers);
			return FALSE;
		}

		// Set up row pointers
		if (y+stepsize > height)
			stepsize = height - y;

		for(int i = 0; i < stepsize; i++)
			row_pointers[i]=(png_bytep)&data[i*width];

		png_write_rows(png_ptr, &row_pointers[0], stepsize);

		// update statusbar
		dlg.setProgress(y);
		dlg.getApp()->runModalWhileEvents(&dlg);
	}

	// Wrap up
	png_write_end(png_ptr,info_ptr);

	// clean up after the write, and free any memory allocated
	png_destroy_write_struct(&png_ptr,&info_ptr);

	// Get rid of it
	FXFREE(&row_pointers);

	return TRUE;
}
