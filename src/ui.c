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
#include "util.h"
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
extern void * UI_inst;


static int uiLoaded=0;


int LoadUI (void )
{

	if (uiLoaded) return 1;

		UI_inst= LoadLibrary(FS_Add_Basedir(UI_name) );
	if (!UI_inst) return 0;

	UI_entry=(EntryFunc*) GetProcAddress(
			UI_inst,   
            "dllEntry"   
	);
	if (!UI_entry) return 0;
 

	UI_main=(MainFunc*) GetProcAddress(
			UI_inst,    
            "vmMain"   
	);

	if (!UI_main) return 0;
	
	UI_entry(UI_syscall);
	
	uiLoaded=1;
	return 1;

}


int UnLoadUI (void )
{

	if (!uiLoaded)
		return 0;

	if (!UI_inst )
		return 0;



	FreeLibrary(UI_inst);
	
	uiLoaded=0;

	return 1;
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
	UI_main(UI_REFRESH,(int)(g_frametime*1000.0),0,0,0,0,0,0 );
}





