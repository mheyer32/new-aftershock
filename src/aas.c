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
#include "io.h"
#include "console.h"
#include "aas.h"

static byte 		*aas_data = NULL;
static aas_header_t *aas_header = NULL;
aas_t aas;

/*
=================
AAS_Load_Map
=================
*/
aboolean AAS_Load_Map (char *mapname)
{
	int len, file;
	char fname[MAX_APATH];

	COM_StripExtension(mapname, fname);
	strcat(fname, ".aas");

	len = FS_OpenFile (fname, &file, FS_READ);

	if (!len)
	{
		Con_Printf ("Could not find aas file: %s\n", mapname);
		return afalse;
	}

	aas_data = malloc (len);
	
	FS_Read (aas_data, len, file);

	FS_FCloseFile (file);

	aas_header = (aas_header_t *)aas_data; 

	if (aas_header->version == AAS_VERSION)
	{
		// do nothing
	}
#ifdef AAS_SUPPORTV4
	else if (aas_header->version == AAS_VERSION_)
	{
		// "decrypt"
		int i;
		unsigned char *buf = (unsigned char *)&(aas_header->crc);

		for (i = 0; i < sizeof(aas_header_t) - 8; i++, buf++) 
			*buf ^= i * 0x77;
	}
#endif
	else
		Con_Printf ("%s has wrong version: %s\n", fname, aas_header->version);

	memset (&aas, 0, sizeof(aas_t));

	// TODO

	return atrue;
}

void AAS_Free (void)
{
	if (!aas_data) return; free (aas_data); aas_data = NULL;
	if (!aas_header) return; free (aas_header); aas_header = NULL;

	memset (&aas, 0, sizeof(aas_t));
}
