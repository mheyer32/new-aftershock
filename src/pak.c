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
#include "pak.h"
#include "unzip.h"
#include "console.h"
#include "command.h"

/*
 * These are facilities to manipulate pk3 files.  It uses Gilles Vollant's
 * unzip extensions to zlib (source included with distribution).
 */

typedef struct pak_s
{
	char	name[MAX_OSPATH];
	unzFile pakfile;
	int		numentries;
	struct pak_s *next;
} pak_t;

static unzFile actfile;
static pak_t   *pakfile;

void Pak_Init (void)
{
	pakfile = NULL;

	Cmd_AddCommand ("paklist", Pak_List);
}

void Pak_Shutdown (void)
{
	pak_t *pakfile0, *pak = pakfile;

	while (pak->next)
	{
		pakfile0 = pak;
		pak = pak->next;
		free (pakfile0);
	}
}

void Pak_List (void)
{
	pak_t *pak = pakfile;

	for ( ; pak->next; pak = pak->next)
	{
		Con_Printf ("%s, %i entries\n", pak->name, pak->numentries);
	}
}

aboolean Pak_OpenPak (const char *path)
{
	pak_t *pak;

	if (!path || !path[0])
		return afalse;

	pak = (pak_t *)malloc (sizeof(pak_t));
	memset (pak, 0, sizeof(pak_t));

	pak->pakfile = unzOpen(path);

	if (!pak->pakfile)
	{
		free (pak);
		return afalse;
	}

	pak->numentries = Unz_NumEntries (pak->pakfile);

	if (!pak->numentries)
	{
		free (pak);
		return afalse;
	}

	A_strncpyz (pak->name, path, MAX_OSPATH);
	pak->next = pakfile;
	pakfile = pak;

	return atrue;
}

int Pak_OpenFile (const char *path)
{
	pak_t *pak;

	for (pak = pakfile; pak->next; pak = pak->next)
	{
		if (unzLocateFile(pak->pakfile, path, 2) == UNZ_OK)
		{
			if (unzOpenCurrentFile(pak->pakfile) == UNZ_OK)
			{
				actfile = pak->pakfile;
				return 1;
			}
		}
	}

	return 0;
}

int Pak_GetFileLen (const char *path)
{
	unz_file_info info;

	if (!Pak_OpenFile (path))
		return 0;

	if (unzGetCurrentFileInfo(actfile, &info, NULL, 0, NULL, 0, NULL, 0)!= UNZ_OK)
		return 0;

	unzCloseCurrentFile(actfile);

    return info.uncompressed_size;
}

int Pak_ReadFile (const char *path, int len, void *buf)
{
	int read;
	
	if (!Pak_OpenFile (path))
		return 0;

	read = unzReadCurrentFile(actfile, buf, len);

	unzCloseCurrentFile(actfile);

	return (len == read);
}

int Pak_GetFileList (const char *dir, const char *extension, char *str, int bufsize)
{
	int len = 0, alllen = 0;
	int state = 0;
	int found = 0, allfound = 0;
	pak_t *pak;

	for (pak = pakfile; pak->next; pak = pak->next)
	{
		found = Unz_GetStringForDir(pak->pakfile, dir, extension, str + alllen, bufsize - alllen, &len);

		if (found)
		{
			allfound += found;
			state = 1;
			alllen += len + 1;
		}
	}

	return allfound;
}

aboolean Pak_FileExists (const char *file)
{
	pak_t *pak;

	for (pak = pakfile; pak->next; pak = pak->next)
		if (Unz_FileExists(pak->pakfile, file))
			return atrue;

	return afalse;
}

