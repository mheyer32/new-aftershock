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
#include "sys_file.h"
#include "util.h"
#include "pak.h"
#include "io.h"
#include "unzip.h"
#include "console.h"


#define MAX_FILES 128 



# define OPEN_READONLY			0
# define OPEN_WRITEONLY			1
# define OPEN_READWRITE			2
# define OPEN_APPEND			3





typedef enum {
	FILE_NORMAL,
	FILE_IN_PAK
}filetype_t;

typedef struct {
	char name [MAX_OSPATH];
	filetype_t type;
	void * fhandle ;
}file_t;


static file_t files [MAX_FILES ];
static int files_used [MAX_FILES ];
static int num_files=0;


static char basedir [MAX_OSPATH];


char *FS_Add_Basedir (const char * s)
{
	static char buf [128];

	sprintf (buf,"%s/%s",basedir,s);

	return buf;
}




int Get_FirstFreeFile (void )
{
	int i;
	for (i=0;i<MAX_FILES;i++)
	{
		if (!files_used[i])
			return i;
	}

	Error ("Too much files !");
	return 0;
}



int FS_OpenFile (const char *path, int  *handle,fsMode_t mode)
{
	int fnum =Get_FirstFreeFile ();
	file_t *f = &files[fnum];
	void * h;
	char *buf;

	if (!path || !handle )
		return 0;


	switch(mode) {
		case FS_READ:
			mode = OPEN_READONLY;
			break;
		case FS_WRITE:
			mode = OPEN_WRITEONLY;
			break;
		case FS_APPEND:
			mode = OPEN_APPEND;
			break;
		case FS_APPEND_SYNC:
			/* TODO */
			mode = OPEN_APPEND;
			break;
		default:
			break;
	}

	buf=FS_Add_Basedir (path);
	h=File_Open(buf,mode);
	
	
	if (h)
	{
		files_used[fnum]=1;
		f->fhandle=h;
		f->type = FILE_NORMAL;

		*handle = fnum +1;
		A_strncpyz (f->name,path,MAX_APATH);
		return File_GetLen (h);
	}
	else	
	if (Pak_FileExists(path))
	{
		files_used[fnum]=1;
		f->fhandle=NULL;
		f->type=FILE_IN_PAK;

		*handle = fnum +1;
		A_strncpyz (f->name,path,MAX_APATH);
		return Pak_GetFileLen(path);
	}

	*handle = 0;
	
	Con_Printf ("WARNING :Could not open file : %s \n",path);

	return 0;

}


void FS_Read(void *buffer,int len ,int  handle) 
{
	file_t *f;

	if (handle <1 || handle > MAX_FILES )
		return;

	if (!buffer )
		Error ("FS_Read : buffer = NULL ! ");
	
	f = &files [handle-1];
	
	switch (f->type)
	{
	case FILE_NORMAL:
		File_Read (buffer,len,f->fhandle);
		break;



	case FILE_IN_PAK:
		if (!Pak_ReadFile (f->name,len,buffer))
			Con_Printf ("WARNING: Could not read file %s\n",f->name);

		break;

	default :
		Error ("FS_Read : Bad file type ! ");
	}

}



int FS_Write( const void *buffer, int len, int  handle ) 
{
	file_t * f ;
	int byteswritten=0;

	if (handle < 1 || handle > MAX_APATH)
		return 0;

	if (!buffer )
		Error ("FS_Write : buffer = NULL !");

	f= &files [handle];

	if (f->type != FILE_NORMAL)
		return 0;
	
	byteswritten =File_Write( buffer,len,f->fhandle);

	return (byteswritten == len);
}

void FS_FCloseFile( int  handle ) 
{
	file_t *f;

	if (handle<1 || handle > MAX_FILES)
		return ;

	f = &files [handle -1];

	if (f->type==FILE_NORMAL)
		File_Close(f->fhandle);

	files_used [handle-1] =0;


}

void FS_Shutdown(void)
{

	Pak_Shutdown ();


}

void FS_Init (const char *dir )
{
	void * handle;
	char buf [MAX_OSPATH];
	char fname [MAX_OSPATH];
	char buf2[MAX_OSPATH];

	Con_Printf("   ------- FS_INIT: -------    \n");

	if (!Pak_Init ())
		return ;

	strcpy (basedir,dir );


	// Find all pk3 files in the base-directory :

	sprintf (buf,"%s/*.pk3",dir);

	handle=File_FindFirst (buf,fname,0,0);

	if (handle)
	{
		sprintf (buf2,"%s/%s",dir,fname);
		if (!Pak_OpenPak(buf2))
			Error ("Could not open pk3 : %s ",fname);
	}
	else 
	{
		Error ("Could not find any .pk3 files !");
	}
	
	while(File_FindNext(handle,fname,0,0))
	{
		printf (buf2,"%s/%s",dir,fname);
			Pak_OpenPak(buf2);
		
	}

	File_FindClose(handle);

	Con_Printf ("-----  FS_INIT :Success --------\n");


}

// TODO !
int FS_FileExists (char * file )
{
	

	return Pak_FileExists(file);

}




// Works , but could be faster 
int  FS_GetFileList (const char *path,const char *extension,char *listbuf,int bufsize)
{

	char *bufpos;
	int found=0,buflen=0;
	char tmp [MAX_OSPATH];
	int len=0;
	void * handle ;
	char *syspath;
	char fname [MAX_OSPATH];

	listbuf[0]=0;
	bufpos=listbuf;

	// FIXME !!!
	if (!path[0])
		return 0;

	
	syspath=FS_Add_Basedir (path);
	
	sprintf (tmp,"%s/*.%s",syspath,extension);	

	handle=File_FindFirst (tmp,fname,0,0);
	
	if (handle)
	{
		// TODO !!!

		File_FindClose (handle);
	}



	found=Pak_GetFileList(path,extension,listbuf,bufsize);
	

	return found;
}


int FS_FileSize (const char * name )
{
	int len ,file;;


	len = FS_OpenFile (name,&file,FS_READ);

	if (!len || !file )
		return 0;
	
	FS_FCloseFile(file );

	return len ;

}






