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
#include "util.h"
#include "a_shared.h"
#include "pak.h"
#include "unzip.h"
#include "console.h"

/*
 * These are facilities to manipulate pk3 files.  It uses Gilles Vollant's
 * unzip extensions to zlib (source included with distribution).
 */


#define MAX_PAKFILES 64



static unzFile pakfiles [MAX_PAKFILES] ;
static char basedir[MAX_STRING_CHARS];
static int handles_in_use [MAX_PAKFILES];
static unzFile actfile;



void pak_init (char * dir)
{
	int i;
	char file[MAX_OSPATH];

	A_strncpyz(basedir,dir,1024);
	memset(pakfiles,0,sizeof (unzFile)*MAX_PAKFILES);
	memset(handles_in_use,0,sizeof(int)*MAX_PAKFILES);

	actfile=NULL;



// open all pk3 files in the form pak0.pk3,pak1.pk3 ...

	for (i=0; ;i++)
	{
		
		sprintf(file,"%s/pak%i.pk3",dir,i);
		
		if (!pak_openpak(file))
		{
			
			if (i==0)
			{
				Error("Could not find any pakfiles in basedir %s",dir);


			}
			break;


		}
	}


	Con_Printf("found %i pk3 files in basedir %s\n",i,dir);



}


static int Find_free_Handle (void )
{
	int i;
	for (i=0;i<MAX_PAKFILES;i++)
	{
		if (!handles_in_use [i])
			return i;


	}


	return -1;



}


int
pak_openpak(const char *path)
{
	int pak=Find_free_Handle();

	if (pak<0)
	{
		Error("NO More free pak-Handles !");
	}

    pakfiles[pak] = unzOpen(path);
    if (!pakfiles[pak])
	return 0;

	handles_in_use[pak]=1;
	return 1;
}

void
pak_closepak(int num)
{
    if (pakfiles[num])
	unzClose(pakfiles[num]);
	
    pakfiles[num] = NULL;
	
	handles_in_use[num]=0;
}

void pak_closeall (void)
{
	int i;

	for (i=0;i<MAX_PAKFILES;i++)
	{
	if (handles_in_use[i])
	{
	unzClose(pakfiles[i]);
    pakfiles[i] = NULL;
	}
	}

}
int
pak_open(const char *path)
{

	char buf [4096];
	int i;

	for (i=0;i<MAX_PAKFILES;i++)
	{
		
		if (handles_in_use[i])
		{
			
			if (Unz_Search (pakfiles[i] ,path,buf,4096))
			{
			if (unzLocateFile(pakfiles[i], buf, 2) == UNZ_OK)
			{
				if (unzOpenCurrentFile(pakfiles[i]) == UNZ_OK)
				{
					actfile=pakfiles[i];
					return 1;
				}
			}
			}

		}


		}
   return 0;
}

void
pak_close(void)
{
    unzCloseCurrentFile(actfile);
}

int
pak_read(void *buf, uint_t size, uint_t nmemb)
{
    int len;

    len = unzReadCurrentFile(actfile, buf, size * nmemb);
    return len / size;
}

uint_t
pak_getlen(void)
{
    unz_file_info info;

    if (unzGetCurrentFileInfo(actfile, &info, NULL, 0, NULL, 0, NULL, 0)
	!= UNZ_OK)
	return 0;
    return info.uncompressed_size;
}

uint_t
pak_readfile(const char *path, uint_t bufsize, byte *buf)
{
    uint_t len;
    
    if (!pak_open(path))
	return 0;

    if ((len = pak_getlen()) != 0)
    {
	if (pak_read((void*)buf, 1, len) != len)
	    len = 0;
    }
    pak_close();    
    return len;
}
/*
uint_t
pak_listshaders(uint_t bufsize, char *buf)
{
    uint_t num = 0, len;
    int status;
    char *end = buf + bufsize;

    status = unzGoToFirstFile(pakfiles[0]);
    while (status == UNZ_OK)
    {
	unzGetCurrentFileInfo(pakfiles[0], NULL, buf, 64, NULL, 0, NULL, 0);
	len = strlen(buf);
	if (len >= 7 && strcmp(&buf[len-7], ".shader") == 0)
	{
	    num++;
	    buf += len + 1;
	    if (buf > end)
		Error("pak_listshaders(): buffer overrun");
	}
	status = unzGoToNextFile(pakfiles[0]);
    }

    return num;
}
*/
int
pak_checkfile(const char *path)
{
    int status;

    status = unzLocateFile(actfile, path, 2);
    return (status == UNZ_OK);
}

// FIXME : MULTI PAK SUPPORT !

int pak_FileListForDir(char * list ,const char * directory,const char *ext)
{
	
	return Get_NameListforDir(pakfiles[0],list,directory,ext);
	

}
int pak_Search(const char * name , char * buf ,int bufsize)
{
	return Unz_Search (pakfiles[0] ,name,buf,bufsize);

}
