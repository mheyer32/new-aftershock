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
#include "a_shared.h"
#include "console.h"
#include "c_var.h"
#include "render.h"
#include "ui.h"
#include "command.h"
#include "cgame.h"
#include "keys.h"


cvar_t *cl_running ;
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
	int	flags;
} cvarTable_t;


static cvarTable_t cvarTable [] =
{
	{&cl_running , "cl_running","0",CVAR_ARCHIVE},
	{&cl_cdkey, "cl_cdkey","" ,CVAR_ARCHIVE},
	{&cl_yawspeed,"cl_yawspeed","100", CVAR_ARCHIVE },
	{&cl_pitchspeed,"cl_pitchspeed","100",CVAR_ARCHIVE},
	{&cl_maxpackets,"cl_maxpackets","30",CVAR_ARCHIVE},
	{&cl_packetdup,"cl_packetdup","1",CVAR_ARCHIVE},
	{&cl_mouseAccel, "cl_mouseAccel","0",CVAR_ARCHIVE},
	{&cl_maxPing,"cl_maxPing","800",CVAR_ARCHIVE}



};

const static int	cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);


static int client_prepared =0;



void CL_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

void Cmd_vid_restart (void )
{
	client_prepared=0;

	R_Shutdown ();
	R_Init ();

	client_prepared=1;

	UI_main(UI_SHUTDOWN,0,0,0,0,0,0,0);
	UI_main(UI_INIT,0,0,0,0,0,0,0);
	UI_main(UI_SET_ACTIVE_MENU,UIMENU_MAIN,0,0,0,0,0,0 );
}



int CL_Init (void )
{

	Con_Printf (" ------ Client Initialization -------  \n");


	R_Init();

	// TODO :  SOUND 

	CL_GetCvars();
	Cmd_AddCommand ("vid_restart",Cmd_vid_restart );

	if (!LoadUI ())
	{
	
		client_prepared=0;
		Error ("FAILED to load UI ! ");

	}


	client_prepared=1;

	Con_Printf(" ------ Client Initialization Successful ---- \n" );


	return 1;

}


int CL_Shutdown (void )
{

	if (! client_prepared )
		return 0;

	Con_Printf(" ------ Client Shutdown --------  \n");


	R_Shutdown ();



	
	if (!UnLoadUI()) 
		return 0;


	client_prepared=0;

	return 1;


	

}

int CL_Startup (void )
{

	if (! client_prepared)
		Error ("CL_Starup : not initialised ");





	LoadCGAME ();
	CGAME_main(CG_INIT,0,0,0,0,0,0,0);


	Cvar_Set ("cl_running","1");

	return 1;

}

int CL_End_Gaming (void )
{
	if (!cl_running->integer)
		Error ( "CL_End_Gaming : not running ");


	CGAME_main (CG_SHUTDOWN ,0,0,0,0,0,0,0);
	UnLoadCGAME();



	Cvar_Set ("cl_running","0");


	return 1;
}

void CL_Run_Frame (void )
{
		int catcher ;

		if (! client_prepared)
			return ;



		R_StartFrame();

	 
		Cbuf_Execute();

			
		catcher=Key_GetCatcher ();

		
		if (catcher & KEYCATCH_UI )
		UI_main(UI_REFRESH,(int)(g_frametime*1000.0),0,0,0,0,0,0 );



		Con_Draw();


		R_EndFrame ();
}


