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


#ifndef C_VAR_H 
#define C_VAR_H



#define MAX_CVARS 1024


extern int cvar_count;
extern cvar_t			*cvarlist ;


cvar_t *Cvar_Set( const char *var_name, const char *value );
cvar_t *Cvar_FindVar( const char *var_name );
aboolean Com_MatchPattern( char *pattern, char *string, aboolean case_sensitive );
cvar_t *Cvar_Get( const char *var_name, const char *resetString, int flags );
cvar_t *Cvar_Set2( const char *var_name, const char *value, aboolean usercreated );
cvar_t *Cvar_Set( const char *var_name, const char *value );
void Cvar_SetValue( const char *var_name, float value );
cvar_t *Cvar_Reset( const char *var_name );
float Cvar_VariableValue( const char *var_name );
void Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
int Cvar_VariableIntegerValue( const char *var_name );
void Cvar_WriteVariables( fileHandle_t file );

void Cvar_Init( void );
void Cvar_Shutdown( void );
void Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *resetString, unsigned int flags );
void Cvar_Update( vmCvar_t *cvar );
aboolean Cvar_Print( const char *var_name );







#endif