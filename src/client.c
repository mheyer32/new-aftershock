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
#include "console.h"
#include "c_var.h"
#include "render.h"
#include "sound.h"
#include "ui.h"
#include "command.h"
#include "cgame.h"
#include "cmap.h"
#include "keys.h"
#include "game.h"
#include "server.h"

cvar_t *cl_running;
cvar_t *cl_cdkey;
cvar_t *cl_yawspeed;
cvar_t *cl_pitchspeed;
cvar_t *cl_maxpackets;
cvar_t *cl_packetdup;
cvar_t *cl_mouseAccel;
cvar_t *cl_maxPing;


typedef struct cvarTable_s {
	cvar_t	**cvar;
	char	*name;
	char	*resetString;
	int		flags;
} cvarTable_t;

static cvarTable_t cvarTable [] =
{
	{&cl_running, "cl_running", "0", CVAR_ARCHIVE},
	{&cl_cdkey, "cl_cdkey", "", CVAR_ARCHIVE},
	{&cl_yawspeed, "cl_yawspeed", "100", CVAR_ARCHIVE },
	{&cl_pitchspeed, "cl_pitchspeed", "100", CVAR_ARCHIVE},
	{&cl_maxpackets, "cl_maxpackets", "30", CVAR_ARCHIVE},
	{&cl_packetdup, "cl_packetdup", "1", CVAR_ARCHIVE},
	{&cl_mouseAccel, "cl_mouseAccel", "0", CVAR_ARCHIVE},
	{&cl_maxPing, "cl_maxPing", "800", CVAR_ARCHIVE}
};

const static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

client_t cl;

static aboolean client_prepared = afalse;
static int realtime = 0;

void CL_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

static void Cmd_vid_restart (void)
{
	client_prepared = afalse;

	R_Shutdown();
	R_Init();

	client_prepared = atrue;

//	Key_SetCatcher( Key_GetCatcher() & ~KEYCATCH_CONSOLE );

	if (Key_GetCatcher () & KEYCATCH_UI) {
//		UI_main(UI_SET_ACTIVE_MENU, UIMENU_MAIN, 0, 0, 0, 0, 0, 0 );
		UI_Restart();
		Key_SetCatcher( KEYCATCH_UI );
	}
}

static void Cmd_demo (void)
{
	char demo_name [MAX_OSPATH];

	Cmd_Argv(1, demo_name, MAX_OSPATH);
}

aboolean CL_Init (void)
{
	Con_Printf ("------ Client Initialization -------\n");

	R_Init();
	S_Init();

	cl.connstate = CA_UNINITIALIZED;
//	cl.demo_playing = afalse;

	CL_GetCvars();
	Cmd_AddCommand ("vid_restart",Cmd_vid_restart );
	Cmd_AddCommand ("demo",Cmd_demo);

	if (!LoadUI ())
	{
		client_prepared = afalse;
		Error ("FAILED to load UI!");
		return afalse;
	}

	client_prepared = atrue;

	cl_frametime = 0.0;
	cl.connstate = CA_DISCONNECTED; 	// not talking to a server
	
	// Init UI
	UI_main(UI_INIT, 0, 0, 0, 0, 0, 0, 0);
	UI_main(UI_SET_ACTIVE_MENU, UIMENU_MAIN, 0, 0, 0, 0, 0, 0);

	return atrue;
}

aboolean CL_Shutdown (void)
{
	if (!client_prepared)
		return afalse;

	Con_Printf("------ Client Shutdown --------\n");

	R_Shutdown ();
	S_Shutdown ();

	if (!UnLoadUI()) 
		return afalse;

	client_prepared = afalse;

	return atrue;
}

void CL_Startup (void)
{
	if (!client_prepared) {
		Error ("CL_Starup: not initialised");
		return;
	}

	if (!LoadCGAME ())
	{
		Error ("Couldn't load cgame");
		return;
	}

	CGAME_main(CG_INIT, 0, 0, 0, 0, 0, 0, 0);

	Cvar_Set ("cl_running", "1");

	// Vic: FIXME!!!
	cl.connstate = CA_CONNECTED;
}

void CL_End_Gaming (void)
{
	if (!cl_running->integer) {
		Error ("CL_End_Gaming: not running");
		return;
	}

	CGAME_main (CG_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
	UnLoadCGAME();

	Cvar_Set ("cl_running", "0");
}

void CL_Run_Frame (void)
{
	if (!client_prepared)
		return;
	
	R_StartFrame();
	
	switch (cl.connstate)
	{
		case CA_UNINITIALIZED:
		case CA_DISCONNECTED: 	
			if (Key_GetCatcher () & KEYCATCH_UI)
				UI_Refresh ();
			break;	// Vic: ?

		case CA_AUTHORIZING:
			break;

		case CA_CONNECTING:		
			break;

		case CA_CHALLENGING:		
			break;

		case CA_CONNECTED:
			{
				realtime += (cl_frametime * 1000);
				CGAME_main (CG_DRAW_ACTIVE_FRAME, realtime, 0, 0, 0, 0, 0, 0);
//				R_Draw_World();
			}
			break;

		case CA_LOADING:		
			break;

		case CA_PRIMED:
			break;

		case CA_ACTIVE:
			break;

		case CA_CINEMATIC:
			break;
	} 
	
	Con_Draw();
	
	R_EndFrame ();
}


void CL_KeyEvent (int Key)
{
	char buf[MAX_STRING_CHARS];

	if (!cl_running->integer)
		return;

	Key_GetBindingBuf (Key, buf, MAX_STRING_CHARS);

	if (!buf[0])
		return;
	
	// TODO !!!
}