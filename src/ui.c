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




#include "util.h"
#include "bsp.h"
#include "vec.h"
#include "glinc.h"
#include "render.h"
#include "opengl.h"
#include "ui.h"

#include <windows.h>

int UI_Call (int arg, ...);
int (__cdecl *UI_syscall)( int arg, ... ) = UI_Call;

char  UI_name[]="uix86.dll";

EntryFunc *UI_entry;
MainFunc *UI_main;
extern HINSTANCE UI_inst;


static int uiLoaded=0;


int LoadUI (void )
{

	if (uiLoaded) return 1;

		UI_inst= LoadLibrary(UI_name );
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














