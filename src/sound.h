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


#ifndef SOUND_H__
#define SOUND_H__

#include "c_var.h"

extern cvar_t *s_volume;
extern cvar_t *s_musicvolume;
extern cvar_t *s_separation;
extern cvar_t *s_khz;
extern cvar_t *s_loadas8bit;
extern cvar_t *s_mixahead;
extern cvar_t *s_mixPrestep;
extern cvar_t *s_rolloff;
extern cvar_t *s_doppler;
extern cvar_t *s_distance;
extern cvar_t *s_mindistance;
extern cvar_t *s_maxdistance;
extern cvar_t *s_leafnum;
extern cvar_t *s_refgain;
extern cvar_t *s_refdelay;
extern cvar_t *s_polykeep;
extern cvar_t *s_polysize;
extern cvar_t *s_polyreflectsize;
extern cvar_t *s_numpolys;
extern cvar_t *s_bloat;
extern cvar_t *s_occfactor;
extern cvar_t *s_occ_eq;


aboolean    S_Init (void);
void        S_Shutdown (void); 

void		S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx );

void		S_StartLocalSound( sfxHandle_t sfx, int channelNum );
void		S_ClearLoopingSounds( void );
void		S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx );
void		S_UpdateEntityPosition( int entityNum, const vec3_t origin );

void		S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater );
sfxHandle_t	S_RegisterSound( const char *sample );		
void		S_StartBackgroundTrack( const char *intro, const char *loop );	

#endif 