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
#include "sys_main.h"
#include "console.h"
#include "c_var.h"
#include "opengl.h"
#include "render.h"
#include "command.h"
#include "ui.h"
#include "game.h"
#include "cgame.h"
#include "shader.h"
#include "io.h"
#include "server.h"
#include "keys.h"
#include "client.h"
#include "server.h"
#include "network.h"

cvar_t *com_maxfps;

typedef struct cvarTable_s {
	cvar_t	**cvar;
	char	*name;
	char	*resetString;
	int		flags;
} cvarTable_t;


static cvarTable_t cvarTable [] =
{
	{&com_maxfps, "com_maxfps", "85", CVAR_ARCHIVE}
};

const static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

void Engine_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

// TODO !!!
void Error (const char *fmt, ...)
{
	char buf[1024];
    va_list ap;

    va_start(ap, fmt);    
    Com_sprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

	MessageBox(NULL, buf, "ERROR", MB_OK);

	CL_Shutdown();
	
    exit(1);
}

void Engine_Shutdown (void)
{
	// TODO !
/*
	Cbuf_AddText("writeconfig\n");
	Cbuf_Execute();
*/
	SV_Shutdown();
	CL_Shutdown();
	Net_Shutdown();
	Cbuf_Free();
	Cmd_Shutdown();
	Con_Shutdown();
	Cvar_Shutdown();  
	Key_Shutdown();
	FS_Shutdown();
}

static void Cmd_Quit (void)
{
	Engine_Shutdown ();
	exit (0);
}

aboolean Engine_Init (void)
{
	Key_ClearStates();
	Swap_Init();
	Con_Init();
	FS_Init("baseq3");
	Key_Init();
	Cmd_Init();
	Cbuf_Init();
	Cvar_Init();
	
	// essential commands
	Cmd_AddCommand("quit", Cmd_Quit);

	Cbuf_AddText("exec q3config.cfg\n");
	Cbuf_Execute();

	Engine_GetCvars();

	if (!SV_Init("FIXME")) {
		Com_Error ( ERR_FATAL, "Could not initialize server subsystem");
		return afalse;
	}

	if (!CL_Init()) {
		Com_Error ( ERR_FATAL, "Could not initialize client subsystem");
		return afalse;
	}
	
	Con_Printf("... Common Initialization Complete ...\n");

	if (!Net_Init())
	{
		Com_Error ( ERR_FATAL, "Could not initialize networking");
		return afalse;
	}

	FS_GetWorkingDir();

	return atrue;
}

void Engine_Render_Next_Frame (void)
{
	static uint_t LastTime = 0;
	static uint_t LastFPS = 0, numframes = 0;
	uint_t Now = Sys_Get_Time();
	uint_t Diff;
	
	if (!LastTime) 
	{
		LastTime = Now;
		return;
	}

	if (!LastFPS)
	{
		LastFPS = Now;
		return;
	}

	Diff = Now - LastTime;

	if (Diff > (uint_t )(1000.0 / (float)com_maxfps->integer))
	{
		LastTime = Now;
		cl_frametime = (double)Now / 1000.0;

		numframes++;
		Cbuf_Execute();
		CL_Run_Frame();
	}

	Diff = Now - LastFPS;

	if (Diff > 1000)
	{
		Con_DPrintf ("FPS: %i\n", numframes);
		numframes = 0;
		LastFPS = Now;
	}
}
