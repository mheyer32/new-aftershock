/* Aftershock 3D rendering engine
 * Copyright (C) 1999 Stephen C. Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "a_shared.h"
#include "shader.h"
#include "tex.h"
#include "opengl.h"
#include "jpeglib.h"
#include "render.h"
#include "io.h"
#include "console.h"

#define IMG_BUFSIZE (512 * 512 * 4)

#define MAX_TEXTURES 512 

texture_t *r_dynamic_tex = NULL;
int num_dynamic_tex = 0;
int *ids = NULL;

static byte img_buf[IMG_BUFSIZE];
static int r_num_textures = TEX_PREGEN_LAST;
static texture_t textures[MAX_TEXTURES];
static uint_t r_textures_id[MAX_TEX];

typedef struct
{
    byte idlen;
    byte cmtype;
    byte imgtype;
    byte cmspec[5];
    ushort_t xorig, yorig;
    ushort_t width, height;
    byte pixsize;
    byte imgdesc;
} tgaheader_t;


int round_2 (int x)
{
	int v = 2;

	while (x > v)
		v *= 2;
	
	return v;
}

static void tex_loadtexture(byte *rgb, int w, int h, int format, uint_t flags);
static void *imgbuf;

static void jpg_noop(j_decompress_ptr cinfo)
{
}

static boolean jpg_fill_input_buffer(j_decompress_ptr cinfo)
{
    Error("Premeture end of jpeg file");
    return TRUE;
}

static void jpg_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
        
    cinfo->src->next_input_byte += (size_t) num_bytes;
    cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    if (cinfo->src->bytes_in_buffer < 0)
	Error("Premeture end of jpeg file");
}

static void jpeg_mem_src(j_decompress_ptr cinfo, byte *mem, int len)
{
    cinfo->src = (struct jpeg_source_mgr *)
	(*cinfo->mem->alloc_small)((j_common_ptr) cinfo,
				   JPOOL_PERMANENT,
				   sizeof(struct jpeg_source_mgr));
    cinfo->src->init_source = jpg_noop;
    cinfo->src->fill_input_buffer = jpg_fill_input_buffer;
    cinfo->src->skip_input_data = jpg_skip_input_data;
    cinfo->src->resync_to_restart = jpeg_resync_to_restart;
    cinfo->src->term_source = jpg_noop;
    cinfo->src->bytes_in_buffer = len;
    cinfo->src->next_input_byte = mem;
}
	

static char *Find_Texture (const char *filename)
{
	char path[MAX_APATH];
	char base[MAX_APATH];
	char dirlist[200 * MAX_APATH];
	char s_base[MAX_APATH];
	static char buf[MAX_APATH];
	int numdirs;
	char *dirptr;
	int i;
	int dirlen;

	strcpy(buf, filename);

	if (FS_FileExists(buf) ) {
		return buf;
	}

	COM_ExtractFilePath(buf, path);
	COM_ExtractFileBase(buf, base);

	/* remove the slash */
	path[strlen(path) - 1] = 0;

	numdirs = FS_GetFileList(path, "", dirlist, sizeof(dirlist));

	dirptr = dirlist;

	for (i = 0; i < numdirs; i++, dirptr += dirlen + 1) {
		dirlen = strlen(dirptr);
		COM_ExtractFileBase(dirptr, s_base);

		if( !A_stricmp(base, s_base) ) {
			sprintf(buf, "%s/%s", path, dirptr);
			return buf;
		}
	}

	return 0;
}

void Tex_Init (void)
{
	int i = 0;

	r_num_textures = TEX_PREGEN_LAST;
	memset (textures, 0, MAX_TEXTURES * sizeof(texture_t));

	glGenTextures (MAX_TEXTURES, r_textures_id);

	for (i = 0; i < TEX_PREGEN_LAST; i++)
		r_textures_id[i] = -1;
}

void Tex_Shutdown (void)
{
	glDeleteTextures (MAX_TEXTURES, r_textures_id);

	r_num_textures = 0;
	memset (textures, 0, MAX_TEXTURES * sizeof(texture_t));
}

////////////////////////////////////////////
// TODO !!!!!!!!!!!!!!!!!!!!!
////////////////////////////////////////////////
byte *Tex_Load_TGA (const char *fname, int *width, int *height, int *format)
{
	int file;
	tgaheader_t *tgahead;
    byte *img, *tga, *tgaend, *tgacur;
    int tgalen, len, depth = 0;

    tgalen = FS_OpenFile (fname, &file, FS_READ);

	if (!tgalen || !file)
		return NULL;

	FS_Read (img_buf, tgalen, file);
	FS_FCloseFile (file);

    tga = (byte *)img_buf;
    tgaend = tga + tgalen;
    
    tgahead = (tgaheader_t *)tga;
    tgahead->xorig = LittleShort(tgahead->xorig);
    tgahead->yorig = LittleShort(tgahead->yorig);
    tgahead->width = LittleShort(tgahead->width);
    tgahead->height = LittleShort(tgahead->height);

    if (tgahead->imgtype != 2 && tgahead->imgtype != 10) {
//		Con_Printf("Bad tga image type\n");
		return NULL;
	}

    if (tgahead->pixsize == 24)
		depth = 3;
    else if (tgahead->pixsize == 32)
		depth = 4;
    else {
		Con_Printf("Non 24 or 32 bit tga image\n");
  		return NULL;
	}
  
    len = tgahead->width * tgahead->height * depth;
    img = malloc(len);

    tgacur = tga + sizeof(tgaheader_t) + tgahead->idlen;

    if (tgahead->imgtype == 10)
    {
		int i, j, packetlen;
		byte packethead;
		byte *c = img, *end = img + len;
		byte rlc[4];
	
		while (c < end)
		{
			packethead = *tgacur;

			if (++tgacur > tgaend) {
				Con_Printf("Unexpected end of tga file\n");
				free (img);
				return NULL;
			}

			if (packethead & 0x80)
			{
				// Run-length packet
				packetlen = (packethead & 0x7f) + 1;
				memcpy(rlc, tgacur, depth);

				if ((tgacur += depth) > tgaend) {
					Con_Printf("Unexpected end of tga file\n");
					free (img);
					return NULL;
				}

				for (j=0; j < packetlen; ++j)
					for(i=0; i < depth; ++i)
						*c++ = rlc[i];
			}
			else
			{
				// Raw data packet
				packetlen = packethead + 1;
				memcpy(c, tgacur, depth * packetlen);

				if ((tgacur += depth * packetlen) > tgaend) {
					Con_Printf("Unexpected end of tga file\n");
					free (img);
					return NULL;
				}
				c += packetlen * depth;
			}
		}

		// Flip image in y
		{
			int i, linelen;
			byte *temp;
	    
			linelen = tgahead->width * depth;
			temp = malloc(linelen);

			for (i = 0; i < tgahead->height/2; ++i)
			{
				memcpy(temp, &img[i * linelen], linelen);
				memcpy(&img[i * linelen], &img[(tgahead->height - i - 1) * linelen], linelen);
				memcpy(&img[(tgahead->height - i - 1) * linelen], temp, linelen);
			}

			free(temp);
		}	
    }
    else
    {
		int i, linelen;
		
		if (tgaend - tgacur + 1 < len) {
			Con_Printf("Bad tga image data length\n");
			free (img);
			return NULL;
		}

		// Flip image in y
		linelen = tgahead->width * depth;
		for (i=0; i < tgahead->height; ++i)
			memcpy(&img[i * linelen],
			   &tgacur[(tgahead->height - i - 1) * linelen], linelen);
    }    

    // Exchange B and R to get RGBA ordering
    {
		int i;
		byte temp;

		for (i=0; i < len; i += depth)
		{
			temp = img[i];
			img[i] = img[i+2];
			img[i+2] = temp;
		}
    }
    
    *width = tgahead->width;
    *height = tgahead->height;
    *format = (depth == 3) ? GL_RGB : GL_RGBA;

    return img;
}

// TODO !!!
// Does not work yet !
byte *Tex_Load_TGA2 (const char *fname, int *width, int *height, int *format)
{
	int fsize;
	int file;
	tgaheader_t *tgahead;
	byte *where = NULL;
	byte *img = NULL;
	int size = 0, i = 0;
	byte temp;

	// Read the File
	fsize = FS_OpenFile (fname, &file, FS_READ);

	if (!file || !fsize)
		return NULL;

	FS_Read (img_buf, fsize, file);
	FS_FCloseFile (file);

	tgahead = (void *)img_buf;

	where = img_buf;

	// Swap the header
	tgahead->height = LittleShort (tgahead->height);
	tgahead->width = LittleShort (tgahead->width);
	tgahead->xorig = LittleShort (tgahead->xorig);
	tgahead->yorig = LittleShort (tgahead->yorig);

	where += sizeof (tgahead) + tgahead->idlen;

	if (tgahead->cmtype != 0)
		return NULL;

	if ((tgahead->imgtype != 2) && (tgahead->imgtype != 3))
		return NULL;

	size = tgahead->width * tgahead->height;

	switch (tgahead->pixsize)
	{
		case 32:
			img = malloc (size * 4);
			*format = GL_RGBA;
			
			memcpy (img, where, size * 4);
			
			for (i = 0; i < size * 4; i += 4)
			{
				temp = img[i];
				img[i] = img[i+2];
				img[i+2] = temp;
			}
			
			break;

		case 24:
			img = malloc (size * 3);
			*format = GL_RGB;
			
			memcpy (img, where, size * 3);
			
			for (i = 0; i < size * 3; i += 3)
			{
				temp = img[i];
				img[i] = img[i + 2];
				img[i+2] = temp;
			}
			
			break;

		default:
			return NULL;
	}

	*width = tgahead->width;
	*height = tgahead->height;

	return img;
}

byte *Tex_Load_JPG (const char *fname, int *width, int *height, int *format)
{
	int file;
	struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
	byte *img, *c;
	int len = FS_OpenFile (fname, &file, FS_READ);

	if (!len || !file)
		return NULL;

	FS_Read (img_buf, len, file);
	FS_FCloseFile (file);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, img_buf, len);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    if (cinfo.output_components != 3) {
		Con_Printf("Bad number of jpg components\n");
		jpeg_finish_decompress(&cinfo);
	    jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

    img = c = malloc(cinfo.output_width * cinfo.output_height * 3);
    while (cinfo.output_scanline < cinfo.output_height)
    {
		jpeg_read_scanlines(&cinfo, &c, 1);
		c += cinfo.output_width * 3;
    }

    *width = cinfo.output_width;
    *height = cinfo.output_height;
    *format = GL_RGB;

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return img;
}

int Tex_UploadTexture (byte **data, int width, int height, int format, int flags)
{
	int internal_format;
	int w = round_2(width);
	int h = round_2(height);

	// Scale the image
	if (glconfig.maxTextureSize < w )
		w = glconfig.maxTextureSize;

	if (glconfig.maxTextureSize < h )
		h = glconfig.maxTextureSize;

	if (!(flags & SHADER_NOPICMIP))
	{
		int i;

		for (i = 0; i < r_picmip->integer; i++)
		{
			if ((w > 4) && (h > 4))
			{
				w >>= 1;
				h >>= 1;
			}
		}
	}

	// Set the internal_format
	if (r_ext_compress_textures->integer)
	{
		if (gl_ext_info._GL_S3_s3tc)
		{
			if (format == GL_RGB)
				internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			else 
				internal_format = (r_texturebits->integer <= 16) ? GL_COMPRESSED_RGBA_S3TC_DXT3_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		else if (gl_ext_info._GL_3DFX_texture_compression_FXT1)
		{
			if (format == GL_RGB)
				internal_format = GL_COMPRESSED_RGB_FXT1_3DFX;
			else 
				internal_format = GL_COMPRESSED_RGBA_FXT1_3DFX;
		}
		else
		{
			if (format == GL_RGB)
				internal_format = (r_texturebits->integer <= 16) ? GL_RGB5 : GL_RGB8;
			else if (format == GL_RGBA)
				internal_format = (r_texturebits->integer <= 16) ? GL_RGBA4 : GL_RGBA8;
			else 
				return 0;
		}
	}
	else
	{
		if (format == GL_RGB)
			internal_format = (r_texturebits->integer <= 16) ? GL_RGB5 : GL_RGB8;
		else if (format == GL_RGBA)
			internal_format = (r_texturebits->integer <= 16) ? GL_RGBA4 : GL_RGBA8;
		else 
			return 0;
	}

	GL_BindTexture(GL_TEXTURE_2D, r_textures_id[r_num_textures]);
	
	if ((w != width) || (h != height))
	{
		byte *tmp = *data;
		byte *scaled = malloc (w * h * ((format == GL_RGB) ? 3 : 4 ));

		if (GL_ScaleImage(format, width, height, GL_UNSIGNED_BYTE, tmp, w,
			h, GL_UNSIGNED_BYTE, scaled))
		{
			free (tmp);
			free (scaled);
			return 0;
		}

		free (tmp);

		*data = scaled;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (!(flags & SHADER_NOMIPMAPS) ) {
		if ( !A_stricmp(r_textureMode->string, "GL_NEAREST"))
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		else if ( !A_stricmp(r_textureMode->string, "GL_LINEAR"))
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		else if ( !A_stricmp(r_textureMode->string, "GL_NEAREST_MIPMAP_NEAREST"))
		    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else if ( !A_stricmp(r_textureMode->string, "GL_NEAREST_MIPMAP_LINEAR"))
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
		else if ( !A_stricmp(r_textureMode->string, "GL_LINEAR_MIPMAP_NEAREST"))
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		else if ( !A_stricmp(r_textureMode->string, "GL_LINEAR_MIPMAP_LINEAR"))
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}	
	
	if (flags & SHADER_CLAMP)
    {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    else
    {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

	// Final Step
	if (flags & SHADER_NOMIPMAPS )
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format,	w, h, 0, format, GL_UNSIGNED_BYTE, *data);
    else
		GL_Build2DMipmaps(GL_TEXTURE_2D, internal_format, w, h, format, GL_UNSIGNED_BYTE, *data);

	return 1;
}

int R_Load_Texture (const char *name, int flags)
{
	char fname[MAX_APATH];
	byte *data = NULL;
	int img_width = 0, img_height = 0, format = GL_RGB, i;

	if (!name[0] || !name)
		return -1;

	A_strncpyz (fname, name, MAX_APATH);

	for (i = 0; i < strlen(fname); i++)
		fname[i] = tolower (fname[i]);

	// Check if already loaded 
	for (i = 0; i < r_num_textures; i++)
	{
		if (!A_stricmp(textures[i].name, fname))
			return r_textures_id[i];
	}

	if (r_num_textures == MAX_TEX) 
	{
		Error ("Out of Textures!");
		return -1;
	}

	if (!A_stricmp(fname, "*white") || !A_stricmp (fname, "white"))
	{
		// wasn't uploaded yet
		if (r_textures_id[TEX_PREGEN_WHITE] == -1)
		{
			img_width = 32;
			img_height = 32;

			data = malloc (img_width * img_height * 3);
			
			memset (data, 255, img_width * img_height * 3);

			glGenTextures (1, &r_textures_id[TEX_PREGEN_WHITE]);

			if (!Tex_UploadTexture (&data, img_width, img_height, format, flags))
			{
				Con_Printf ("WARNING: Could not upload texture %s\n", name);
				free (data);
				return -1;
			}

			return r_textures_id[TEX_PREGEN_WHITE];
		}
		else {
			return r_textures_id[TEX_PREGEN_WHITE];
		}
	}
	else if (!A_stricmp(fname, "*identityLight")) 
	{
		// wasn't uploaded yet
		if (r_textures_id[TEX_PREGEN_IDENTITYLIGHT] == -1)
		{
			byte c = 255;

			img_width = 32;
			img_height = 32;
			format = GL_RGB;

			data = malloc (img_width * img_height * 3);

			memset (data, c, img_width * img_height * 3);
			glGenTextures (1, &r_textures_id[TEX_PREGEN_IDENTITYLIGHT]);

			if (!Tex_UploadTexture (&data, img_width, img_height, format, flags))
			{
				Con_Printf ("WARNING: Could not upload texture %s\n", name);
				free (data);
				return -1;
			}

			return r_textures_id[TEX_PREGEN_IDENTITYLIGHT];
		}
		else {
			return r_textures_id[TEX_PREGEN_IDENTITYLIGHT];
		}
	}
	else if (!A_stricmp(fname, "*scratch"))
	{
		return -1;
	}
	else if (!A_stricmp (fname, "$lightmap"))
	{
		return -1;
	}
	else
	{
		char ext[3];
		char *tex_name = Find_Texture (fname);
		
		if (!tex_name)
		{
			Con_Printf ("WARNING: Could not load texture %s\n", name);
			return -1;
		}

		COM_ExtractFileExtension (tex_name, ext);	
		
		if (!ext[0])
		{
			Con_Printf ("%s\n", fname);

			// .tga by default
			A_strncpyz (ext, "tga", 3);
			A_strcat (fname, MAX_APATH, ".tga");
		}
		
		if (!A_stricmp (ext, "tga"))
		{
			data = Tex_Load_TGA (tex_name, &img_width, &img_height, &format);
			
			if (!data)
			{
				char newtexname[MAX_APATH];

				// Might still be a jpg file !!! (compatibility with old
				// shader scripts?)
				A_strncpyz (newtexname, tex_name, MAX_APATH);
				A_strncpyz (&newtexname[strlen(newtexname)-3], "jpg", MAX_APATH);

				data = Tex_Load_JPG (newtexname, &img_width, &img_height, &format);

				if (!data)
				{
					Con_Printf ("WARNING: Could not load texture %s\n", name);
					return -1;
				}
			}
		}
		else if (!A_stricmp (ext, "jpg"))
		{
			data = Tex_Load_JPG (tex_name, &img_width, &img_height, &format);
			
			if (!data)
			{
				Con_Printf ("WARNING: Could not load texture %s\n", name);
				return -1;
			}
		}
		else 
		{
			Con_Printf ("WARNING: Could not load texture %s\n", name);
			return -1;
		}
	}
	
	if (!Tex_UploadTexture (&data, img_width, img_height, format, flags))
	{
		Con_Printf ("WARNING: Could not upload texture %s\n", name);
		free (data);
		return -1;
	}

	A_strncpyz (textures[r_num_textures].name, name, MAX_APATH);

	free (data);

	// Success !
	return r_textures_id[r_num_textures++];
}
