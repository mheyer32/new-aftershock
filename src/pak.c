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

/*
 * These are facilities to manipulate pk3 files.  It uses Gilles Vollant's
 * unzip extensions to zlib (source included with distribution).
 */

#define MAX_PAKFILES	1024

static unzFile pakfiles[MAX_PAKFILES];
static unzFile actfile;
static int pak_numfiles = 0;

void Pak_Init (void)
{
	pak_numfiles = 0;
	memset (pakfiles, 0, MAX_PAKFILES * sizeof (unzFile));
}

void Pak_Shutdown (void)
{
	int i;

	for (i = 0; i < pak_numfiles; i++)
		unzClose (pakfiles[i]);

	pak_numfiles = 0;
}

int Pak_OpenPak (const char *path)
{
	if (!path || !path[0])
		return 0;

	if (pak_numfiles == MAX_PAKFILES) {
		Error ("Pak overflow!");
		return 0;
	}

	pakfiles[pak_numfiles] = unzOpen(path);

	if (!pakfiles[pak_numfiles])
		return 0;

	pak_numfiles++;

	return 1;
}

int Pak_OpenFile (const char *path)
{
	int i;

	for (i = 0; i < pak_numfiles; i++)
	{
		if (unzLocateFile(pakfiles[i], path, 2) == UNZ_OK)
		{
			if (unzOpenCurrentFile(pakfiles[i]) == UNZ_OK)
			{
				actfile = pakfiles[i];
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
	int i, state = 0;
	int found = 0, allfound = 0;
	
	for (i = 0;i < pak_numfiles; i++)
	{
		found = Unz_GetStringForDir(pakfiles[i], dir, extension, str + alllen, bufsize - alllen, &len);

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
	int i;

	for (i = 0; i < pak_numfiles; i++)
		if (Unz_FileExists(pakfiles[i], file))
			return atrue;

	return afalse;
}

