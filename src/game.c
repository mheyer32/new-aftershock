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
#include "game.h"
#include "io.h"

int GAME_Call (int arg,...);
int (__cdecl *GAME_syscall)( int arg, ... ) = GAME_Call;

char GAME_name[] = "qagamex86.dll";

EntryFunc *GAME_entry;
MainFunc *GAME_main;

static int gameLoaded = 0;

int LoadGame (void)
{
	if (gameLoaded)
		return 1;

	GAME_inst = LoadLibrary(FS_Add_Basedir(GAME_name));

	if (!GAME_inst) 
		return 0;

	GAME_entry = (EntryFunc*) GetProcAddress(
			GAME_inst,   
            "dllEntry"   
	);

	if (!GAME_entry) 
		return 0;
 
	GAME_main = (MainFunc*) GetProcAddress(
			GAME_inst,    
            "vmMain"   
	);

	if (!GAME_main) 
		return 0;
	
	GAME_entry(GAME_syscall);

	gameLoaded = 1;
	return 1;
}

int UnLoadGame (void)
{
	if (!gameLoaded) 
		return 0;

	if (!GAME_inst)
		return 0;

	FreeLibrary(GAME_inst);

	gameLoaded = 0;
	return 1;
}