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
#include "sys_main.h"
#include "util.h"
#include "vec.h"
#include "cmap.h"
#include "c_var.h"
#include "command.h"
#include "render.h"
#include "game.h"
#include "ui.h"
#include "cgame.h"
#include "c_var.h"
#include "render.h"
#include "renderback.h"
#include "io.h"
#include <time.h>
#include "keys.h"
#include "console.h"
#include "shader.h"
#include "opengl.h"
#include "entity.h"
#include "client.h"
#include "server.h"
#include "brushtest.h"
#include "sound.h"

#include "net_lan.h"

typedef    int  SYSCALL (int arg, ...) ;




static float GETFLOAT( int x )
{
	int intTemp;
	intTemp = x;
	return *(float *)&intTemp;
}

static int PASSFLOAT( float x ) {
	float	Temp;
	Temp = x;
	return *(int *)&Temp;
}




static void Call_R_AddPolyToScene(int Shader ,int numVerts,const polyVert_t *verts )
{
	R_AddPolyToScene(verts,numVerts,Shader);


}


static void Call_Cvar_Register (unsigned int flags,const char *resetString,const char *var_name,vmCvar_t *cvar )
{
	Cvar_Register(cvar,var_name,resetString,flags );
}

static void Call_Cvar_Set(const char *value,const char *var_name )
{
	Cvar_Set(var_name,value );
}

static void Call_Cvar_SetValue(int  value, const char *var_name )
{
	
	Cvar_SetValue(var_name,GETFLOAT (value));


}

static void Call_Cvar_Create(unsigned int flags,const char *resetString, const char *var_name )
{
	Cvar_Get( var_name, resetString, flags );
}

static void Call_Cvar_VariableStringBuffer( int bufsize,char *buffer, const char *var_name )
{
	Cvar_VariableStringBuffer(var_name,buffer,bufsize);

}


static int Call_FOpenFile ( int mode ,int  *handle,const char *path) 
{
	return FS_OpenFile(path,handle, mode ); 
}

static void Call_FS_Read(int  handle,int len,void *buffer ) 
{
	FS_Read(buffer,len ,handle); 
}
static int Call_FS_Write( int  handle ,int len,const void *buffer) 
{
	return FS_Write( buffer, len, handle ); 
}

static int  Call_FS_GetFileList (int bufsize,char *listbuf, const char *extension,const char *path)
{
	return FS_GetFileList (path,extension,listbuf,bufsize);

}

static void Call_Cmd_Argv(int bufferLength,char *buffer, int n )
{
	Cmd_Argv(n,buffer,bufferLength);
}

static Call_Cbuf_ExecuteText(const char *text,cbufExec_t exec_when )
{

	Cbuf_ExecuteText(exec_when,text);
}

static void Call_Key_SetBinding( const char *binding,int keynum ) 
{
	Key_SetBinding( keynum,binding ) ;
}

static void Call_Key_GetBindingBuf(int buflen,char *buf, int keynum ) 
{
	Key_GetBindingBuf( keynum, buf, buflen ) ;

}

static void Call_Key_KeynumToStringBuf(int buflen,char *buf, int keynum )
{
	 Key_KeynumToStringBuf( keynum, buf, buflen );
}

static void Call_R_DrawStretchPic(int hShader,int t2,int s2,int t1,int s1,int h,int w,int y,int x)
{
	R_DrawStretchPic (GETFLOAT(x),GETFLOAT(y),GETFLOAT(w),GETFLOAT(h),GETFLOAT(s1),GETFLOAT(t1),
		GETFLOAT(s2),GETFLOAT(t2),hShader);


}

static void Call_R_AddLightToScene(int b ,int g,int r,int intensity, const vec3_t org)
{
	R_AddLightToScene( org, GETFLOAT(intensity),GETFLOAT( r),GETFLOAT( g),GETFLOAT(b) );
}
static void Call_LAN_GetLocalServerAddressString(int buflen, char *buf, int n )
{
	LAN_GetLocalServerAddressString( n, buf, buflen );
}

static void Call_LAN_GetGlobalServerAddressString(int buflen, char *buf, int n )
{
	LAN_GetGlobalServerAddressString( n, buf, buflen );
}

static void Call_LAN_GetPing( int *pingtime,int buflen,char *buf , int n)
{
	LAN_GetPing( n, buf, buflen, pingtime );
}

static void Call_LAN_GetPingInfo( int buflen ,char *buf,int n)
{
	LAN_GetPingInfo( n, buf,buflen );
}

static void Call_Cmd_Args( int bufferLength ,char *buffer)
{
	Cmd_Args( buffer, bufferLength );

}

static void Call_R_LerpTag( const char *tagName, int  frac ,int endFrame, int startFrame,int model,orientation_t *tag) 
{
	R_LerpTag( tag, model, startFrame, endFrame,GETFLOAT(frac), tagName ); 

}


static void	Call_S_StartLocalSound(int channelNum, sfxHandle_t sfx)
{
	S_StartLocalSound( sfx, channelNum );

}


 int UI_Call (int arg, ...)
{

	 va_list mark;
	 va_start (mark,arg);
	 
	switch (arg)
	{

	case UI_ERROR:
		Error (va_arg(mark,const char *));
		break;   
	case UI_PRINT:
		Con_Printf(va_arg(mark,char*));
		break;
	case UI_MILLISECONDS:
		return Sys_Get_Time();
		break;
	case UI_CVAR_SET:
		Call_Cvar_Set(va_arg(mark,char*),va_arg(mark,char*));
		break;
	case UI_CVAR_VARIABLEVALUE:
		return  PASSFLOAT(  Cvar_VariableValue( va_arg(mark,const char* )));
		break;
	case UI_CVAR_VARIABLESTRINGBUFFER:
		Call_Cvar_VariableStringBuffer(va_arg(mark,int) ,va_arg(mark,char *), va_arg(mark,const char *)) ;
		break;
	case UI_CVAR_SETVALUE:
		Call_Cvar_SetValue(va_arg(mark,int),va_arg(mark,char*));
		break;
	case UI_CVAR_RESET:
		Cvar_Reset(va_arg(mark,const char *));
		break;
	case UI_CVAR_CREATE:
		Call_Cvar_Create( va_arg(mark,int),va_arg(mark,char*) ,va_arg(mark,char*)); 
		break;
	case UI_CVAR_INFOSTRINGBUFFER:
		break;  // TODO !
	case UI_ARGC:
		return Cmd_Argc ();
		break;
	case UI_ARGV:
		Call_Cmd_Argv(va_arg(mark,int),va_arg(mark,char *),va_arg(mark,int));
		break;
	case UI_CMD_EXECUTETEXT:
		Call_Cbuf_ExecuteText(va_arg(mark,char *),va_arg(mark,int));
		break;
	case UI_FS_FOPENFILE:
		return Call_FOpenFile(va_arg(mark,int), va_arg(mark,int*),va_arg(mark, const char*) ) ;
		break;
	case UI_FS_READ:
		Call_FS_Read(va_arg(mark,int),va_arg(mark,int), va_arg(mark,void*));
		break;
	case UI_FS_WRITE:
		Call_FS_Write( va_arg(mark,int)   ,va_arg(mark,int ),va_arg(mark,const void *)); 
		break;  
	case UI_FS_FCLOSEFILE:
		FS_FCloseFile( va_arg(mark,int) ); 
		break;
	case UI_FS_GETFILELIST:
		return	Call_FS_GetFileList (va_arg(mark,int),va_arg(mark,char*), va_arg(mark,const char *),va_arg(mark,const char*));
		break;
	case UI_R_REGISTERMODEL:
		return R_RegisterModel( va_arg(mark,const char *)) ;
		break;
	case UI_R_REGISTERSKIN:
		return R_RegisterSkin( va_arg(mark,const char *));
		break;   
	case UI_R_REGISTERSHADERNOMIP:
		return R_RegisterShaderNoMip( va_arg(mark,const char *)) ;
		break;
	case UI_R_CLEARSCENE:
		R_ClearScene ();
		break;
	case UI_R_ADDREFENTITYTOSCENE:
		R_AddRefEntityToScene( va_arg(mark,const refEntity_t *) );
		break;
	case UI_R_ADDPOLYTOSCENE:
		Call_R_AddPolyToScene(  va_arg(mark,int)  , va_arg(mark,int),va_arg(mark, const polyVert_t *) ); 
		break;
	case UI_R_ADDLIGHTTOSCENE:
		Call_R_AddLightToScene(va_arg(mark,int)  ,va_arg (mark,int),va_arg(mark,int),
			va_arg(mark,int),va_arg(mark, const vec3_t));
		break; 
	case UI_R_RENDERSCENE:
		R_RenderScene( va_arg(mark,const refdef_t *) ) ;
		break;
	case UI_R_SETCOLOR:
		R_SetColor( va_arg(mark,const float * )) ;
		break;
	case UI_R_DRAWSTRETCHPIC:
		Call_R_DrawStretchPic(va_arg(mark,int),va_arg(mark, int),va_arg(mark, int),va_arg(mark, int)
						,va_arg(mark, int), va_arg(mark,int), va_arg(mark,int),va_arg(mark, int),va_arg(mark, int));
		break;
	case UI_UPDATESCREEN:
		R_Update_Screen ();
		break;
	case UI_CM_LERPTAG:
		Call_R_LerpTag( va_arg (mark,const char *),va_arg (mark, int ) ,va_arg (mark,int ),
			va_arg (mark,int ),va_arg (mark,int ),va_arg(mark,orientation_t *)); 
		break; 
	case UI_CM_LOADMODEL:
		break;  // TODO !
	case UI_S_REGISTERSOUND:
		return S_RegisterSound( va_arg (mark,const char *) );
		break ; // TODO !
	case UI_S_STARTLOCALSOUND:
		Call_S_StartLocalSound (va_arg (mark,int ), va_arg (mark,sfxHandle_t ));
		break;  // TODO !
	case UI_KEY_KEYNUMTOSTRINGBUF:
		Call_Key_KeynumToStringBuf(va_arg(mark,int),va_arg(mark,char*),va_arg(mark,int));
		break;
	case UI_KEY_GETBINDINGBUF:
		Call_Key_GetBindingBuf(va_arg(mark,int),va_arg(mark,char*),va_arg(mark,int));
		break;
	case UI_KEY_SETBINDING:
		Call_Key_SetBinding(va_arg(mark,const char *),va_arg(mark,int));
		break;
	case UI_KEY_ISDOWN:
		return Key_IsDown( va_arg(mark,int) ); 
		break;
	case UI_KEY_GETOVERSTRIKEMODE:
		return Key_GetOverstrikeMode();
		break;
	case UI_KEY_SETOVERSTRIKEMODE:
		Key_SetOverstrikeMode(va_arg(mark,int));
		break;
	case UI_KEY_CLEARSTATES:
		Key_ClearStates();
		break;
	case UI_KEY_GETCATCHER:
		return Key_GetCatcher();
		break;
	case UI_KEY_SETCATCHER:
		Key_SetCatcher(va_arg(mark,int)); 
		break;
	case UI_GETCLIPBOARDDATA:
		break; // TODO !
	case UI_GETGLCONFIG:
		GetGlConfig(va_arg (mark,glconfig_t*));
		break;
	case UI_GETCLIENTSTATE:
		break;   // TODO !
	case UI_GETCONFIGSTRING:
		break;   // TODO !
	case UI_LAN_GETLOCALSERVERCOUNT:
		return LAN_GetLocalServerCount( );
		break;
	case UI_LAN_GETLOCALSERVERADDRESSSTRING:
		Call_LAN_GetLocalServerAddressString(va_arg (mark,int) , va_arg(mark,char *), va_arg(mark,int)  );
		break;
	case UI_LAN_GETGLOBALSERVERCOUNT:
		return  LAN_GetGlobalServerCount();
		break;
	case UI_LAN_GETGLOBALSERVERADDRESSSTRING:
		Call_LAN_GetGlobalServerAddressString(va_arg(mark,int ), va_arg(mark,char *), va_arg(mark,int) );
		break;
	case UI_LAN_GETPINGQUEUECOUNT:
		return LAN_GetPingQueueCount( );
		break;
	case UI_LAN_CLEARPING:
		LAN_ClearPing( va_arg(mark,int) );
		break;
	case UI_LAN_GETPING:
		Call_LAN_GetPing( va_arg(mark,int *),va_arg(mark,int) ,va_arg(mark,char *) ,va_arg(mark,int));
		break;
	case UI_LAN_GETPINGINFO:
		Call_LAN_GetPingInfo( va_arg(mark,int)  ,va_arg(mark,char *),va_arg(mark,int));
		break;
	case UI_CVAR_REGISTER:
		Call_Cvar_Register (va_arg (mark,int),va_arg(mark,const char*),va_arg(mark,const char*),va_arg(mark,vmCvar_t*) );
		break; 
	case UI_CVAR_UPDATE:
		Cvar_Update(va_arg(mark, vmCvar_t *));
		break;
	case UI_MEMORY_REMAINING:
		return Sys_GetRemaining_Memory();
		break;
	case UI_GET_CDKEY:
		break;  // TODO !
	case UI_SET_CDKEY:
		break;  // TODO !

	case UI_MEMSET :
	case UI_MEMCPY:
	case UI_STRNCPY:
	case UI_SIN:
	case UI_COS:
	case UI_ATAN2:
	case UI_SQRT:
	case UI_FLOOR:
	case UI_CEIL:
		break;

	default:
		Con_Printf ("UI_Call : unknown trap : %i \n",arg);
		return 0;

		break;
	}


	return 0;



}

// SERVER :


static void Call_SV_LocateGameData (int sizeofGClient ,playerState_t *Gclients,int sizeofGEntity_t,int numEntities, void *gEntities)
{							
	 SV_LocateGameData(gEntities, numEntities, sizeofGEntity_t,
						 Gclients, sizeofGClient ) ;

}
static void Call_SV_SetConfigstring(const char *string, int num )
{
	SV_SetConfigstring( num, string );
}

static void Call_SV_GetConfigstring(int bufferSize ,char *buffer, int num)
{

SV_GetConfigstring( num, buffer, bufferSize );
}

static int Call_SV_Get_Entity_Token ( int bufferSize,char *buffer  )
{

	return  SV_Get_Entity_Token ( buffer, bufferSize );
}


static void Call_SV_SetBrushModel( const char *name ,sharedEntity_t *ent)
{
	SV_SetBrushModel( ent, name );
}

static void Call_Trace( int contentmask,int passEntityNum,const vec3_t end,const vec3_t maxs,const vec3_t mins,const vec3_t start,trace_t *results  )
{

	Trace( results, start, mins,
		   maxs, end, passEntityNum, contentmask );

}

static int  Call_Point_Contents (int passEntityNum,const vec3_t pos )
{
	return  Point_Contents (pos ,passEntityNum);

}

int GAME_Call (int arg,...)
{

	va_list mark;
	va_start (mark,arg);


	switch (arg)
	{
		
		case G_PRINT:		// ( const char *string );
			Con_Printf(va_arg(mark,const char *));
			break;
			


		case G_ERROR:		// ( const char *string );
			Error (va_arg (mark,const char * ));
			break;

		case G_MILLISECONDS:	// ( void );
			return Sys_Get_Time();

			break;

	
		case G_CVAR_REGISTER:	// ( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
			Call_Cvar_Register (va_arg (mark,int),va_arg(mark,const char*),va_arg(mark,const char*),va_arg(mark,vmCvar_t*) );
			break;
		case G_CVAR_UPDATE:	// ( vmCvar_t *vmCvar );
			Cvar_Update(va_arg(mark, vmCvar_t *));
			break;
		case G_CVAR_SET:		// ( const char *var_name, const char *value );
			Call_Cvar_Set(va_arg(mark,char*),va_arg(mark,char*));
			break;
		case G_CVAR_VARIABLE_INTEGER_VALUE:	// ( const char *var_name );
			return Cvar_VariableIntegerValue(va_arg(mark,const char *));
			break;

		case G_CVAR_VARIABLE_STRING_BUFFER:	// ( const char *var_name, char *buffer, int bufsize );
			Call_Cvar_VariableStringBuffer(va_arg(mark,int) ,va_arg(mark,char *), va_arg(mark,const char *)) ;
			break;

		case G_ARGC:			// ( void );
			return Cmd_Argc ();
			break;

		case G_ARGV:			// ( int n, char *buffer, int bufferLength );
			Call_Cmd_Argv(va_arg(mark,int),va_arg(mark,char *),va_arg(mark,int));
			break;

		case G_FS_FOPEN_FILE:	// ( const char *qpath, fileHandle_t *file, fsMode_t mode );
			return Call_FOpenFile(va_arg(mark,int), va_arg(mark,int*),va_arg(mark, const char*) ) ;
			break;
		case G_FS_READ:		// ( void *buffer, int len, fileHandle_t f );
			Call_FS_Read(va_arg(mark,int),va_arg(mark,int), va_arg(mark,void*));
			break;
		case G_FS_WRITE:		// ( const void *buffer, int len, fileHandle_t f );
			Call_FS_Write( va_arg(mark,int)   ,va_arg(mark,int ),va_arg(mark,const void *)); 
			break;
		case G_FS_FCLOSE_FILE:		// ( fileHandle_t f );
			FS_FCloseFile( va_arg(mark,int) ); 

			break;
		case G_SEND_CONSOLE_COMMAND:	// ( const char *text );
			Cbuf_AddText(va_arg(mark,const char *));

			break;
		
			//=========== server specific functionality =============

		case G_LOCATE_GAME_DATA:		// ( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t,
			Call_SV_LocateGameData (va_arg(mark,int),va_arg(mark,playerState_t *),
				va_arg(mark,int) ,va_arg(mark,int) , va_arg(mark,void *));
			break;	
		case G_DROP_CLIENT:		// ( int clientNum, const char *reason );
			break;
			// kick a client off the server with a message

		case G_SEND_SERVER_COMMAND:	// ( int clientNum, const char *fmt, ... );
            break;		
			// reliably sends a command string to be interpreted by the given
	// client.  If clientNum is -1, it will be sent to all clients

		case G_SET_CONFIGSTRING:	// ( int num, const char *string );
			Call_SV_SetConfigstring(va_arg(mark,const char *), va_arg(mark,int) );
			break;
		case G_GET_CONFIGSTRING:	// ( int num, char *buffer, int bufferSize );
			Call_SV_GetConfigstring(va_arg(mark,int)  ,va_arg(mark,char *),va_arg(mark, int));
			break;
		case G_GET_USERINFO:		// ( int num, char *buffer, int bufferSize );
			break;
			// userinfo strings are maintained by the server system, so they
	// are persistant across level loads, while all other game visible
	// data is completely reset

		case G_SET_USERINFO:		// ( int num, const char *buffer );
			break;
		case G_GET_SERVERINFO:	// ( char *buffer, int bufferSize );
			break;
			// the serverinfo info string has all the cvars visible to server browsers

		case G_SET_BRUSH_MODEL:	// ( gentity_t *ent, const char *name );
			Call_SV_SetBrushModel( va_arg (mark,const char *) ,va_arg ( mark ,sharedEntity_t *));
			break;
			// sets mins and maxs based on the brushmodel name

		case G_TRACE:	// ( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
			Call_Trace(va_arg (mark,int ),va_arg (mark,int ),va_arg (mark,const vec3_t ),va_arg (mark,const vec3_t ),
				va_arg(mark,const vec3_t ),va_arg (mark,const vec3_t ),va_arg(mark,trace_t * ));
			break;
			// collision detection against all linked entities

		case G_POINT_CONTENTS:	// ( const vec3_t point, int passEntityNum );
			return Call_Point_Contents (va_arg (mark,int ),va_arg (mark,const vec3_t ));
			break;
			// point contents against all linked entities

		case G_IN_PVS:			// ( const vec3_t p1, const vec3_t p2 );
			return 1;
			break;
		case G_IN_PVS_IGNORE_PORTALS:	// ( const vec3_t p1, const vec3_t p2 );
			// no Call function : order doesn`t matter 
			return R_TestVis ( va_arg (mark, const vec3_t ),va_arg (mark,const vec3_t ));
			break;
		case G_ADJUST_AREA_PORTAL_STATE:	// ( gentity_t *ent, qboolean open );
			break;
		case G_AREAS_CONNECTED:	// ( int area1, int area2 );
			break;
		case G_LINKENTITY:		// ( gentity_t *ent );
			break;
			// an entity will never be sent to a client or used for collision
	// if it is not passed to linkentity.  If the size, position, or
	// solidity changes, it must be relinked.

		case G_UNLINKENTITY:		// ( gentity_t *ent );		
			break;
			// call before removing an interactive entity

		case G_ENTITIES_IN_BOX:	// ( const vec3_t mins, const vec3_t maxs, gentity_t **list, int maxcount );
			break;
			// EntitiesInBox will return brush models based on their bounding box,
	// so exact determination must still be done with EntityContact

		case G_ENTITY_CONTACT:	// ( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
			break;
			// perform an exact check against inline brush models of non-square shape

	// access for bots to get and free a server client (FIXME?)
		case G_BOT_ALLOCATE_CLIENT:	// ( void );
			break;
		case G_BOT_FREE_CLIENT:	// ( int clientNum );
			break;
		case G_GET_USERCMD:	// ( int clientNum, usercmd_t *cmd )
			break;
		case G_GET_ENTITY_TOKEN:  // qboolean ( char *buffer, int bufferSize )
			return Call_SV_Get_Entity_Token ( va_arg (mark ,int ),va_arg (mark,char *)  );
			break;
		case G_FS_GETFILELIST:
			return	Call_FS_GetFileList (va_arg(mark,int),va_arg(mark,char*), va_arg(mark,const char *),va_arg(mark,const char*));
			break;
		case G_DEBUG_POLYGON_CREATE:
			break;
		case G_DEBUG_POLYGON_DELETE:
			break;
		
		case BOTLIB_SETUP :				// ( void );
		case BOTLIB_SHUTDOWN:				// ( void );
		case BOTLIB_LIBVAR_SET:
		case BOTLIB_LIBVAR_GET:
		case BOTLIB_DEFINE:
		case BOTLIB_START_FRAME:
		case BOTLIB_LOAD_MAP:
		case BOTLIB_UPDATENTITY:
		case BOTLIB_TEST:

		case BOTLIB_GET_SNAPSHOT_ENTITY:		// ( int client, int ent );
		case BOTLIB_GET_CONSOLE_MESSAGE:		// ( int client, char *message, int size );
		case BOTLIB_USER_COMMAND:			// ( int client, usercmd_t *ucmd );

		case BOTLIB_AAS_ENTITY_VISIBLE :	//FIXME: remove
		case BOTLIB_AAS_IN_FIELD_OF_VISION:		//FIXME: remove
		case BOTLIB_AAS_VISIBLE_CLIENTS:			//FIXME: remove
		case BOTLIB_AAS_ENTITY_INFO:

		case BOTLIB_AAS_INITIALIZED:
		case BOTLIB_AAS_PRESENCE_TYPE_BOUNDING_BOX:
		case BOTLIB_AAS_TIME:

		case BOTLIB_AAS_POINT_AREA_NUM:
		case BOTLIB_AAS_TRACE_AREAS:

		case BOTLIB_AAS_POINT_CONTENTS:
		case BOTLIB_AAS_NEXT_BSP_ENTITY:
		case BOTLIB_AAS_VALUE_FOR_BSP_EPAIR_KEY:
		case BOTLIB_AAS_VECTOR_FOR_BSP_EPAIR_KEY:
		case BOTLIB_AAS_FLOAT_FOR_BSP_EPAIR_KEY:
		case BOTLIB_AAS_INT_FOR_BSP_EPAIR_KEY:

		case BOTLIB_AAS_AREA_REACHABILITY:

		case BOTLIB_AAS_AREA_TRAVEL_TIME_TO_GOAL_AREA:

		case BOTLIB_AAS_SWIMMING:
		case BOTLIB_AAS_PREDICT_CLIENT_MOVEMENT:



		case BOTLIB_EA_SAY :
		case BOTLIB_EA_SAY_TEAM:
		case BOTLIB_EA_USE_ITEM:
		case BOTLIB_EA_DROP_ITEM:
		case BOTLIB_EA_USE_INV:
		case BOTLIB_EA_DROP_INV:
		case BOTLIB_EA_GESTURE:
		case BOTLIB_EA_COMMAND:

		case BOTLIB_EA_SELECT_WEAPON:
		case BOTLIB_EA_TALK:
		case BOTLIB_EA_ATTACK:
		case BOTLIB_EA_USE:
		case BOTLIB_EA_RESPAWN:
		case BOTLIB_EA_JUMP:
		case BOTLIB_EA_DELAYED_JUMP:
		case BOTLIB_EA_CROUCH:
		case BOTLIB_EA_MOVE_UP:
		case BOTLIB_EA_MOVE_DOWN:
		case BOTLIB_EA_MOVE_FORWARD:
		case BOTLIB_EA_MOVE_BACK:
		case BOTLIB_EA_MOVE_LEFT:
		case BOTLIB_EA_MOVE_RIGHT:
		case BOTLIB_EA_MOVE:
		case BOTLIB_EA_VIEW:

		case BOTLIB_EA_END_REGULAR:
		case BOTLIB_EA_GET_INPUT:
		case BOTLIB_EA_RESET_INPUT:



		case BOTLIB_AI_LOAD_CHARACTER :
		case BOTLIB_AI_FREE_CHARACTER:
		case BOTLIB_AI_CHARACTERISTIC_FLOAT:
		case BOTLIB_AI_CHARACTERISTIC_BFLOAT:
		case BOTLIB_AI_CHARACTERISTIC_INTEGER:
		case BOTLIB_AI_CHARACTERISTIC_BINTEGER:
		case BOTLIB_AI_CHARACTERISTIC_STRING:

		case BOTLIB_AI_ALLOC_CHAT_STATE:
		case BOTLIB_AI_FREE_CHAT_STATE:
		case BOTLIB_AI_QUEUE_CONSOLE_MESSAGE:
		case BOTLIB_AI_REMOVE_CONSOLE_MESSAGE:
		case BOTLIB_AI_NEXT_CONSOLE_MESSAGE:
		case BOTLIB_AI_NUM_CONSOLE_MESSAGE:
		case BOTLIB_AI_INITIAL_CHAT:
		case BOTLIB_AI_REPLY_CHAT:
		case BOTLIB_AI_CHAT_LENGTH:
		case BOTLIB_AI_ENTER_CHAT:
		case BOTLIB_AI_STRING_CONTAINS:
		case BOTLIB_AI_FIND_MATCH:
		case BOTLIB_AI_MATCH_VARIABLE:
		case BOTLIB_AI_UNIFY_WHITE_SPACES:
		case BOTLIB_AI_REPLACE_SYNONYMS:
		case BOTLIB_AI_LOAD_CHAT_FILE:
		case BOTLIB_AI_SET_CHAT_GENDER:
		case BOTLIB_AI_SET_CHAT_NAME:

		case BOTLIB_AI_RESET_GOAL_STATE:
		case BOTLIB_AI_RESET_AVOID_GOALS:
		case BOTLIB_AI_PUSH_GOAL:
		case BOTLIB_AI_POP_GOAL:
		case BOTLIB_AI_EMPTY_GOAL_STACK:
		case BOTLIB_AI_DUMP_AVOID_GOALS:
		case BOTLIB_AI_DUMP_GOAL_STACK:
		case BOTLIB_AI_GOAL_NAME:
		case BOTLIB_AI_GET_TOP_GOAL:
		case BOTLIB_AI_GET_SECOND_GOAL:
		case BOTLIB_AI_CHOOSE_LTG_ITEM:
		case BOTLIB_AI_CHOOSE_NBG_ITEM:
		case BOTLIB_AI_TOUCHING_GOAL:
		case BOTLIB_AI_ITEM_GOAL_IN_VIS_BUT_NOT_VISIBLE:
		case BOTLIB_AI_GET_LEVEL_ITEM_GOAL:
		case BOTLIB_AI_AVOID_GOAL_TIME:
		case BOTLIB_AI_INIT_LEVEL_ITEMS:
		case BOTLIB_AI_UPDATE_ENTITY_ITEMS:
		case BOTLIB_AI_LOAD_ITEM_WEIGHTS:
		case BOTLIB_AI_FREE_ITEM_WEIGHTS:
		case BOTLIB_AI_SAVE_GOAL_FUZZY_LOGIC:
		case BOTLIB_AI_ALLOC_GOAL_STATE:
		case BOTLIB_AI_FREE_GOAL_STATE:

		case BOTLIB_AI_RESET_MOVE_STATE:
		case BOTLIB_AI_MOVE_TO_GOAL:
		case BOTLIB_AI_MOVE_IN_DIRECTION:
		case BOTLIB_AI_RESET_AVOID_REACH:
		case BOTLIB_AI_RESET_LAST_AVOID_REACH:
		case BOTLIB_AI_REACHABILITY_AREA:
		case BOTLIB_AI_MOVEMENT_VIEW_TARGET:
		case BOTLIB_AI_ALLOC_MOVE_STATE:
		case BOTLIB_AI_FREE_MOVE_STATE:
		case BOTLIB_AI_INIT_MOVE_STATE:

		case BOTLIB_AI_CHOOSE_BEST_FIGHT_WEAPON:
		case BOTLIB_AI_GET_WEAPON_INFO:
		case BOTLIB_AI_LOAD_WEAPON_WEIGHTS:
		case BOTLIB_AI_ALLOC_WEAPON_STATE:
		case BOTLIB_AI_FREE_WEAPON_STATE:
		case BOTLIB_AI_RESET_WEAPON_STATE:

		case BOTLIB_AI_GENETIC_PARENTS_AND_CHILD_SELECTION:
		case BOTLIB_AI_INTERBREED_GOAL_FUZZY_LOGIC:
		case BOTLIB_AI_MUTATE_GOAL_FUZZY_LOGIC:
		case BOTLIB_AI_GET_NEXT_CAMP_SPOT_GOAL:
		case BOTLIB_AI_GET_MAP_LOCATION_GOAL:
		case BOTLIB_AI_NUM_INITIAL_CHATS:
		case BOTLIB_AI_GET_CHAT_MESSAGE:
		case BOTLIB_AI_REMOVE_FROM_AVOID_GOALS:
		case BOTLIB_AI_PREDICT_VISIBLE_POSITION:

			Con_Printf ( " BOTLIB CALL : should not be \n");
		break;

		default :
			Con_Printf (" Unknown trap : %i ",arg);
		break;
	}




	return 0;

}

int CGAME_Call (int arg,...)
{


	va_list mark;
	va_start (mark,arg);


	switch (arg)
	{
		
	case CG_PRINT:
		Con_Printf(va_arg(mark,const char *));
		break;
	case CG_ERROR:
		Error (va_arg(mark,const char *));
		break;
	case CG_MILLISECONDS:
		return Sys_Get_Time();
		break;
	case CG_CVAR_REGISTER:
		Call_Cvar_Register (va_arg (mark,int),va_arg(mark,const char*),va_arg(mark,const char*),va_arg(mark,vmCvar_t*) );
		break;
	case CG_CVAR_UPDATE:
		Cvar_Update(va_arg(mark, vmCvar_t *));
		break;
	case CG_CVAR_SET:
		Call_Cvar_Set(va_arg(mark,char*),va_arg(mark,char*));
		break;
	case CG_CVAR_VARIABLESTRINGBUFFER:
		Call_Cvar_VariableStringBuffer(va_arg(mark,int) ,va_arg(mark,char *), va_arg(mark,const char *)) ;
		break;
	case CG_ARGC:
		return Cmd_Argc ();
		break;
	case CG_ARGV:
		Call_Cmd_Argv(va_arg(mark,int),va_arg(mark,char *),va_arg(mark,int));
		break;
	case CG_ARGS:
		Call_Cmd_Args( va_arg(mark,int ) ,va_arg (mark,char *));
		break;
	case CG_FS_FOPENFILE:
		return Call_FOpenFile(va_arg(mark,int), va_arg(mark,int*),va_arg(mark, const char*) ) ;
		break;
	case CG_FS_READ:
		Call_FS_Read(va_arg(mark,int),va_arg(mark,int), va_arg(mark,void*));
		break;
	case CG_FS_WRITE:
		Call_FS_Write( va_arg(mark,int)   ,va_arg(mark,int ),va_arg(mark,const void *)); 
		break;
	case CG_FS_FCLOSEFILE:
		FS_FCloseFile( va_arg(mark,int) ); 
		break;
	case CG_SENDCONSOLECOMMAND:
		break;
	case CG_ADDCOMMAND:
		break;
	case CG_SENDCLIENTCOMMAND:
		break;
	case CG_UPDATESCREEN:
		R_Update_Screen (); // ?
		break;

	case CG_CM_LOADMAP:
		break;
	case CG_CM_NUMINLINEMODELS:
		break;
	case CG_CM_INLINEMODEL:
		break;
	case CG_CM_LOADMODEL:
		break;
	case CG_CM_TEMPBOXMODEL:
		break;
	case CG_CM_POINTCONTENTS:
		break;
	case CG_CM_TRANSFORMEDPOINTCONTENTS:
		break;
	case CG_CM_BOXTRACE:
		break;
	case CG_CM_TRANSFORMEDBOXTRACE:
		break;
	case CG_CM_MARKFRAGMENTS:
		break;
	case CG_S_STARTSOUND:
		break;
	case CG_S_STARTLOCALSOUND:
		break;
	case CG_S_CLEARLOOPINGSOUNDS:
		break;
	case CG_S_ADDLOOPINGSOUND:
		break;
	case CG_S_UPDATEENTITYPOSITION:
		break;
	case CG_S_RESPATIALIZE:
		break;
	case CG_S_REGISTERSOUND:
		break;
	case CG_S_STARTBACKGROUNDTRACK:
		break;
	case CG_R_LOADWORLDMAP:
		R_LoadWorldMap(va_arg (mark, const char *) );
		break;
	case CG_R_REGISTERMODEL:
		return R_RegisterModel( va_arg(mark,const char *)) ;
		break;
	case CG_R_REGISTERSKIN:
		return R_RegisterSkin( va_arg(mark,const char *));
		break;
	case CG_R_REGISTERSHADER:
		return R_RegisterShader (va_arg (mark ,const char * ));
		break;
	case CG_R_CLEARSCENE:
		R_ClearScene ();
		break;
	case CG_R_ADDREFENTITYTOSCENE:
		R_AddRefEntityToScene ( va_arg (mark ,const refEntity_t * ));
		break;
	case CG_R_ADDPOLYTOSCENE:
		Call_R_AddPolyToScene(  va_arg(mark,int)  , va_arg(mark,int),va_arg(mark, const polyVert_t *) ); 
		break;
	case CG_R_ADDLIGHTTOSCENE:
		Call_R_AddLightToScene(va_arg(mark,int)  ,va_arg (mark,int),va_arg(mark,int),
			va_arg(mark,int),va_arg(mark, const vec3_t));
		break;
	case CG_R_RENDERSCENE:
		R_RenderScene ( va_arg (mark ,const refdef_t * ));
		break;
	case CG_R_SETCOLOR:
		R_SetColor( va_arg(mark,const float * )) ;
		break;
	case CG_R_DRAWSTRETCHPIC:
		Call_R_DrawStretchPic(va_arg(mark,int),va_arg(mark, int),va_arg(mark, int),va_arg(mark, int)
						,va_arg(mark, int), va_arg(mark,int), va_arg(mark,int),va_arg(mark, int),va_arg(mark, int));
		break;
	case CG_R_MODELBOUNDS:
		break;
	case CG_R_LERPTAG:
		break;
	case CG_GETGLCONFIG:
		GetGlConfig(va_arg (mark,glconfig_t*));
		break;
	case CG_GETGAMESTATE:
		SV_Get_GameState ( va_arg (mark,gameState_t *) );
		break;
	case CG_GETCURRENTSNAPSHOTNUMBER:
		break;
	case CG_GETSNAPSHOT:
		break;
	case CG_GETSERVERCOMMAND:
		break;
	case CG_GETCURRENTCMDNUMBER:
		break;
	case CG_GETUSERCMD:
		break;
	case CG_SETUSERCMDVALUE:
		break;
	case CG_R_REGISTERSHADERNOMIP:
		return  R_RegisterShaderNoMip( va_arg(mark,const char *)) ;
		break;
	case CG_MEMORY_REMAINING:
		return Sys_GetRemaining_Memory();
		break;

	case CG_MEMSET :
	case CG_MEMCPY:
	case CG_STRNCPY:
	case CG_SIN:
	case CG_COS:
	case CG_ATAN2:
	case CG_SQRT:
	case CG_FLOOR:
	case CG_CEIL:

	case CG_TESTPRINTINT:
	case CG_TESTPRINTFLOAT:

		break;

	}



	return 0;
}








SYSCALL * UI_SysCall=UI_Call;
SYSCALL * GAME_SysCall=GAME_Call;
SYSCALL * CGAME_SysCall=CGAME_Call;


