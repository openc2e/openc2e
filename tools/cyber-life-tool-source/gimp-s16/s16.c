// Copyright (c) 2001, Creature Labs Ltd.
// All rights reserved.
 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
//  * Neither the name of Creature Labs Ltd. nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//    
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CREATURE LABS
// LTD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <stdlib.h>

#include "../../libgimp/gimp.h"

/*
 * Constants...
 */

#define PLUG_IN_VERSION		"1.1.6 - 17 May 1998"
#define SCALE_WIDTH		125


/*
 * Structures...
 */

typedef struct
{
  gint	type555;
} S16SaveVals;


/*
 * Local functions...
 */

static void	query(void);
static void	run(char *, int, GParam *, int *, GParam **);
static gint32	load_image(char *);
static gint	save_image (char *, gint32, gint32);

/*
 * Globals...
 */

GPlugInInfo	PLUG_IN_INFO =
{
  NULL,    /* init_proc */
  NULL,    /* quit_proc */
  query,   /* query_proc */
  run,     /* run_proc */
};

S16SaveVals	s16vals = 
{
  FALSE,
};

int		runme = FALSE;

MAIN()


/*
 * 'query()' - Respond to a plug-in query...
 */

static void query(void)
{
  static GParamDef	load_args[] =
  {
    { PARAM_INT32, "run_mode", "Interactive, non-interactive" },
    { PARAM_STRING, "filename", "The name of the file to load" },
    { PARAM_STRING, "raw_filename", "The name of the file to load" },
  };
  static GParamDef	load_return_vals[] =
  {
    { PARAM_IMAGE, "image", "Output image" },
  };
  static int		nload_args = sizeof (load_args) / sizeof (load_args[0]);
  static int		nload_return_vals = sizeof (load_return_vals) / sizeof (load_return_vals[0]);
  static GParamDef	save_args[] =
  {
    { PARAM_INT32,	"run_mode",	"Interactive, non-interactive" },
    { PARAM_IMAGE,	"image",	"Input image" },
    { PARAM_DRAWABLE,	"drawable",	"Drawable to save" },
    { PARAM_STRING,	"filename",	"The name of the file to save the image in" },
    { PARAM_STRING,	"raw_filename",	"The name of the file to save the image in" },
    { PARAM_INT32,	"type555",	"Save as 555 instead of 565" },
  };
  static int		nsave_args = sizeof (save_args) / sizeof (save_args[0]);


  gimp_install_procedure("file_s16_load",
      "Loads files in S16 file format",
      "This plug-in loads Creatures2 s16 files",
      "BenC",
      "BenC",
      PLUG_IN_VERSION,
      "<Load>/S16", NULL, PROC_PLUG_IN, nload_args, nload_return_vals,
      load_args, load_return_vals);

  gimp_install_procedure("file_s16_save",
      "Saves files in PNG file format",
      "This plug-in saves Creatures2 s16 files.",
      "BenC",
      "BenC",
      PLUG_IN_VERSION,
      "<Save>/S16", "RGB*,", PROC_PLUG_IN, nsave_args, 0, save_args, NULL);

  gimp_register_load_handler("file_s16_load", "s16", "");
  gimp_register_save_handler("file_s16_save", "s16", "");
}


/*
 * 'run()' - Run the plug-in...
 */

static void
run(char   *name,		/* I - Name of filter program. */
    int    nparams,		/* I - Number of parameters passed in */
    GParam *param,		/* I - Parameter values */
    int    *nreturn_vals,	/* O - Number of return values */
    GParam **return_vals)	/* O - Return values */
{
  gint32	image_ID;	/* ID of loaded image */
  GParam	*values;	/* Return values */


 /*
  * Initialize parameter data...
  */

  values = g_new(GParam, 2);

  values[0].type          = PARAM_STATUS;
  values[0].data.d_status = STATUS_SUCCESS;

  *return_vals  = values;

 /*
  * Load or save an image...
  */

  if (strcmp(name, "file_s16_load") == 0)
  {
    *nreturn_vals = 2;

    image_ID = load_image(param[1].data.d_string);

    if (image_ID != -1)
    {
      values[1].type         = PARAM_IMAGE;
      values[1].data.d_image = image_ID;
    }
    else
      values[0].data.d_status = STATUS_EXECUTION_ERROR;
  }
  else if (strcmp (name, "file_s16_save") == 0)
  {
    *nreturn_vals = 1;

    switch (param[0].data.d_int32)
    {
      case RUN_INTERACTIVE :
         /*
          * Possibly retrieve data...
          */

          gimp_get_data("file_s16_save", &s16vals);

         /*
          * Then acquire information with a dialog...
          */

  //        if (!save_dialog())
            return;
          break;

      case RUN_NONINTERACTIVE :
         /*
          * Make sure all the arguments are there!
          */

          if (nparams != 6)
            values[0].data.d_status = STATUS_CALLING_ERROR;
          else
          {
            s16vals.type555        = param[5].data.d_int32;
          };
          break;

      case RUN_WITH_LAST_VALS :
         /*
          * Possibly retrieve data...
          */

          gimp_get_data("file_s16_save", &s16vals);
          break;

      default :
          break;
    };

    if (values[0].data.d_status == STATUS_SUCCESS)
    {
      if (save_image(param[3].data.d_string, param[1].data.d_int32,
                     param[2].data.d_int32))
        gimp_set_data("file_s16_save", &s16vals, sizeof(s16vals));
      else
	values[0].data.d_status = STATUS_EXECUTION_ERROR;
    };
  }
  else
    values[0].data.d_status = STATUS_EXECUTION_ERROR;
}


/*
 */

#define MAXFRAMES 500

struct FrameRecord 
{
	int Width;
	int Height;
	int Offset;
};

static gint32 load_image( char *filename )
{
	struct FrameRecord s16info[ MAXFRAMES ];

	FILE *fp;
	unsigned long int ltemp;
	unsigned short int wtemp;
	char buf[128];

	int numimages;
	int width,height;
	int i;
	int offset;

	gint32 image;		/* Image */
	gint32 layer;		/* Layer */
	GDrawable *drawable;	/* Drawable for layer */
	GPixelRgn pixel_rgn;	/* Pixel region for layer */
	guchar *pixel;		/* Pixel data */
	guchar *p;
	guint16 *inbuf;	
	int x,y;


	fp = fopen( filename, "r" );
	if( fp == NULL )
		return -1;

	fread( &ltemp, 1, 4, fp );
	fread( &wtemp, 1, 2, fp );
	numimages = wtemp;

	if( numimages > MAXFRAMES )
		return -1;

	sprintf( buf, "%d images\n", numimages );
	g_print( buf );

	for( i=0; i<numimages; i++ )
	{
		fread( &ltemp, 1, 4, fp );
		s16info[i].Offset = ltemp;
		fread( &wtemp, 1, 2, fp );
		s16info[i].Width = wtemp;
		fread( &wtemp, 1, 2, fp );
		s16info[i].Height = wtemp;
	}

	i = 0;

	width = s16info[i].Width;
	height = s16info[i].Height;

	image = gimp_image_new( width, height, RGB );
	if (image == -1)
	{
		g_print("can't allocate new image\n");
		gimp_quit();
	};

	gimp_image_set_filename(image, filename);

	layer = gimp_layer_new(image, "Background", width, height,
		RGB_IMAGE, 100, NORMAL_MODE);
	gimp_image_add_layer( image, layer, 0 );


	/* Get the drawable and set the pixel region for our load... */

	drawable = gimp_drawable_get(layer);

	gimp_pixel_rgn_init(&pixel_rgn, drawable, 0, 0, drawable->width,
		drawable->height, TRUE, FALSE);

	/* Temporary buffer... */
	inbuf = g_new(guint16, width * 2);	/* 2 bpp */
	pixel = g_new(guchar, width * 3);	/* 3 bpp */

	/* This works if you are only reading one row at a time...  */

	for( y = 0; y < height; y++ )
	{
		/* read in a row */
		fread( inbuf, 2, width, fp );

		gimp_pixel_rgn_get_rect(&pixel_rgn, pixel, 0, y, drawable->width, 1);

		p = pixel;
		for( x=0; x<width; x++ )
		{
			*p++ = (((inbuf[x]>>11) & 31)*255)/31;
			*p++ = (((inbuf[x]>>5) & 63)*255)/63;
			*p++ = (((inbuf[x]) & 31)*255)/31;
		}
		gimp_pixel_rgn_set_rect(&pixel_rgn, pixel, 0, y, drawable->width, 1);

		/*
			gimp_progress_update(((double)pass + (double)end / (double)info->height) /
			(double)num_passes);
		*/
	}

	/* * Done with the file...  */

	free(pixel);

	fclose(fp);

	/* * Update the display...  */

	gimp_drawable_flush(drawable);
	gimp_drawable_detach(drawable);

	return (image);
}



#ifdef HACKHACKHACK

static gint32
load_image(char *filename)	/* I - File to load */
{
  int		i,		/* Looping var */
		bpp,		/* Bytes per pixel */
		image_type,	/* Type of image */
		layer_type,	/* Type of drawable/layer */
		num_passes,	/* Number of interlace passes in file */
		pass,		/* Current pass in file */
		tile_height,	/* Height of tile in GIMP */
		begin,		/* Beginning tile row */
		end,		/* Ending tile row */
		num;		/* Number of rows to load */
  FILE		*fp;		/* File pointer */
  gint32	image,		/* Image */
		layer;		/* Layer */
  GDrawable	*drawable;	/* Drawable for layer */
  GPixelRgn	pixel_rgn;	/* Pixel region for layer */
  png_structp	pp;		/* PNG read pointer */
  png_infop	info;		/* PNG info pointers */
  guchar	**pixels,	/* Pixel rows */
		*pixel;		/* Pixel data */
  char		progress[255];	/* Title for progress display... */


 /*
  * Setup the PNG data structures...
  */

#if PNG_LIBPNG_VER > 88
 /*
  * Use the "new" calling convention...
  */

  pp   = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct(pp);
#else
 /*
  * SGI (and others) supply libpng-88 and not -89c...
  */

  pp = (png_structp)calloc(sizeof(png_struct), 1);
  png_read_init(pp);

  info = (png_infop)calloc(sizeof(png_info), 1);
#endif /* PNG_LIBPNG_VER > 88 */

 /*
  * Open the file and initialize the PNG read "engine"...
  */

  fp = fopen(filename, "r");
  if (fp == NULL)
    return (-1);

  png_init_io(pp, fp);

  if (strrchr(filename, '/') != NULL)
    sprintf(progress, "Loading %s:", strrchr(filename, '/') + 1);
  else
    sprintf(progress, "Loading %s:", filename);

  gimp_progress_init(progress);

 /*
  * Get the image dimensions and create the image...
  */

  png_read_info(pp, info);

  if (info->bit_depth < 8)
  {
    png_set_packing(pp);
    png_set_expand(pp);

    if (info->valid & PNG_INFO_sBIT)
      png_set_shift(pp, &(info->sig_bit));
  }
  else if (info->bit_depth == 16)
    png_set_strip_16(pp);

 /*
  * Turn on interlace handling...
  */

  if (info->interlace_type)
    num_passes = png_set_interlace_handling(pp);
  else
    num_passes = 1;
  
  switch (info->color_type)
  {
    case PNG_COLOR_TYPE_RGB :		/* RGB */
        bpp        = 3;
        image_type = RGB;
        layer_type = RGB_IMAGE;
        break;

    case PNG_COLOR_TYPE_RGB_ALPHA :	/* RGBA */
        bpp        = 4;
        image_type = RGB;
        layer_type = RGBA_IMAGE;
        break;

    case PNG_COLOR_TYPE_GRAY :		/* Grayscale */
        bpp        = 1;
        image_type = GRAY;
        layer_type = GRAY_IMAGE;
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA :	/* Grayscale + alpha */
        bpp        = 2;
        image_type = GRAY;
        layer_type = GRAYA_IMAGE;
        break;

    case PNG_COLOR_TYPE_PALETTE :	/* Indexed */
        bpp        = 1;
        image_type = INDEXED;
        layer_type = INDEXED_IMAGE;
        break;
  };

  image = gimp_image_new(info->width, info->height, image_type);
  if (image == -1)
  {
    g_print("can't allocate new image\n");
    gimp_quit();
  };

  gimp_image_set_filename(image, filename);

 /*
  * Load the colormap as necessary...
  */

  if (info->color_type & PNG_COLOR_MASK_PALETTE)
    gimp_image_set_cmap(image, (guchar *)info->palette, info->num_palette);

 /*
  * Create the "background" layer to hold the image...
  */

  layer = gimp_layer_new(image, "Background", info->width, info->height,
                         layer_type, 100, NORMAL_MODE);
  gimp_image_add_layer(image, layer, 0);

 /*
  * Get the drawable and set the pixel region for our load...
  */

  drawable = gimp_drawable_get(layer);

  gimp_pixel_rgn_init(&pixel_rgn, drawable, 0, 0, drawable->width,
                      drawable->height, TRUE, FALSE);

 /*
  * Temporary buffer...
  */

  tile_height = gimp_tile_height ();
  pixel       = g_new(guchar, tile_height * info->width * bpp);
  pixels      = g_new(guchar *, tile_height);

  for (i = 0; i < tile_height; i ++)
    pixels[i] = pixel + info->width * info->channels * i;

  for (pass = 0; pass < num_passes; pass ++)
  {
   /*
    * This works if you are only reading one row at a time...
    */

    for (begin = 0, end = tile_height;
         begin < info->height;
         begin += tile_height, end += tile_height)
    {
      if (end > info->height)
        end = info->height;

      num = end - begin;
	
      if (pass != 0) /* to handle interlaced PiNGs */
        gimp_pixel_rgn_get_rect(&pixel_rgn, pixel, 0, begin, drawable->width, num);

      png_read_rows(pp, pixels, NULL, num);

      gimp_pixel_rgn_set_rect(&pixel_rgn, pixel, 0, begin, drawable->width, num);

      gimp_progress_update(((double)pass + (double)end / (double)info->height) /
                           (double)num_passes);
    };
  };

 /*
  * Done with the file...
  */

  png_read_end(pp, info);
  png_read_destroy(pp, info, NULL);

  free(pixel);
  free(pixels);
  free(pp);
  free(info);

  fclose(fp);

 /*
  * Update the display...
  */

  gimp_drawable_flush(drawable);
  gimp_drawable_detach(drawable);

  return (image);
}

#endif /* HACKHACKHACK */




static gint save_image( char *filename, gint32 image_ID, gint32 drawable_ID )
{
	return 0;
}



#ifdef HACKHACKHACK 
/*
 * 'save_image()' - Save the specified image to a PNG file.
 */

static gint
save_image(char   *filename,	/* I - File to save to */
	   gint32 image_ID,	/* I - Image to save */
	   gint32 drawable_ID)	/* I - Current drawable */
{
  int		i,		/* Looping var */
		bpp,		/* Bytes per pixel */
		type,		/* Type of drawable/layer */
		num_passes,	/* Number of interlace passes in file */
		pass,		/* Current pass in file */
		tile_height,	/* Height of tile in GIMP */
		begin,		/* Beginning tile row */
		end,		/* Ending tile row */
		num;		/* Number of rows to load */
  FILE		*fp;		/* File pointer */
  GDrawable	*drawable;	/* Drawable for layer */
  GPixelRgn	pixel_rgn;	/* Pixel region for layer */
  png_structp	pp;		/* PNG read pointer */
  png_infop	info;		/* PNG info pointer */
  gint		num_colors;	/* Number of colors in colormap */
  guchar	**pixels,	/* Pixel rows */
		*pixel;		/* Pixel data */
  char		progress[255];	/* Title for progress display... */


 /*
  * Open the file and initialize the PNG write "engine"...
  */

  fp = fopen(filename, "w");
  if (fp == NULL)
    return (0);

  if (strrchr(filename, '/') != NULL)
    sprintf(progress, "Saving %s:", strrchr(filename, '/') + 1);
  else
    sprintf(progress, "Saving %s:", filename);

  gimp_progress_init(progress);

 /*
  * Get the drawable for the current image...
  */

  drawable = gimp_drawable_get(drawable_ID);
  type     = gimp_drawable_type(drawable_ID);

  gimp_pixel_rgn_init(&pixel_rgn, drawable, 0, 0, drawable->width,
                      drawable->height, FALSE, FALSE);

 /*
  * Set the image dimensions and save the image...
  */
/*
  info->width          = drawable->width;
  info->height         = drawable->height;
*/

  switch (type)
  {
    case RGB_IMAGE :
        break;
    case RGBA_IMAGE :
        break;
    case GRAY_IMAGE :
        break;
    case GRAYA_IMAGE :
        break;
    case INDEXED_IMAGE :
        break;
  };

  png_write_info(pp, info);

 /*
  * Turn on interlace handling...
  */

  if (pngvals.interlaced)
    num_passes = png_set_interlace_handling(pp);
  else
     num_passes = 1;

 /*
  * Allocate memory for "tile_height" rows and save the image...
  */

  tile_height = gimp_tile_height();
  pixel       = g_new(guchar, tile_height * drawable->width * bpp);
  pixels      = g_new(guchar *, tile_height);

  for (i = 0; i < tile_height; i ++)
    pixels[i]= pixel + drawable->width * bpp * i;

  for (pass = 0; pass < num_passes; pass ++)
  {
   /*
    * This works if you are only writing one row at a time...
    */

    for (begin = 0, end = tile_height;
         begin < drawable->height;
         begin += tile_height, end += tile_height)
    {
      if (end > drawable->height)
        end = drawable->height;

      num = end - begin;

      gimp_pixel_rgn_get_rect(&pixel_rgn, pixel, 0, begin, drawable->width, num);

//      png_write_rows(pp, pixels, num);

      gimp_progress_update(((double)pass + (double)end / (double)info->height) /
                           (double)num_passes);
    };
  };

  png_write_end(pp, info);
  png_write_destroy(pp);

  free(pixel);
  free(pixels);

 /*
  * Done with the file...
  */

  free(pp);
  free(info);

  fclose(fp);

  return (1);
}
#endif /* HACKHACKHACK */

