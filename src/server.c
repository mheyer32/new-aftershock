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
#include "util.h"
#include "network.h"
#include "game.h"
#include "c_var.h"
#include "command.h"
#include "console.h"
#include "bsp.h"
#include "server.h"
#include "client.h"



/* cvars */
static cvar_t	*dedicated;
cvar_t			*sv_cheats;
cvar_t			*sv_hostname;
cvar_t			*sv_maxclients;
cvar_t          *sv_privateClients;
cvar_t          *sv_keywords;
cvar_t          *timelimit ;
cvar_t          *fraglimit ;
cvar_t          *dmflags;
cvar_t          *sv_floodProtect;
cvar_t          *sv_maxRate;

static cvar_t	*sv_padPackets;
static cvar_t	*sv_running;
static cvar_t	*sv_timeout;

extern cvar_t *protocol;
extern cvar_t *name;
extern cvar_t *mapname;

typedef struct cvarTable_s {
	cvar_t	**cvar;
	char	*name;
	char	*resetString;
	int	flags;
} cvarTable_t;

static cvarTable_t cvarTable[] = {
	{ &dedicated, "dedicated", "0", CVAR_ARCHIVE },
	{ &sv_cheats, "sv_cheats", "1", CVAR_ROM },
	{ &sv_hostname, "sv_hostname", "noname", CVAR_ARCHIVE | CVAR_SERVERINFO },
	{ &sv_maxclients, "sv_maxclients", "8", CVAR_ARCHIVE | CVAR_SERVERINFO | CVAR_LATCH },
	{ &sv_privateClients, "sv_privateClients","0", CVAR_SERVERINFO},
	{ &sv_keywords, "sv_keywords","", CVAR_SERVERINFO},
	{ &timelimit, "timelimit","0",CVAR_ARCHIVE | CVAR_SERVERINFO },
	{ &fraglimit, "fraglimit","15",CVAR_ARCHIVE | CVAR_SERVERINFO},
	{ &dmflags,"dmflags","0",CVAR_ARCHIVE | CVAR_SERVERINFO },
	{ &sv_floodProtect,"sv_floodProtect","1", CVAR_ARCHIVE| CVAR_SERVERINFO},
	{ &sv_maxRate,"sv_maxRate","0",CVAR_ARCHIVE | CVAR_SERVERINFO},
	{ &sv_padPackets, "sv_padPackets", "0", 0 },
	{ &sv_running, "sv_running", "0", CVAR_ROM },
	{ &sv_timeout, "sv_timeout", "200", 0 }
};

const static int	cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);





typedef struct client_s {
	net_address_t		address;
	connstate_t     	connstate;
	int					timeout;
	int					ping;
	char				userinfo[MAX_INFO_STRING];
	usercmd_t			cmd;
} client_t;

typedef struct server_static_s {
	aboolean	initialized;
	int			num_clients;
	client_t	clientlist[MAX_CLIENTS];
	gameState_t	gamestate;
} server_static_t;

typedef struct {

	void			*gEnts;
	int				numGEntities;
	int				sizeofGEntity_t;
	playerState_t	*clients;
	int				sizeofGClient;

}server_t ;


server_static_t svs;
server_t sv;



void SV_GetCvars( void )
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}



/* TODO: this could be optimized */
void SV_SetConfigstring( int num, const char *string )
{
	gameState_t		oldstate;
	char			*configstring;
	const char		*value;
	unsigned int	len;
	int				size;
	int				i;

	if( num < 0 || num >= MAX_CONFIGSTRINGS ) {
		Error( "SV_SetConfigstring: bad index %i", num);
	}

	memcpy( &oldstate, &svs.gamestate, sizeof(svs.gamestate) );
	memset( &svs.gamestate,0, sizeof(svs.gamestate) );

	size = sizeof( svs.gamestate.stringData );
	configstring = svs.gamestate.stringData;

	for( i=0; i<MAX_CONFIGSTRINGS; i++ ) {

		svs.gamestate.stringOffsets[i] = svs.gamestate.dataCount;

		if( i == num ) {
			value = string;
		} else {
			value = oldstate.stringData + oldstate.stringOffsets[i];
		}

		len = strlen( value );

		if( svs.gamestate.dataCount + len + 1 > MAX_GAMESTATE_CHARS) {
			Error( "MAX_GAMESTATE_CHARS exceeded" );
		}

		A_strncpyz( configstring, value, size );
		configstring[len] = 0;

		size -= len + 1;
		configstring += len + 1;
		svs.gamestate.dataCount += len + 1;
	}
}

void SV_GetConfigstring( int num, char *buffer, int bufferSize )
{
	if( num < 0 || num >= MAX_CONFIGSTRINGS ) {
		Error( "SV_GetConfigstring: bad index %i", num);
	}

	A_strncpyz(buffer, svs.gamestate.stringData + svs.gamestate.stringOffsets[num], bufferSize);
	//SV_GetConfigstring: bufferSize == %i
}

void SV_GetServerinfo( char *buffer, int bufferSize )
{
	SV_GetConfigstring( CS_SERVERINFO, buffer, bufferSize );
}

void SV_MakeServerinfo( char *serverinfo )
{
	cvar_t *cvar;

	for( cvar=cvarlist; cvar; cvar=cvar->next ) {
		if( cvar->flags & CVAR_SERVERINFO ) {
			Info_SetValueForKey( serverinfo, cvar->name, cvar->string );
		}
	}
}


void SV_Get_GameState ( gameState_t * state )
{
	memcpy (state,&svs.gamestate,sizeof (gameState_t ));

}

static void Cmd_map (void )
{

	char mapname [MAX_APATH];

	if (Cmd_Argc()<1)
	{
		Con_Printf("Invalid call to map ");
		return ;
	}



	Cmd_Argv(1,mapname,MAX_APATH);

//	BSP_Load_Map (mapname);

//	Con_Printf("map : %s \n",mapname);


	SV_Startup(mapname);


	CL_Startup ();


	GAME_main (GAME_RUN_FRAME,1000,0,0,0,0,0,0);
}


void SV_LocateGameData (  void *gEntities, int numEntities, int sizeofGEntity_t,
						 playerState_t *Gclients, int sizeofGClient ) 
{

	sv.gEnts=gEntities;
	sv.numGEntities=numEntities;
	sv.sizeofGEntity_t=sizeofGEntity_t;
	sv.clients=Gclients;
	sv.sizeofGClient=sizeofGClient;

}


int SV_Init (void )
{



	SV_GetCvars();

	Cmd_AddCommand("map",Cmd_map);
	
	memset (&svs,0,sizeof (svs ));

	svs.initialized=1;

	return 1;

}



int SV_Shutdown (void )
{





	svs.initialized=0;

	return 1;
}


int SV_Startup (char * mapname )
{


	char buf [MAX_OSPATH];

	Con_Printf( "------ Server Initialization ------\n" );

	Cvar_Set2( "sv_running", "1",0 );

	Com_sprintf( buf, sizeof(buf), "map %s", mapname );
	Cvar_Set2( "nextmap", buf,0 );

	BSP_Load_Map (mapname);
//	AAS_Load_Map (mapname );

	Cvar_Set2( "mapname", mapname,0 );


	memset( buf,0, sizeof(buf) );
	SV_MakeServerinfo(buf);


	SV_SetConfigstring( CS_SERVERINFO, buf );

	if (!LoadGame()) 
	{
		Error ("Could not load Game ! ");
		return 0;
	}
	GAME_main(0, 0, 0,0,0,0,0,0);

	Con_Printf( "-----------------------------------\n" );

	return 1;
}


void SV_SetBrushModel( sharedEntity_t *ent, const char *name )
{
	int			model_idx;
	cmodel_t	*model;

	if( !ent || !name ) {
		Error( "SV_SetBrushModel: NULL" );
	}

	ent->r.contents = -1;
	VectorClear( ent->r.mins );
	VectorClear( ent->r.maxs );

	if( !name || !name[0] || name[0] != '*' || !name[1] )
		return;

	model_idx = atoi( name+1 );

	if( model_idx < 0 || model_idx > map.num_models ) {
		Con_Printf( "SV_SetBrushModel: %s isn't a brush model\n", name );
		return;
	}

	model = &map.models[model_idx];

	VectorCopy( model->mins, ent->r.mins );
	VectorCopy( model->maxs, ent->r.maxs );



}

