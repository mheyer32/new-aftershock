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

extern cvar_t			*sv_hostname;
extern cvar_t			*sv_maxclients;




int SV_Init (void );
void SV_LocateGameData (  void *gEntities, int numEntities, int sizeofGEntity_t,
						 playerState_t *Gclients, int sizeofGClient );
int SV_Shutdown (void );
int SV_Startup (char * mapname );
void SV_SetBrushModel( sharedEntity_t *ent, const char *name );
void SV_SetConfigstring( int num, const char *string );
void SV_Get_GameState ( gameState_t * state );
void SV_GetConfigstring( int num, char *buffer, int bufferSize );

























#endif