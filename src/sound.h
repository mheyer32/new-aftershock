
#ifndef SOUND_H__
#define SOUND_H__

int         S_Init ( void );
void        S_Shutdown (void ); 

void		S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx );

void		S_StartLocalSound( sfxHandle_t sfx, int channelNum );
void		S_ClearLoopingSounds( void );
void		S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx );
void		S_UpdateEntityPosition( int entityNum, const vec3_t origin );

void		S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater );
sfxHandle_t	S_RegisterSound( const char *sample );		
void		S_StartBackgroundTrack( const char *intro, const char *loop );	





#endif 