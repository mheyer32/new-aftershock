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

#ifndef SERVER_H__
#define SERVER_H__

#include "a_shared.h"
#include "network.h"

extern cvar_t			*sv_hostname;
extern cvar_t			*sv_maxclients;

typedef enum connstate_e {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key 
	CA_CONNECTING,		// sending request packets to the server
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
} connstate_t;

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
} server_t;

extern server_static_t svs;
extern server_t sv;

void SV_LocateGameData (void *gEntities, int numEntities, int sizeofGEntity_t,
						 playerState_t *Gclients, int sizeofGClient);
void SV_Shutdown (void);
aboolean SV_Init (char *name);
void SV_Frame (int millisec);
void SV_SetBrushModel(sharedEntity_t *ent, const char *name);
void SV_SetConfigstring(int num, const char *string);
void SV_Get_GameState (gameState_t *state);
void SV_GetConfigstring(int num, char *buffer, int bufferSize);
int SV_Get_Entity_Token (char *buffer, int bufferSize);

#endif