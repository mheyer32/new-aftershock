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
#include "cgame.h"
#include "io.h"

EntryFunc *CGAME_entry;
MainFunc *CGAME_main;

int CGAME_Call (int arg, ...);
int (__cdecl *CGAME_syscall)( int arg, ... ) = CGAME_Call;

char CGAME_name[] = "cgamex86.dll";
HINSTANCE UI_inst, GAME_inst, CGAME_inst;

static aboolean cgameLoaded = afalse;

aboolean LoadCGAME (void)
{
	if (cgameLoaded) 
		return atrue;

	CGAME_inst = LoadLibrary(FS_Add_Basedir(CGAME_name));

	if (!CGAME_inst) 
		return afalse;

	CGAME_entry = (EntryFunc *)GetProcAddress(CGAME_inst, "dllEntry");

	if (!CGAME_entry)
		return afalse;

	CGAME_main = (MainFunc *)GetProcAddress(CGAME_inst, "vmMain");

	if (!CGAME_main) 
		return afalse;
	
	CGAME_entry(CGAME_syscall);
	
	cgameLoaded = atrue;

	return atrue;
}

aboolean UnLoadCGAME (void)
{
	if (!cgameLoaded) 
		return afalse;

	if (!CGAME_inst)
		return afalse;

	FreeLibrary(CGAME_inst);

	cgameLoaded = afalse;

	return atrue;
}