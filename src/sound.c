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
#include "c_var.h"
#include "command.h"
#include "io.h"
#include "console.h"
#include "fmod/fmod.h"
#include "sound.h"

#define MAX_SAMPLES 32786
#define NUM_CHANNELS 32 

typedef struct 
{
	char name[MAX_OSPATH];
	FSOUND_SAMPLE *handle;
	int channel; // the actual channel the sound is played on 
	int mode;
	vec3_t origin; 
} sample_t; 

static sample_t samples[MAX_SAMPLES];
static int s_num_samples = 0;
static aboolean s_intialized = afalse;

typedef struct cvarTable_s {
	cvar_t	**cvar;
	char	*name;
	char	*resetString;
	int		flags;
} cvarTable_t;

cvar_t *s_volume;
cvar_t *s_musicvolume;
cvar_t *s_separation;
cvar_t *s_khz;
cvar_t *s_initsound;
cvar_t *s_loadas8bit;
cvar_t *s_mixahead;
cvar_t *s_mixPrestep;
cvar_t *s_rolloff;
cvar_t *s_doppler;
cvar_t *s_distance;
cvar_t *s_mindistance;
cvar_t *s_maxdistance;
cvar_t *s_leafnum;
cvar_t *s_refgain;
cvar_t *s_refdelay;
cvar_t *s_polykeep;
cvar_t *s_polysize;
cvar_t *s_polyreflectsize;
cvar_t *s_numpolys;
cvar_t *s_bloat;
cvar_t *s_occfactor;
cvar_t *s_occ_eq;

static cvarTable_t cvarTable[] = {
	{&s_volume, "s_volume", "1", CVAR_ARCHIVE},
	{&s_musicvolume, "s_musicvolume", "0", CVAR_ARCHIVE},
	{&s_separation, "s_separation", "0.5", CVAR_ARCHIVE},
	{&s_khz, "s_khz", "22", CVAR_ARCHIVE},
	{&s_initsound, "s_initsound", "1", 0},
	{&s_loadas8bit, "s_loadas8bit", "0", CVAR_ARCHIVE},
	{&s_mixahead, "s_mixahead", "0.2", CVAR_ARCHIVE},
	{&s_mixPrestep, "s_mixPrestep", "0.05", CVAR_ARCHIVE},
	{&s_rolloff, "s_rolloff", "1.0", CVAR_ARCHIVE},
	{&s_doppler, "s_doppler", "1.0", CVAR_ARCHIVE},
	{&s_distance, "s_distance", "100.0", CVAR_ARCHIVE},
	{&s_mindistance, "s_mindistance", "100.0", CVAR_ARCHIVE},
	{&s_maxdistance, "s_maxdistance", "1000.0", CVAR_ARCHIVE},
	{&s_leafnum, "s_leafnum", "0", CVAR_ARCHIVE},
	{&s_refgain, "s_refgain", "0.45", CVAR_ARCHIVE},
	{&s_refdelay, "s_refdelay", "2.0", CVAR_ARCHIVE},
	{&s_polykeep, "s_polykeep", "10000000000", CVAR_ARCHIVE},
	{&s_polysize, "s_polysize", "100000000", CVAR_ARCHIVE},
	{&s_polyreflectsize, "s_polyreflectsize", "100000000", CVAR_ARCHIVE},
	{&s_numpolys, "s_numpolys", "400", CVAR_ARCHIVE},
	{&s_bloat, "s_bloat", "2.0", CVAR_ARCHIVE},
	{&s_occfactor, "s_occfactor", "0.5", CVAR_ARCHIVE},
	{&s_occ_eq, "s_occ_eq", "0.75", CVAR_ARCHIVE}
};

const static int	cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);

void S_GetCvars (void)
{
	int		i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		*cv->cvar = Cvar_Get( cv->name, cv->resetString, cv->flags );
	}
}

void Cmd_Soundinfo (void)
{
	Con_Printf ("..... Sound Info .....\n");
	Con_Printf ("sound system is muted\n");
	Con_Printf ("    1 stereo\n");
	Con_Printf ("%i samples\n", FSOUND_GetMaxSamples());
	Con_Printf ("   16 samplebits\n");
	Con_Printf ("    1 submission_chunk\n");
	Con_Printf ("%i speed\n", FSOUND_GetOutputRate());
	Con_Printf (" dma buffer\n");
	Con_Printf ("No background file.\n");

	Con_Printf ("......................\n");
}

aboolean S_Init (void)
{
	int khz;

	Con_Printf ("\n....... sound initialization .......\n");

	S_GetCvars();

	if( !s_initsound->integer ) {
		Com_Printf("not initializing.\n"); 
		Com_Printf("------------------------------------\n");
		return afalse;
	}

	Cmd_AddCommand("soundinfo", Cmd_Soundinfo);

	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		Con_Printf(S_COLOR_YELLOW "WARNING: You are using the wrong FMOD DLL version! You should be using FMOD %.02f\n", FMOD_VERSION);
		Con_Printf (".... sound initialization failed ....\n");
		s_intialized = afalse;
		return afalse;
	}

	memset (samples, 0, MAX_SAMPLES * sizeof(sample_t));
	s_num_samples = 0;
	
	// FSOUND_INIT !!!
	khz = s_khz->integer * 1024;

	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);

	Con_Printf ("Initializing DirectSound\n");

	if (!FSOUND_Init(khz, NUM_CHANNELS, 0))
	{
		Con_Printf (S_COLOR_YELLOW "WARNING: Could not initialize Sound!\n");
		Con_Printf (".... sound initialization failed ....\n");
		s_intialized = afalse;
		return afalse;
	}

	FSOUND_3D_Listener_SetDopplerFactor(s_doppler->value);

	Con_Printf ("....................................\n");

	Cmd_Soundinfo();

	s_intialized = atrue;

	return atrue;
}

void S_Shutdown (void)
{
	int i; 

	if (!s_intialized)
		return;

	for (i = 0; i < s_num_samples; i++ )
	{
		FSOUND_Sample_Free(samples[i].handle);
	}

	FSOUND_Close ();

	s_num_samples = 0;
	s_intialized = afalse;
}

// normal sounds will have their volume dynamically changed as their entity
// moves and the listener moves
void S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx )
{
	sample_t *s = NULL;
	vec3_t vel = {0.0f, 0.0f, 0.0f};
	int act_channel = -1;

	if (sfx < 0 || sfx > MAX_SAMPLES) 
		return;
	if (!s_intialized)
		return;

	s = &samples[sfx];

	act_channel = FSOUND_PlaySound3DAttrib(entchannel, s->handle, -1, s_volume->value * 255, FSOUND_STEREOPAN, origin, NULL);
	FSOUND_SetSFXMasterVolume (s_volume->value * 255);

	if (act_channel == -1) // problem
		return;

	VectorCopy (origin, s->origin);
	s->channel = act_channel;
}

// a local sound is always played full volume
void S_StartLocalSound( sfxHandle_t sfx, int channelNum )
{
	sample_t *s; 
	int act_channel = -1;

	if (sfx < 0 || sfx >= s_num_samples || !s_intialized)
		return;
	if (!s_intialized)
		return;

	s = &samples[sfx];
	
	act_channel = FSOUND_PlaySound(channelNum, s->handle);
	FSOUND_SetSFXMasterVolume (s_volume->value * 255);

	if (act_channel == -1) // failed ! 
		return;

	s->channel = act_channel;
}

void S_ClearLoopingSounds( void )
{
	if (!s_intialized)
		return;

}

void S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx )
{
	if (!s_intialized)
		return;

}

void S_UpdateEntityPosition( int entityNum, const vec3_t origin )
{
	if (!s_intialized)
		return;

}

// repatialize recalculates the volumes of sound as they should be heard by the
// given entityNum and position
void S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater )
{
	if (!s_intialized)
		return;

}

sfxHandle_t	S_RegisterSound ( const char *sample )		// returns buzz if not found
{
	int file, i;
	void *f_data;
	unsigned int f_len;
	FSOUND_SAMPLE *handle;
	int sound_mode = 0;

	if (!s_intialized)
		return -1;

	// check if already loaded
	for (i = 0; i < s_num_samples; i++)
	{
		if (!strcmp (samples[i].name, sample))
			return i;
	}

	// check overflow
	if (s_num_samples >= MAX_SAMPLES)
	{
		Con_Printf (S_COLOR_YELLOW "WARNING: Out of sample-space! Could not register %s!\n", sample);
		return -1;
	}

	f_len = FS_OpenFile (sample, &file, FS_READ);

	if (!file || !f_len)
	{
		Con_Printf (S_COLOR_YELLOW "WARNING: Could not register sound %s\n",sample);
		return 0;
	}

	f_data = malloc (f_len);

	FS_Read(f_data, f_len, file);

	FS_FCloseFile (file);

	// set the standart mode
	sound_mode = FSOUND_STEREO | FSOUND_LOOP_OFF | FSOUND_HW3D | FSOUND_LOADMEMORY;

	if (s_loadas8bit->integer)
		sound_mode |= FSOUND_8BITS;
	else
		sound_mode |= FSOUND_16BITS;

	handle = FSOUND_Sample_Load (FSOUND_UNMANAGED, f_data, sound_mode, f_len);

	free (f_data);

	if (!handle)
		return -1;

	// Set some attributes
	FSOUND_Sample_SetMinMaxDistance (samples[s_num_samples].handle, s_mindistance->value,s_maxdistance->value);

	A_strncpyz (samples[s_num_samples].name, sample, MAX_OSPATH);
	samples[s_num_samples].handle = handle;
	samples[s_num_samples].mode = sound_mode;
	VectorClear (samples[s_num_samples].origin);
	
	return s_num_samples++;
}

void S_StartBackgroundTrack( const char *intro, const char *loop )	// empty name stops music
{
	if (!s_intialized)
		return;

}

