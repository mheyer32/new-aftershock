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
#include <windows.h>
#include "util.h"
#include "pak.h"
#include "io.h"
#include "unzip.h"
#include "console.h"



static int fileHandle =1;

# define OPEN_READONLY			0
# define OPEN_WRITEONLY			1
# define OPEN_READWRITE			2
# define OPEN_APPEND			3




typedef struct file_s {

	int handle ;
	char *name ;
	int inPak;
	int flags;
	HANDLE real_file;

	struct file_s *next;

}file_t;




file_t * openFiles=NULL;

#define MAX_BASEDIRS 64 





static char main_basedir [MAX_OSPATH];
static char basedirs [MAX_BASEDIRS][MAX_OSPATH];





file_t * FindFile (int handle)
{

	file_t *file;

	for (file=openFiles;file;file=file->next)
	{

		if (file->handle== handle )
			return file;

	}




	return NULL;
}


int DelFile (int handle)
{
	file_t *file,*prev=NULL,*next=NULL;

	for (file=openFiles;file;file=file->next)
	{

		if (file->handle== handle )
		{
				next=file->next;

				free(file->name);
				

				if (!file->inPak)
				{
					
					CloseHandle(file->real_file);


				}
				

				free(file);

				if (prev)
				prev->next=next;

				return 1;
		}
			
		prev=file;

	}


// File was already deleted:
	return 0;


}

/*
static int OpenRealFile (file_t * file ,const char * path ,fsMode_t mode)
{

	int sharemode=0;
	int creation_mode=0;
	int access=0;
	HANDLE file_id;

	
	switch (mode) {
		case OPEN_READONLY:
			access = GENERIC_READ;
			creation_mode = OPEN_EXISTING;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_READONLY;
			break;
		case OPEN_WRITEONLY:
			access = GENERIC_WRITE;
			creation_mode = CREATE_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		case OPEN_READWRITE:
			access = GENERIC_READ|GENERIC_WRITE;
			creation_mode = CREATE_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		case OPEN_APPEND:
			access = GENERIC_WRITE;
			creation_mode = OPEN_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		default:
			return(0);
	}


	file_id = CreateFile(path, access, FILE_SHARE_READ, NULL, creation_mode, flags, NULL);


		if (file_id==INVALID_HANDLE_VALUE)
		{

			file=NULL;
			return 0;

		}


		file=malloc(sizeof(file_t));
		file->name=malloc (MAX_OSPATH);
		strcpy(file->name,path);
		file->flags=flags;
		file->handle=fileHandle;
		file->next=openFiles;
		file->inPak=0;
		file->real_file=file_id;





}
*/


// returns lenght of file:
int FOpenFile(const char *path,int  *handle, fsMode_t mode ) 
{

	HANDLE file_id;
	file_t *file;
	int res=1,flags=0;
		int openmode=0;
		int sharemode=0;
		int creation_mode=0;
		int access;


	res=pak_open(path);

	if (res) {
	
	


	file=malloc(sizeof(file_t));
	file->name=malloc (MAX_OSPATH);
	strcpy(file->name,path);
	file->flags=mode;
	file->handle=fileHandle;
	file->next=openFiles;
	file->inPak=1;
	file->real_file=NULL;

	openFiles=file;
	
	*handle=fileHandle;
	fileHandle++;

	return (int) pak_getlen();

	}
	else 
	{
	

		
	switch (mode) {
		case OPEN_READONLY:
			access = GENERIC_READ;
			creation_mode = OPEN_EXISTING;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_READONLY;
			break;
		case OPEN_WRITEONLY:
			access = GENERIC_WRITE;
			creation_mode = CREATE_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		case OPEN_READWRITE:
			access = GENERIC_READ|GENERIC_WRITE;
			creation_mode = CREATE_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		case OPEN_APPEND:
			access = GENERIC_WRITE;
			creation_mode = OPEN_ALWAYS;
			flags = FILE_FLAG_RANDOM_ACCESS|FILE_ATTRIBUTE_NORMAL;
			break;
		default:
			return(0);
	}

	file_id = CreateFile(path, access, FILE_SHARE_READ, NULL, creation_mode, flags, NULL);


		if (file_id==INVALID_HANDLE_VALUE)
		{
			*handle=0;
			return 0;

		}

		file=malloc(sizeof(file_t));
		file->name=malloc (MAX_OSPATH);
		strcpy(file->name,path);
		file->flags=flags;
		file->handle=fileHandle;
		file->next=openFiles;
		file->inPak=0;
		file->real_file=file_id;

		openFiles=file;
	
		*handle=fileHandle;
		fileHandle++;



		return GetFileSize(file_id,NULL);
	}



}

void FS_Read(void *buffer,int len ,int  handle) 
{
	file_t *file;

	file=FindFile(handle);


	if (file->inPak)
	{

	pak_open(file->name);

	pak_read(buffer,len,1);
	}
	else
	{

		ReadFile(file->real_file,buffer,len,&len,NULL);

	}
	
}

int FS_Write( const void *buffer, int len, int  handle ) 
{
	file_t * file =FindFile (handle);
	int byteswritten=0;

	if (!file) return 0;

	if (file->inPak) return 0;


	
	WriteFile(file->real_file,buffer,len,&byteswritten,0);

	return (byteswritten== len);

}

void FS_FCloseFile( int  handle ) 
{

	DelFile(handle);






}

void FS_Shutdown(void)
{

	pak_closeall();



}

void FS_Init (char *dir )
{
	Con_Printf("   ------- FS_INIT: -------    \n");

	A_strncpyz(main_basedir,dir,MAX_OSPATH);
	pak_init(dir);


}


int  FS_GetFileList (const char *path,const char *extension,char *listbuf,int bufsize)
{
	int numfiles;

	numfiles= pak_FileListForDir(listbuf,path,extension);


	return numfiles;

}


int FS_FileSize (const char * name )
{


	pak_open(name); 
	return pak_getlen();



}






