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
#include "io.h."
#include "network.h"
#include "game.h"
#include "c_var.h"
#include "command.h"
#include "console.h"
#include "cmap.h"
#include "server.h"
#include "client.h"

/* cvars */
static cvar_t	*dedicated;
cvar_t			*sv_cheats;
cvar_t			*sv_hostname;
cvar_t			*sv_maxclients;
cvar_t          *sv_privateClients;
cvar_t          *sv_keywords;
cvar_t			*sv_fps;
cvar_t          *timelimit;
cvar_t          *fraglimit;
cvar_t          *dmflags;
cvar_t          *sv_floodProtect;
cvar_t          *sv_maxRate;
cvar_t			*mapname;

static cvar_t	*sv_padPackets;
static cvar_t	*sv_running;
static cvar_t	*sv_timeout;

extern cvar_t *protocol;
extern cvar_t *name;

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
	{ &sv_privateClients, "sv_privateClients", "0", CVAR_SERVERINFO},
	{ &sv_keywords, "sv_keywords", "", CVAR_SERVERINFO},
	{ &sv_fps, "sv_fps", "20", CVAR_TEMP},
	{ &timelimit, "timelimit", "0", CVAR_ARCHIVE | CVAR_SERVERINFO },
	{ &fraglimit, "fraglimit", "15", CVAR_ARCHIVE | CVAR_SERVERINFO},
	{ &dmflags, "dmflags","0", CVAR_ARCHIVE | CVAR_SERVERINFO },
	{ &sv_floodProtect, "sv_floodProtect", "1", CVAR_ARCHIVE| CVAR_SERVERINFO},
	{ &sv_maxRate, "sv_maxRate", "0", CVAR_ARCHIVE | CVAR_SERVERINFO},
	{ &sv_padPackets, "sv_padPackets", "0", 0 },
	{ &sv_running, "sv_running", "0", CVAR_ROM },
	{ &sv_timeout, "sv_timeout", "200", 0 },
	{ &mapname, "mapname", "Q3DM1", CVAR_SERVERINFO }
};

server_static_t svs;
server_t sv;

const static int cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

char *entitypos = NULL;

void SV_GetCvars (void)
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

/* TODO: this could be optimized */
void SV_SetConfigstring(int num, const char *string)
{
	gameState_t		oldstate;
	char			*configstring;
	const char		*value;
	unsigned int	len;
	int				size;
	int				i;

	if ( num < 0 || num >= MAX_CONFIGSTRINGS ) {
		Error( "SV_SetConfigstring: bad index %i", num);
		return;
	}

	memcpy( &oldstate, &svs.gamestate, sizeof(svs.gamestate) );
	memset( &svs.gamestate, 0, sizeof(svs.gamestate) );

	size = sizeof( svs.gamestate.stringData );
	configstring = svs.gamestate.stringData;

	for( i=0; i<MAX_CONFIGSTRINGS; i++ ) {
		svs.gamestate.stringOffsets[i] = svs.gamestate.dataCount;

		if ( i == num ) {
			value = string;
		} else {
			value = oldstate.stringData + oldstate.stringOffsets[i];
		}

		len = strlen( value );

		if ( svs.gamestate.dataCount + len + 1 > MAX_GAMESTATE_CHARS) {
			Error( "MAX_GAMESTATE_CHARS exceeded" );
		}

		A_strncpyz( configstring, value, size );
		configstring[len] = 0;

		size -= len + 1;
		configstring += len + 1;
		svs.gamestate.dataCount += len + 1;
	}
}

void SV_GetConfigstring (int num, char *buffer, int bufferSize)
{
	if (num < 0 || num >= MAX_CONFIGSTRINGS) {
		Error( "SV_GetConfigstring: bad index %i", num);
		return;
	}

	A_strncpyz(buffer, svs.gamestate.stringData + svs.gamestate.stringOffsets[num], bufferSize);
}

void SV_GetServerinfo (char *buffer, int bufferSize)
{
	SV_GetConfigstring (CS_SERVERINFO, buffer, bufferSize);
}

void SV_MakeServerinfo(char *serverinfo)
{
	cvar_t *cvar;

	for (cvar = cvarlist; cvar; cvar = cvar->next) {
		if (cvar->flags & CVAR_SERVERINFO) {
			if (cvar->name && cvar->string)
				Info_SetValueForKey(serverinfo, cvar->name, cvar->string);
		}
	}
}


void SV_Get_GameState (gameState_t *state)
{
	memcpy (state, &svs.gamestate, sizeof(gameState_t));
}

static void SV_Startup( void )
{
	if( svs.initialized ) {
		Error("SV_Startup: svs.initialized");
		return;
	}

	sv_maxclients->modified = afalse;

	if( sv_maxclients->integer < 1 ) {
		Cvar_Set( "sv_maxclients", "1" );
	} else if( sv_maxclients->integer > MAX_CLIENTS ) {
		Cvar_Set( "sv_maxclients", va( "%i", MAX_CLIENTS ) );
	}

	Cvar_Set( "sv_running", "1" );
	svs.initialized = atrue;
}

static void SV_Map( const char *mapname )
{
	char buf[MAX_INFO_STRING];

	if (GAME_main) {
		GAME_main (GAME_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
		UnLoadGame();
	}

	Con_Printf("------ Server Initialization ------\n");

	Con_Printf("Server: %s\n", mapname);

	if ( Cvar_VariableValue("sv_running") ) {
		SV_Startup();
	}

	if( sv_maxclients->modified ) {
		// TODO
	}

	FS_Restart();

	Cvar_Set( "nextmap", va( "map %s", mapname ) );

	if (!CM_LoadMap (va( "maps/%s.bsp", mapname ), atrue ))
	{
		Con_Printf ("Could not load map %s\n", mapname);
		SV_Shutdown();
		return;
	}

	Cvar_Set( "mapname", mapname );

	entitypos = cm.entityspawn;

	memset(buf, 0, sizeof(buf));

	SV_MakeServerinfo (buf);
	SV_SetConfigstring (CS_SERVERINFO, buf);

	if (!LoadGame()) 
	{
		Error ("Could not load Game!");
		return;
	}

//	GAME_main(GAME_INIT, 0, 0, 0, 0, 0, 0, 0);

	CL_Startup ();

	Com_Printf ("-----------------------------------\n");
}

static void Cmd_map (void)
{
	char name[MAX_APATH];
	char mapfile[MAX_APATH];
	int file, filelen;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("Usage: map [mapname]\n");
		return;
	}

	Cmd_Argv(1, mapfile, MAX_APATH);
	COM_StripExtension(mapfile, name);
	A_strncpyz (mapfile, va("maps/%s.bsp", name), MAX_APATH);

	// read the file
	filelen = FS_OpenFile (mapfile, &file, FS_READ);

	if (!file || !filelen) {
		Con_Printf("Can't find map %s\n", mapfile);
		return;
	}

	FS_FCloseFile (file);

	SV_Map(name);
}

static void Cmd_spmap (void )
{
	char name[MAX_APATH];
	char mapfile[MAX_APATH];
	int file, filelen;

	if (Cmd_Argc() < 2)
	{
		Con_Printf("Usage: spmap [mapname]\n");
		return;
	}

	Cmd_Argv(1, mapfile, MAX_APATH);
	COM_StripExtension(mapfile, name);
	A_strncpyz (mapfile, va("maps/%s.bsp", name), MAX_APATH);

	// read the file
	filelen = FS_OpenFile (mapfile, &file, FS_READ);

	if (!file || !filelen ) {
		Con_Printf("Can't find map %s\n", mapfile);
		return;
	}

	FS_FCloseFile (file);

	SV_Map(name);
}

void SV_LocateGameData (void *gEntities, int numEntities, int sizeofGEntity_t,
						 playerState_t *Gclients, int sizeofGClient) 
{
	sv.gEnts = gEntities;
	sv.numGEntities = numEntities;
	sv.sizeofGEntity_t = sizeofGEntity_t;
	sv.clients = Gclients;
	sv.sizeofGClient = sizeofGClient;
}

aboolean SV_Init (char *map)
{
	memset (&svs, 0, sizeof (svs));

	SV_GetCvars();

	Cmd_AddCommand("map", Cmd_map);
	Cmd_AddCommand("spmap", Cmd_spmap);

	return atrue;
}

void SV_Frame (int millisec)
{
	if( sv_fps->integer < 1 ) {
		Cvar_Set( "sv_fps", "10" );
	}

}

void SV_Shutdown (void)
{
	if ( !sv_running || !sv_running->integer ) 
		return;

	Con_Printf( "----- Server Shutdown -----\n" );

	if (GAME_main) {
		GAME_main (GAME_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
		UnLoadGame();
	}

	memset (&svs, 0, sizeof (svs));

	Cvar_Set( "sv_running", "0" );

	Com_Printf( "---------------------------\n" );
}

void SV_SetBrushModel(sharedEntity_t *ent, const char *name)
{
	int			model_idx;
	cmodel_t	*model;

	if( !ent || !name ) {
		Error( "SV_SetBrushModel: NULL" );
		return;
	}

	ent->r.contents = -1;
	VectorClear(ent->r.mins);
	VectorClear(ent->r.maxs);

	if (!name || !name[0] || (name[0] != '*') || !name[1])
		return;

	model_idx = atoi( name + 1 );

	if (model_idx < 0 || model_idx > cm.num_models) {
		Con_Printf( "SV_SetBrushModel: %s isn't a brush model\n", name );
		return;
	}

	model = &cm.models[model_idx];

	VectorCopy(model->mins, ent->r.mins);
	VectorCopy(model->maxs, ent->r.maxs);
}

int SV_Get_Entity_Token (char *buffer, int bufferSize)
{
	char *token;
	
	COM_BeginParseSession ();

	while (entitypos)
	{
		token = COM_ParseExt (&entitypos, 1);
		
		if (!token[0]) // new Line 
			continue;
				
		A_strncpyz (buffer, token, bufferSize); // success
		
		return 1;
	}

	return 0;
}

