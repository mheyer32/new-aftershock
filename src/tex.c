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
#include "util.h"
#include "pak.h"
#include "bsp.h"
#include "shader.h"
#include "tex.h"
#include "opengl.h"
#include <jpeglib.h>
#include "render.h"

#define IMG_BUFSIZE (1024*1024-8)



texture_t * r_dynamic_tex=NULL;
int num_dynamic_tex=0;
int *ids=NULL;




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


static void tex_loadtexture(byte *rgb, int w, int h, int format, uint_t flags);

static void *imgbuf;

static void
tex_loadtexture(byte *rgb, int w, int h, int format, uint_t flags)
{
    byte *tex = rgb;
    int width = w, height = h;
    int size = width*height* (format == GL_RGB ? 3 : 4);

    /* Scale image down for biased level of detail (lowered texture quality) */
    if (r_lodbias->integer > 0)
    {
	width /= 1 << r_lodbias->integer;
	height /= 1 << r_lodbias->integer;
	tex = malloc(size);

	gluScaleImage(format, w, h, GL_UNSIGNED_BYTE, rgb,
		      width, height, GL_UNSIGNED_BYTE, tex);
    }

    /* Not really a gamma: prelighten the texture to compensate for
       darkening after lightmap application. */
    /* FIXME: should alpha be brightened too? */
    if (r_gamma->value != 1.0)
    {
	int i, val;

	
	for (i=0; i<size; ++i)
	{
	    val = tex[i] * r_gamma->value;
	    if (val > 255) val = 255;
	    tex[i] = val;
	}
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		    GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (flags & TEXFILE_CLAMP)
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    else
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    if (flags & TEXFILE_NOMIPMAPS)
    {
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
		     GL_UNSIGNED_BYTE, tex);
    }
    else
    {
	gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format,
			  GL_UNSIGNED_BYTE, tex);
    }

    if (r_lodbias > 0)
	free(tex);
}


static void
jpg_noop(j_decompress_ptr cinfo)
{
}

static boolean
jpg_fill_input_buffer(j_decompress_ptr cinfo)
{
    Error("Premeture end of jpeg file");
    return TRUE;
}

static void
jpg_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
        
    cinfo->src->next_input_byte += (size_t) num_bytes;
    cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    if (cinfo->src->bytes_in_buffer < 0)
	Error("Premeture end of jpeg file");
}

static void
jpeg_mem_src(j_decompress_ptr cinfo, byte *mem, int len)
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
	

int tga_read_extern (const char *fname, byte **rgb, int *w, int *h,
		       int *format,void * filebuf)
{
	tgaheader_t *tgahead;
    byte *img, *tga, *tgacur, *tgaend;
    int tgalen, len, depth = 0;

    tgalen = pak_readfile(fname, IMG_BUFSIZE, filebuf);
    if (!tgalen) return 0;

    tga = (byte*)filebuf;
    tgaend = tga + tgalen;
    
    tgahead = (tgaheader_t*)tga;
    tgahead->xorig=LittleShort(tgahead->xorig);
    tgahead->yorig=LittleShort(tgahead->yorig);
    tgahead->width=LittleShort(tgahead->width);
    tgahead->height=LittleShort(tgahead->height);
    if (tgahead->imgtype != 2 && tgahead->imgtype != 10)
	Error("Bad tga image type");

    if (tgahead->pixsize == 24)
	depth = 3;
    else if (tgahead->pixsize == 32)
	depth = 4;
    else
	Error("Non 24 or 32 bit tga image");
    
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
	    if (++tgacur > tgaend)
		Error("Unexpected end of tga file");
	    if (packethead & 0x80)
	    {
		/* Run-length packet */
		packetlen = (packethead & 0x7f) + 1;
		memcpy(rlc, tgacur, depth);
		if ((tgacur += depth) > tgaend)
		    Error("Unexpected end of tga file");
		for (j=0; j < packetlen; ++j)
		    for(i=0; i < depth; ++i)
			*c++ = rlc[i];
	    }
	    else
	    {
		/* Raw data packet */
		packetlen = packethead + 1;
		memcpy(c, tgacur, depth * packetlen);
		if ((tgacur += depth * packetlen) > tgaend)
		    Error("Unexpected end of tga file");
		c += packetlen * depth;
	    }
	}

	/* Flip image in y */
	{
	    int i, linelen;
	    byte *temp;
	    
	    linelen = tgahead->width * depth;
	    temp = malloc(linelen);
	    for (i=0; i < tgahead->height/2; ++i)
	    {
		memcpy(temp, &img[i * linelen], linelen);
		memcpy(&img[i * linelen], &img[(tgahead->height - i - 1)
					      * linelen], linelen);
		memcpy(&img[(tgahead->height - i - 1) * linelen], temp,
		       linelen);
	    }
	    free(temp);
	}	
    }
    else
    {
	int i, linelen;
	
	if (tgaend - tgacur + 1 < len)
	    Error("Bad tga image data length");

	/* Flip image in y */
	linelen = tgahead->width * depth;
	for (i=0; i < tgahead->height; ++i)
	    memcpy(&img[i * linelen],
		   &tgacur[(tgahead->height - i - 1) * linelen], linelen);
    }    

    /* Exchange B and R to get RGBA ordering */
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
    
    *rgb = img;
    *w = tgahead->width;
    *h = tgahead->height;
    *format = (depth == 3) ? GL_RGB : GL_RGBA;
    return 1;
}

int jpg_read_extern(const char *fname, byte **rgb, int *w, int *h, int *format,void * filbuf)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    byte *img, *c;
    int jpglen;

    jpglen = pak_readfile(fname, IMG_BUFSIZE, filbuf);
    if (!jpglen) return 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, filbuf, jpglen);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    if (cinfo.output_components != 3)
	Error("Bad number of jpg components");

    img = c = malloc(cinfo.output_width * cinfo.output_height * 3);
    while (cinfo.output_scanline < cinfo.output_height)
    {
	jpeg_read_scanlines(&cinfo, &c, 1);
	c += cinfo.output_width * 3;
    }

    *rgb = img;
    *w = cinfo.output_width;
    *h = cinfo.output_height;
    *format = GL_RGB;

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    
    return 1;
}


 int
loadtexture_extern(byte *rgb, int w, int h, int format, uint_t flags)
{
    byte *tex;
    int width = w, height = h;
    int size = width*height* (format == GL_RGB ? 3 : 4);


	// not really the right test 
	if (w % 2 || h % 2 )
	{
		return 0;

	}



	if (r_picmip->integer || w>glconfig.maxTextureSize || h> glconfig.maxTextureSize)
    {
		if (!(flags & TEXFILE_NOSCALEDOWN) )
		if (r_picmip->integer > 0)
		{
			width /= 1 << (r_lodbias->integer);
			height /= 1 << (r_lodbias->integer);
		}

		while (width>glconfig.maxTextureSize || height > glconfig.maxTextureSize )
		{
			width /= 1 << 1;
			height /= 1 << 1;
		}
		tex = malloc(size);

		gluScaleImage(format, w, h, GL_UNSIGNED_BYTE, rgb,
		      width, height, GL_UNSIGNED_BYTE, tex);
	}
		else 
		{
			 tex=rgb;
		}






  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (flags & TEXFILE_CLAMP)
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
    else
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

	if (flags & TEXFILE_NOMIPMAPS)
    {
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
		     GL_UNSIGNED_BYTE, tex);
    }
    else
    {
	gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format,
			  GL_UNSIGNED_BYTE, tex);
    }


	return 1;
   
}



 


void  tex_init (void)
{

	r_dynamic_tex=malloc(MAX_TEX *sizeof(texture_t));
	memset(r_dynamic_tex ,0,MAX_TEX *sizeof (texture_t ));
	


	imgbuf=malloc (IMG_BUFSIZE);

	ids=malloc(MAX_TEX *sizeof(int));
		
	glGenTextures(MAX_TEX, ids);
	num_dynamic_tex=0;

}


void tex_shutdown (void )
{
	glDeleteTextures(MAX_TEX, ids);

	free (r_dynamic_tex);
	free (imgbuf);
	free (ids);

	num_dynamic_tex=0;

}




static texture_t * find_free_Texture (int *num )
{

	int i;


	for (i=0;i<MAX_TEX;i++)
	{

		if (!r_dynamic_tex[i].inuse)
		{
			*num =i;
			return &r_dynamic_tex[i];

		}


	}


	*num=0;
	Error (" No more free textures ! ");

	return NULL;




}


void texture_free ( int id )
{
	int i;

	if (id <0) return ;

	for (i=0;i<MAX_TEX;i++)
	{

		if (r_dynamic_tex[i].inuse  && r_dynamic_tex[i].id==id)
		{
			r_dynamic_tex[i].inuse=0;
			num_dynamic_tex--;
		}
	}

}

int complete_texture_load (const char * name ,const int flags)
{

	char * filename=malloc(128);

	char *rgb=NULL;
	int found=0,width,height,format,res=0,i,num;
	texture_t * tex =find_free_Texture(&num);


	COM_StripExtension(name,filename);
	
		// misuse of imgbuf : dont care ? 
	found=pak_Search(filename , imgbuf ,2048);

	if (!found) 
	{
		free(filename);
		return -1;
	}


	strcpy(filename,imgbuf);


	// Check if the Texture already exists 
		for (i=0;i<num_dynamic_tex;i++)
	{
		if (!strcmp(r_dynamic_tex[i].fname,filename))
		{
			
			free(filename);
			return r_dynamic_tex[i].id;
		}


	}
	


	if (!strcmp(".tga",filename+strlen(filename)-strlen(".tga")))
	{
	
		res=tga_read_extern(filename, &rgb, &width, &height,
			     &format,imgbuf);
	}
	else if (!strcmp(".jpg",filename+strlen(filename)-strlen(".jpg")))
	{

		res=jpg_read_extern(filename, &rgb, &width, &height,
  			     &format,imgbuf);


	}
	else 
	{
		res=0;
	}
	if (!res)
	{
		if (rgb)
		free(rgb);
		free(filename);
		return -1;


	}


	// not really the right test 



	GL_BindTexture(GL_TEXTURE_2D, ids[num]);
	if (!loadtexture_extern(rgb, width, height, format, flags))
	{
		
		if (rgb)
		free(rgb);
		free(filename);
		return -1;


	}
	
	

	tex->Flags=flags;
	tex->Handle=ids[num];
	tex->id=ids[num];
	tex->inuse=1;
	strcpy(tex->fname,filename);

	num_dynamic_tex++;


	if (rgb)
		free(rgb);
	free(filename);

	

	return ids[num];





}

