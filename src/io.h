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

#ifndef IO_H__
#define IO_H__

#define MAX_FILES 128 

# define OPEN_READONLY			0
# define OPEN_WRITEONLY			1
# define OPEN_READWRITE			2
# define OPEN_APPEND			3

typedef enum {
	FILE_NORMAL,
	FILE_IN_PAK
} filetype_t;

typedef struct {
	char name[MAX_OSPATH];
	filetype_t type;
	void *fhandle;
} file_t;

int FS_OpenFile(const char *path, int  *handle,fsMode_t mode) ;
void FS_Read(void *buffer,int len,int  handle) ;
int FS_Write( const void *buffer, int len, int  handle );
int  FS_GetFileList (const char *path, const char *extension,char *listbuf,int bufsize);
void FS_FCloseFile( int  handle ) ;
int FS_Search (const char *path ,char * buf);
int FS_FileExists (char * file );
int FS_FileSize (const char * name );

void FS_Init (const char *dir );
void FS_Shutdown(void);

void FS_Restart(void);

char *FS_Add_Basedir (const char * s);












#endif 