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

#include <windows.h>
#include "a_shared.h"
#include "sys_file.h"
#include "sys_mem.h"
#include "pak.h"
#include "io.h"
#include "unzip.h"
#include "console.h"
#include "c_var.h"

static file_t		files[MAX_FILES];
static int			files_used[MAX_FILES];
static int			num_files = 0;
static char			basedir[MAX_OSPATH];
static char			workdir[MAX_OSPATH];
static int			fs_load_stack;

cvar_t	*fs_game;

char *FS_Add_Basedir (const char *s)
{
	static char buf [128];

	Com_sprintf (buf, sizeof(buf), "%s/%s", basedir, s);

	return buf;
}

char *FS_Add_Gamedir (const char *s)
{
	static char buf [128];

	Com_sprintf (buf, sizeof(buf), "%s/%s", basedir, s);

	return buf;
}

int Get_FirstFreeFile (void)
{
	int i;

	for (i = 0; i < MAX_FILES; i++)
	{
		if (!files_used[i])
			return i;
	}

	Com_Error ( ERR_FATAL, "Too much files !");
	return 0;
}

/*
================
FS_Flush
================
*/
void FS_Flush( int f )
{
	File_FlushBuffers(files[f-1].fhandle);
}

int FS_OpenFile (const char *path, int *handle, fsMode_t mode)
{
	int fnum = Get_FirstFreeFile ();
	file_t *f = &files[fnum];
	void *h;
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

	buf = FS_Add_Basedir (path);
	h = File_Open(buf, mode);
	
	if (h)
	{
		files_used[fnum] = 1;
		f->fhandle = h;
		f->type = FILE_NORMAL;

		*handle = fnum +1;
		A_strncpyz (f->name, path, MAX_APATH);
		return File_GetLen (h);
	}
	else	
		if (Pak_FileExists(path))
		{
			files_used[fnum] = 1;
			f->fhandle = NULL;
			f->type = FILE_IN_PAK;

			*handle = fnum +1;
			A_strncpyz (f->name, path, MAX_APATH);
			return Pak_GetFileLen(path);
		}

	*handle = 0;
	
	Con_Printf (S_COLOR_YELLOW "WARNING: Could not open file: %s\n", path);
	return 0;
}

void FS_Read(void *buffer, int len, int handle) 
{
	file_t *f;

	if (handle < 1 || handle > MAX_FILES)
		return;

	if (!buffer)
		Com_Error ( ERR_FATAL, "FS_Read: buffer = NULL !");
	
	f = &files[handle - 1];
	
	switch (f->type)
	{
		case FILE_NORMAL:
			File_Read (buffer, len, f->fhandle);
			break;

		case FILE_IN_PAK:
			if (!Pak_ReadFile (f->name,len,buffer))
				Con_Printf (S_COLOR_YELLOW "WARNING: Could not read file %s\n", f->name);
			break;

		default:
			Com_Error ( ERR_FATAL, "FS_Read: Bad file type!");
			break;
	}
}

int FS_Write(const void *buffer, int len, int handle) 
{
	file_t *f;
	int byteswritten = 0;

	if (handle < 1 || handle > MAX_APATH) {
		Con_Printf ("FS_Write: Wrong file handle\n");
		return 0;
	}

	if (!buffer) {
		Con_Printf ("FS_Write: buffer = NULL\n");
		return 0;
	}

	f = &files[handle-1];

	if (f->type != FILE_NORMAL)
	{
		Con_Printf ("FS_Write: Unknown file type\n");
		return 0;
	}

	byteswritten = File_Write(buffer, len, f->fhandle);

	return (byteswritten == len);
}

void FS_FCloseFile(int handle) 
{
	file_t *f;

	if (handle < 1 || handle > MAX_FILES)
		return;

	f = &files [handle - 1];

	if (f->type == FILE_NORMAL)
		File_Close(f->fhandle);

	files_used[handle - 1] = 0;
}

/*
===========
FS_ReadFile
===========
*/
int FS_ReadFile( const char *path, void **buffer )
{
	int				size;
	fileHandle_t	f;

	if( !path || !path[0] ) {
		Com_Error( ERR_FATAL, "FS_ReadFile with empty name\n" );
	}

	size = FS_OpenFile( path, &f, OPEN_READONLY );
	if( !f ) {
		if( buffer ) {
			*buffer = NULL;
		}
		return( -1 );
	}

	if( !buffer ) {
		FS_FCloseFile( f );
		return( size );
	}

	fs_load_stack++;

	*buffer = Hunk_AllocateTempMemory( size+1 );

	FS_Read( *buffer, size, f );
	((byte *)(*buffer))[size] = 0;

	FS_FCloseFile( f );

	return size;
}

/*
===========
FS_FreeFile
===========
*/
void FS_FreeFile( void *buffer )
{
	if( !buffer ) {
		Com_Error(ERR_FATAL, "FS_FreeFile( NULL )\n");
	}

	fs_load_stack--;

	Hunk_FreeTempMemory( buffer );

	if( !fs_load_stack ) {
		Hunk_ClearTempMemory();
	}
}


void FS_Shutdown(void)
{
	Pak_Shutdown ();
}

void FS_Init (const char *dir)
{
	void *handle;
	char buf[MAX_OSPATH];
	char fname[MAX_OSPATH];
	char buf2[MAX_OSPATH];
	char dir2[MAX_OSPATH];

	Con_Printf("------- FS_INIT: -------\n");

	fs_game = Cvar_Get( "fs_game", "baseq3", 0 );

	Pak_Init ();

	strcpy (basedir, dir);

	// Find all pk3 files in the base-directory
	Com_sprintf (buf, sizeof(buf), "%s/*.pk3", dir);

	handle = File_FindFirst (buf, fname, 0, 0);

	if (handle)
	{
		Com_sprintf (buf2, sizeof(buf2), "%s/%s", dir, fname);

		if (!Pak_OpenPak(buf2))
			Com_Error ( ERR_FATAL, "Could not open pk3: %s", fname);
	}
	else {
		Com_Error ( ERR_FATAL, "Could not find any .pk3 files!");
	}
	
	while (File_FindNext(handle, fname, 0, 0))
	{
		Com_sprintf (buf2, sizeof(buf2), "%s/%s", dir, fname);
		Pak_OpenPak(buf2);
	}

	File_FindClose(handle);

	if (!strcmp (fs_game->string, "baseq3"))
		return;

	// Find all pk3 files in the base-directory
	Com_sprintf (dir2, sizeof(dir2), "%s", "LostArena");
	Com_sprintf (buf, sizeof(buf), "%s/*.pk3", dir2);

	handle = File_FindFirst (buf, fname, 0, 0);

	if (handle)
	{
		Com_sprintf (buf2, sizeof(buf2), "%s/%s", dir2, fname);

		if (!Pak_OpenPak(buf2))
			Com_Error ( ERR_FATAL, "Could not open pk3: %s", fname);
	}
	else {
		return;
	}
	
	while (File_FindNext(handle, fname, 0, 0))
	{
		Com_sprintf (buf2, sizeof(buf2), "%s/%s", dir2, fname);
		Pak_OpenPak(buf2);
	}

	File_FindClose(handle);

	Con_Printf ("----- FS_INIT:Success --------\n");
}

// TODO !
aboolean FS_FileExists (char *file)
{
	return Pak_FileExists(file);
}

void FS_GetWorkingDir (void)
{
    // get the current directory
    GetCurrentDirectory(MAX_OSPATH, workdir);

    // form the path string
    Con_Printf("Working directory: %s\n", workdir);
}

// Works, but could be faster 
int FS_GetFileList (const char *path, const char *extension, char *listbuf, int bufsize)
{
	char *bufpos;
	int found = 0, buflen = 0;
	char tmp[MAX_OSPATH];
	int len = 0;
	void *handle;
	char *syspath;
	char fname[MAX_OSPATH];

	listbuf[0] = 0;
	bufpos = listbuf;

	// FIXME !!!
	if (!path[0])
		return 0;

	syspath = FS_Add_Basedir (path);
	
	Com_sprintf (tmp, sizeof(tmp), "%s/*.%s", syspath, extension);	
	handle = File_FindFirst (tmp, fname, 0, 0);
	
	if (handle)
	{
		// TODO !!!
		File_FindClose (handle);
	}

	found = Pak_GetFileList(path, extension, listbuf, bufsize);

	return found;
}

int FS_FileSize (const char *name)
{
	int len, file;

	len = FS_OpenFile (name, &file, FS_READ);

	if (!len || !file )
		return 0;
	
	FS_FCloseFile(file);

	return len;
}

/*
================
FS_LoadStack
================
*/
int FS_LoadStack( void )
{
	return( fs_load_stack );
}

/*
================
FS_Restart
================
*/
void FS_Restart(void)
{
	FS_Shutdown();
	FS_Init("baseq3");
}

