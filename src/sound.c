
#include "a_shared.h"
#include "sound.h"


// A Big TODO !!!!!!!!!!1



int S_Init ( void )
{


	return 1;
}

void S_Shutdown (void )
{




}





// normal sounds will have their volume dynamically changed as their entity
// moves and the listener moves
void		S_StartSound( vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx )
{







}

// a local sound is always played full volume
void		S_StartLocalSound( sfxHandle_t sfx, int channelNum )
{




}
void		S_ClearLoopingSounds( void )
{





}
void		S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx )
{






}
void		S_UpdateEntityPosition( int entityNum, const vec3_t origin )
{



}

// repatialize recalculates the volumes of sound as they should be heard by the
// given entityNum and position
void		S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater )
{




}
sfxHandle_t	S_RegisterSound( const char *sample )		// returns buzz if not found
{


	return 0;

}
void		S_StartBackgroundTrack( const char *intro, const char *loop )	// empty name stops music
{






}

