/***************************************************************************
                        cvar.c  -  console variables
                             -------------------
    begin                : Sun Oct 17 1999
    copyright            : (C) 1999-2000 by Ingmar Stein
    email                : IngmarStein@Digital-Phenomenon.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "a_shared.h"	/* cvar_t */
#include "console.h"	/* Con_Printf */
#include "command.h"	/* Cbuf_InsertText */
#include "c_var.h"
#include "io.h"			/* FS_Write */


#define MAX_CVARS	1024	

/* TODO: put elsewhere */
extern cvar_t	*sv_cheats;

static cvar_t	cvars[MAX_CVARS];
cvar_t			*cvarlist = 0;
static int		num_cvars = 0;
static aboolean	cvar_dirty = afalse;

/*
============
Cvar_FindVar
============
*/
cvar_t *Cvar_FindVar( const char *var_name )
{
	cvar_t *cvar;

	if( !var_name || !cvarlist ) return( NULL );

	for( cvar = cvarlist; cvar; cvar=cvar->next ) {
		if( !A_stricmp( cvar->name, var_name ) ) {
			return( cvar );
		}
	}

	return( NULL );
}

/*TODO: where to put this? */
aboolean Com_MatchPattern( char *pattern, char *string, aboolean case_sensitive )
{
	char		buf[MAX_STRING_CHARS];
	int			num;
	int			i;
	int			j;
	int			diff;
	char		*pstr;
	aboolean	flag;

	while( *pattern ) {
		if( *pattern == '*' ) {
			pattern++;
			num = 0;
			while( *pattern ) {
				if( *pattern == '*' || *pattern == '?' )
					break;
				buf[num++] = *pattern++;
			}
			buf[num] = 0;
			if( !num )
				continue;
			diff = strlen( string ) - num;
			if( diff < 0 ) {
				return( afalse );
			}
			pstr = string;
			j = 0;
			while( 1 ) {
				for( i=0; buf[i]; i++ ) {
					if( case_sensitive ) {
						if( buf[i] != pstr[i] ) {
							break;
						}
					} else {
						if( toupper( buf[i] ) != toupper( pstr[i] ) ) {
							break;
						}
					}
				}

				if( !buf[i] )
					break;

				j++;
				pstr++;
				if( j > diff )
					return( afalse );
			}
		} else if( *pattern == '?' ) {
		} else if( *pattern == '[' ) {
			if( pattern[1] == '[' ) {
				pattern++;
				continue;
			}
			pattern++;
			flag = afalse;
			if( !*pattern )
				return( afalse );

			while( *pattern ) {
				if( flag )
					break;

				if( *pattern == ']' && pattern[1] != ']' )
					return( afalse );

				if( pattern[1] == '-' && pattern[2] && (pattern[2] != ']' || pattern[3] == ']') ) {
					if( case_sensitive ) {
						if( *string >= pattern[0] && *string <= pattern[2] ) {
							flag = atrue;
						}
					} else {
						if( toupper( *string ) >= toupper( pattern[0] ) && toupper( *string ) <= toupper( pattern[2] ) ) {
							flag = atrue;
						}
					}
					pattern += 3;
				} else {
					if( case_sensitive ) {
						if( *pattern == *string )
							flag = atrue;
					} else {
						if( toupper( *pattern ) == toupper( *string ) )
							flag = atrue;
					}
					pattern++;
				}
			}

			if( !flag )
				return( afalse );

			while( *pattern ) {
				if( *pattern == ']' && pattern[1] != ']' )
					break;
				pattern++;
			}
		} else {
			if( case_sensitive ) {
				if( *pattern != *string )
					return( afalse );
			} else {
				if( toupper( *pattern ) != toupper( *string ) )
					return( afalse );
			}
		}
		pattern++;
		string++;
	}

	return( atrue );
}

static void Cmd_cvarlist( void )
{
	cvar_t	*cvar;
	char	*arg;
	char	flags[8];

	if( Cmd_Argc() > 1 ) {
		Cmd_Argv( 1,arg,1024 );
	} else {
		arg = NULL;
	}

	for( cvar = cvarlist; cvar; cvar=cvar->next ) {
		if( arg && !Com_MatchPattern( arg, cvar->name, afalse ) ) {
			continue;
		}

		flags[0] = cvar->flags & CVAR_SERVERINFO	? 'S' : ' ';
		flags[1] = cvar->flags & CVAR_USERINFO		? 'U' : ' ';
		flags[2] = cvar->flags & CVAR_ROM			? 'R' : ' ';
		flags[3] = cvar->flags & CVAR_INIT			? 'I' : ' ';
		flags[4] = cvar->flags & CVAR_ARCHIVE		? 'A' : ' ';
		flags[5] = cvar->flags & CVAR_LATCH			? 'L' : ' ';
		flags[6] = cvar->flags & CVAR_CHEAT			? 'C' : ' ';
		flags[7] = 0;
		Con_Printf( "%s %s \"%s\"\n", flags, cvar->name, cvar->string );
	}
	Con_Printf( "\n%d total cvars\n", num_cvars );
}

static void Cmd_reset(void)
{
	char buf [1024];
	if(Cmd_Argc() > 1) {
		Cmd_Argv( 1,buf,1024 );
		Cvar_Set( buf, NULL );
	} else {
		Con_Printf("usage: reset <variable>\n");
	}
}

void Cmd_restart(void)
{
	cvar_t *cvar;

	for( cvar=cvarlist; cvar; cvar=cvar->next ) {
		if( !(cvar->flags & CVAR_NORESTART) )
			Cvar_Set( cvar->name, cvar->resetString );
	}
}

/*IS-START*/
static void Cmd_set(void)
{
	char buf1[1024],buf2[1024];
	if( Cmd_Argc() >= 3 ) {
		Cmd_Argv( 1,buf1,1024 );
		Cmd_Argv( 2 ,buf2,1024);
		Cvar_Set( buf1, buf2 );
	} else {
		Con_Printf( "usage: set <variable> <value>\n" );
	}
}

static void Cmd_seta(void)
{
	cvar_t	*cvar;
	char buf1 [1024],buf2[1024];

	if( Cmd_Argc() == 3 ) {
		Cmd_Argv( 1 ,buf1,1024);
		Cmd_Argv( 2 ,buf2,1024);
		cvar = Cvar_Set( buf1, buf2);
		cvar->flags |= CVAR_ARCHIVE;
	} else {
		Con_Printf( "usage: seta <variable> <value>\n" );
	}
}

static void Cmd_setu(void)
{
	cvar_t	*cvar;
	char buf1[1024],buf2[1024];

	if( Cmd_Argc() == 3 ) {
		Cmd_Argv( 1,buf1,1024 );
		Cmd_Argv( 2 ,buf2,1024); 
		cvar = Cvar_Set( buf1, buf2 );
		cvar->flags |= CVAR_USERINFO;
	} else {
		Con_Printf( "usage: setu <variable> <value>\n" );
	}
}

static void Cmd_sets(void)
{
	cvar_t	*cvar;
	char buf1[1024],buf2[1024];

	if( Cmd_Argc() == 3 ) {
		Cmd_Argv( 1,buf1,1024 );
		Cmd_Argv( 2 ,buf2,1024);
		cvar = Cvar_Set( buf1, buf2 );
		cvar->flags |= CVAR_SERVERINFO;
	} else {
		Con_Printf( "usage: sets <variable> <value>\n" );
	}
}
/*IS-END*/

static void Cmd_toggle(void)
{
	cvar_t	*cvar;
	char	arg[1024];

	if( Cmd_Argc() == 2 ) {
		Cmd_Argv( 1,arg,1024 );
		cvar = Cvar_FindVar( arg );
/*MAX-START*/
		if( cvar ) {
			if( cvar->integer ) {
				Cvar_Set( arg, "0" );
			} else {
				Cvar_Set( arg, "1" );
			}
		}
/*MAX-END*/ 
	} else {
		Con_Printf( "usage: toggle <variable>\n" );
	}
}

static void Cmd_vstr(void)
{
	cvar_t	*cvar;
	char buf [1024];

	if( Cmd_Argc() == 2 ) {
		Cmd_Argv( 1,buf,1024 );
		cvar = Cvar_FindVar( buf );
		Cbuf_InsertText( cvar->string );
	} else {
		Con_Printf( "vstr <variablename> : execute a variable command\n" );
	}
}

/*IS-START*/
cvar_t *Cvar_Get( const char *var_name, const char *resetString, int flags )
{
	cvar_t *cvar;

	if( !var_name || !resetString ) {
		Error("Cvar_Get: NULL parameter\n" );
	}

	if( !var_name || strchr( var_name, '\\' ) || strchr( var_name, '\"' ) || strchr( var_name, ';' ) ) {
		Con_Printf( "invalid cvar name string: %s", var_name );
		var_name = "BADNAME";
	}

	cvar = Cvar_FindVar( var_name );

	if( !cvar ) {
		if( num_cvars == MAX_CVARS ) {
			Error("MAX_CVARS" );
		}

		cvar					= &cvars[num_cvars++];
		cvar->name				= copystring( var_name );
		cvar->string			= copystring( resetString );
		/*?? MAX: if were creating, isn't it unmodified? */
		/*?? IS: This is needed, if some initialization routine checks if modified is set */
		cvar->modified			= atrue;
		cvar->modificationCount = 0;
		cvar->value				= (float)atof( resetString );
		cvar->integer			= atoi( resetString );
		cvar->resetString		= copystring( resetString );
		cvar->latchedString		= NULL;
		cvar->flags				= flags;
		cvar->next				= cvarlist;
		cvarlist				= cvar;
		return( cvar );
	}

	if( (cvar->flags & CVAR_USER_CREATED) && !(flags & CVAR_USER_CREATED) && resetString[0] ) {
		cvar->flags &= ~CVAR_USER_CREATED;
		free( cvar->resetString );
		cvar->resetString = copystring( resetString );
	}

	cvar->flags |= flags;
	if( !cvar->resetString[0] ) {
		free( cvar->resetString );
		cvar->resetString = copystring( resetString );
	} else {
		if( resetString[0] && strcmp( cvar->resetString, resetString ) ) {
			Con_Printf( "Warning: cvar %s given initial values \"%s\" and \"%s\"\n", cvar->name, cvar->resetString, resetString );
		}
	}

	if( cvar->latchedString ) {
		Cvar_Set( var_name, cvar->latchedString );
		free( cvar->latchedString );
		cvar->latchedString = NULL;
		return( cvar );
	}

	return( cvar );
}
/*IS-END*/

/* MAX: this isn't working */
/* IS: it isn't?? */
cvar_t *Cvar_Set2( const char *var_name, const char *value, aboolean usercreated )
{
	cvar_t *cvar;

	Con_DPrintf( "Cvar_Set2: %s %s\n", var_name, value );

	if( !var_name || strchr( var_name, '\\' ) || strchr( var_name, '\"' ) || strchr( var_name, ';' ) ) {
		Con_Printf( "invalid cvar name string: %s", var_name );
		var_name = "BADNAME";
	}

	cvar = Cvar_FindVar( var_name );

	if( !cvar ) {
		if( !value ) {
			return( NULL );
		} else {
			if( usercreated ) {
				return( Cvar_Get( var_name, value, CVAR_USER_CREATED ) );
			} else {
				return( Cvar_Get( var_name, value, 0 ) );
			}
		}
	}

	if( !value ) {
		value = cvar->resetString;
	}

	if( !usercreated ) {

		if( cvar->flags & CVAR_ROM ) {
			Con_Printf( "%s is read only\n", cvar->name );
		}

		if( cvar->flags & CVAR_INIT ) {
			Con_Printf( "%s is write protected\n", cvar->name );
		}

		if( cvar->flags & CVAR_LATCH ) {
			if( cvar->latchedString ) {
				if( !strcmp( cvar->latchedString, value ) ) {
					return( cvar );
				}
				free( cvar->latchedString );
			} else {
				if( !strcmp( cvar->string, value ) ) {
					return( cvar );
				}
			}
			Con_Printf( "%s will be changed upon restarting.\n", cvar->name );
			cvar->latchedString = copystring( value );
			cvar->modificationCount++;
			cvar->modified = atrue;
			return( cvar );
		}

		if( (cvar->flags & CVAR_CHEAT) && !sv_cheats->integer ) {
			Con_Printf( "%s is cheat protected\n", cvar->name );
			return( cvar );
		}
	} else {
		if( cvar->latchedString ) {
			free( cvar->latchedString );
			cvar->latchedString = NULL;
		}
	}

	if( !strcmp( cvar->string, value ) ) {
		return( cvar );
	}

	cvar->modificationCount++;
	cvar->modified = atrue;
	free( cvar->string );
	cvar->string = copystring( value );
	cvar->value = (float)atof( cvar->string );
	cvar->integer = atoi( cvar->string );

	/* IS: maybe we can delete cvar_dirty and check for cvar->modified instead */
	/* MAX: yes but the check should be for modified & CVAR_ARCHIVE */
	if( cvar->flags & CVAR_ARCHIVE )
		cvar_dirty = atrue;

	return( cvar );
}

cvar_t *Cvar_Set( const char *var_name, const char *value )
{
	return( Cvar_Set2( var_name, value, atrue ) );
}

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue( const char *var_name, float value )
{
	char val[32];

	if( value == (int)value )
		Com_sprintf( val, sizeof(val), "%i", (int)value );
	else
		Com_sprintf( val, sizeof(val), "%f", value );
	Cvar_Set( var_name, val );
}

cvar_t *Cvar_Reset( const char *var_name )
{
	return( Cvar_Set2( var_name, NULL, afalse ) );
}

aboolean Cvar_Print( const char *var_name )
{
	cvar_t *cvar;

	if( !var_name ) return( afalse );

	cvar = Cvar_FindVar( var_name );
	if( !cvar ) return( afalse );

	Con_Printf( "\"%s\" is:\"%s"S_COLOR_WHITE"\" default:\"%s"S_COLOR_WHITE"\"\n", cvar->name, cvar->string, cvar->resetString );
	if( (cvar->flags & CVAR_LATCH) && cvar->latchedString ) {
		Con_Printf( "latched: %s\n", cvar->latchedString );
	}

	return( atrue );
}

float Cvar_VariableValue( const char *var_name )
{
	cvar_t *cvar;

	if( !var_name ) return( 0.f );

	cvar = Cvar_FindVar( var_name );
	if( !cvar ) return( 0.f );

	return( cvar->value );
}

void Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize )
{
	cvar_t *cvar;

	if( !var_name ) {
		A_strncpyz( buffer, "", bufsize );
		return;
	}

	cvar = Cvar_FindVar( var_name );
	if( !cvar ) {
		A_strncpyz( buffer, "", bufsize );
		return;
	}

	A_strncpyz( buffer, cvar->string, bufsize );
}

int Cvar_VariableIntegerValue( const char *var_name )
{
	cvar_t *cvar;

	if( !var_name ) {
		return( 0 );
	}

	cvar = Cvar_FindVar( var_name );
	if( !cvar ) {
		return( 0 );
	}

	return( cvar->integer );
}

aboolean Cvar_IsDirty(void)
{
	return( cvar_dirty );
}

void Cvar_SetDirty( aboolean state )
{
	cvar_dirty = state;
}

/*
============
Cvar_WriteVariables

Writes lines containing "seta <variable> \"<value>\"" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables( fileHandle_t file )
{
	cvar_t	*cvar;
	char	*value;
	char	buf[1024];

	for( cvar = cvarlist ; cvar ; cvar = cvar->next ) {
		if( cvar->flags & CVAR_ARCHIVE ) {
			if( (cvar->flags & CVAR_LATCH) && cvar->latchedString )
				value = cvar->latchedString;
			else
				value = cvar->string;
			Com_sprintf( buf, sizeof(buf), "seta %s \"%s\"""\n", cvar->name, value );
			FS_Write( buf, strlen( buf ), file );
		}
	}
}

void Cvar_Init( void )
{
	Cmd_AddCommand( "cvar_restart", Cmd_restart );
	Cmd_AddCommand( "cvarlist", Cmd_cvarlist );
	Cmd_AddCommand( "reset", Cmd_reset );
	Cmd_AddCommand( "set", Cmd_set );
	Cmd_AddCommand( "seta", Cmd_seta );
	Cmd_AddCommand( "sets", Cmd_sets );
	Cmd_AddCommand( "setu", Cmd_setu );
	Cmd_AddCommand( "toggle", Cmd_toggle );
	Cmd_AddCommand( "vstr", Cmd_vstr );
}

void Cvar_Shutdown( void )
{
	cvar_t *cvar;

	for( cvar = cvarlist; cvar; cvar=cvar->next ) {
		if( cvar->name )			free( cvar->name );
		if( cvar->resetString )		free( cvar->resetString );
		if( cvar->string )			free( cvar->string );
		if( cvar->latchedString )	free( cvar->latchedString );
	}

	cvarlist = NULL;
}

void Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *resetString, unsigned int flags )
{
	cvar_t	*cur;
	int		i;

	Cvar_Get( var_name, resetString, flags );

	if( !cvar ) return;

	for( i = 0; i<num_cvars; i++ ) {
		cur = &cvars[i];
		if( !A_stricmp( cur->name, var_name ) ) {
			cvar->handle = i;
			cvar->modificationCount = cur->modificationCount;
			cvar->integer = cur->integer;
			cvar->value = cur->value;
			A_strncpyz( cvar->string, cur->string, sizeof(cvar->string) );
			return;
		}
	}
}

void Cvar_Update( vmCvar_t *cvar )
{
	cvar_t	*cur;

	if( !cvar ) return;

	if( cvar->handle < 0 || cvar->handle >= num_cvars ) {
		Error( "Cvar_Update: handle out of range" );
	}

	cur = &cvars[cvar->handle];
	cvar->modificationCount = cur->modificationCount;
	cvar->integer = cur->integer;
	cvar->value = cur->value;
	A_strncpyz( cvar->string, cur->string, sizeof(cvar->string) );
}

