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
#include "glinc.h"
#include "render.h"
#include "opengl.h"
#include "ui.h"
#include "io.h"

int UI_Call (int arg, ...);
int (__cdecl *UI_syscall)( int arg, ... ) = UI_Call;

char UI_name[] = "uix86.dll";

EntryFunc *UI_entry;
MainFunc *UI_main;
extern void *UI_inst;

static aboolean uiLoaded = afalse;


aboolean LoadUI (void)
{
	if (uiLoaded) 
		return atrue;

	UI_inst = LoadLibrary(FS_Add_Basedir(UI_name));

	if (!UI_inst) 
	{
		UI_inst = LoadLibrary(FS_Add_Gamedir(UI_name));

		if (!UI_inst) 
			return 0;
	}

	UI_entry = (EntryFunc *)GetProcAddress(UI_inst, "dllEntry");

	if (!UI_entry) 
		return afalse;
 
	UI_main = (MainFunc *)GetProcAddress(UI_inst, "vmMain");

	if (!UI_main)
		return afalse;
	
	UI_entry(UI_syscall);
	
	uiLoaded = atrue;

	return atrue;
}

aboolean UnLoadUI (void)
{
	if (!uiLoaded)
		return afalse;

	if (!UI_inst)
		return afalse;

	FreeLibrary(UI_inst);
	
	uiLoaded = afalse;

	return atrue;
}

void UI_KeyEvent (int Key )
{
	UI_main(UI_KEY_EVENT,Key,0,0,0,0,0,0);
}

void UI_MouseEvent (int x, int y )
{
	UI_main(UI_MOUSE_EVENT,x,y,0,0,0,0,0);	  
}

void UI_Refresh (void )
{
	UI_main(UI_REFRESH,(int)(cl_frametime*1000.0),0,0,0,0,0,0 );
}

void UI_Restart (void)
{
	if (!UnLoadUI())
		return;

	if (!LoadUI())
		return;

	// Init UI
	UI_main(UI_INIT, 0, 0, 0, 0, 0, 0, 0);
	UI_main(UI_SET_ACTIVE_MENU, UIMENU_MAIN, 0, 0, 0, 0, 0, 0);
}