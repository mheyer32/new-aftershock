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
#include "util.h"
#include "command.h"
#include "console.h"
#include "io.h"
#include "c_var.h"
#include "keys.h"

cmd_t *cmdlist;

static unsigned char	wait;
static int				cmd_argc;
static char				cmd_argv[32][MAX_TOKEN_CHARS];
static char				*cmd_null_string = "";
static char				*cmd_args = NULL;

static void Cmd_cmdlist(void)
{
	unsigned int num_cmds = 0;
	cmd_t *cmd;
	char arg[MAX_TOKEN_CHARS];

	if (Cmd_Argc() > 1) {
		for (cmd=cmdlist; cmd; cmd=cmd->next) {
			num_cmds++;
			Cmd_Argv( 1, arg, sizeof(arg) );
			if( !strncmp( cmd->name, arg, strlen(arg) ) ) {
				Con_Printf( "%s\n", cmd->name );
			}
		}
	} else {
		for (cmd=cmdlist; cmd; cmd=cmd->next) {
			num_cmds++;
			Con_Printf( "%s\n", cmd->name );
		}
	}

	Con_Printf( "%d commands\n", num_cmds );
}

// TODO !
// executes the contents of a file as if they were typed in at the console
void Cmd_exec(void)
{
	char			*data;
	int				size;
	fileHandle_t	f;
	char file[MAX_APATH];

	if (Cmd_Argc() > 1)
	{
		Cmd_Argv(1,file,MAX_APATH); 

	}
	else 
	{
		Con_Printf("Usage: exec <file>\n");
		return;
	}

	if (file) {
		
		size = FS_OpenFile(file, &f, FS_READ);
		if(!size) {
			/* TODO */
				Con_Printf("couldn't execute %s\n", file);
			
			return;
		}

		data = malloc( size+1 );

		FS_Read( data, size, f );
		FS_FCloseFile( f );

		data[size] = 0;

		Con_Printf("executing %s...\n", file);
		Cbuf_InsertText(data);
		free(data);
	}
}


void Cmd_Writeconfig(void)
{
	char			filename[MAX_TOKEN_CHARS];
	int				file;

	if(Cmd_Argc() > 1)
		Cmd_Argv( 1, filename, sizeof(filename) );
	else
		A_strncpyz( filename, "q3config.cfg", sizeof(filename) );

	FS_OpenFile( filename, &file, FS_WRITE );
	if(file) {
		//FS_Write( CONFIG_INTRO, strlen( CONFIG_INTRO ), file );
		Key_WriteBindings( file );
		Cvar_WriteVariables( file );
		FS_FCloseFile( file );
	} else {
		Con_Printf("Couldn't write %s\n", filename);
	}
}

void Cmd_wait(void)
{
	wait = 1;
}

/*
=============================================================================

					COMMAND EXECUTION

=============================================================================
*/

/*
============
Cmd_Argc
============
*/
int	Cmd_Argc (void)
{
	return(cmd_argc);
}

/*
============
Cmd_Argv
============
*/
void Cmd_Argv( int n, char *buffer, int bufferLength )
{
	if ( n >= cmd_argc )
		A_strncpyz( buffer, "", bufferLength );
	else
		A_strncpyz( buffer, cmd_argv[n], bufferLength );
}

/*
============
Cmd_Args
============
*/
void Cmd_Args( char *buffer, int bufferLength )
{
	A_strncpyz( buffer, cmd_args, bufferLength );
}

/*
============
Cmd_TokenizeString

Parses the given string into command line tokens.
============
*/
void Cmd_TokenizeString (char *text)
{
	char *com_token;

	cmd_argc = 0;
	cmd_args = NULL;
	
	while (1) {
		/* skip whitespace up to a /n */
		while (*text && *text <= ' ' && *text != '\n') {
			text++;
		}
		
		if ((*text == '\n') || (*text == '\r')) {
			/* a newline seperates commands in the buffer */
			text++;
			break;
		}

		if (!*text) return;
	
		if (cmd_argc == 1)
			cmd_args = text;
			
		com_token = COM_Parse (&text);

		if(!text) return;

		if (cmd_argc < 32) {
			A_strncpyz (cmd_argv[cmd_argc], com_token, MAX_TOKEN_CHARS);
			cmd_argc++;
		}
	}
}

/*
============
Cmd_ExecuteString

A complete command line has been parsed, so try to execute it
============
*/
void Cmd_ExecuteString(char *text)
{
	int argc;
	char arg0[MAX_TOKEN_CHARS];
	char arg1[MAX_TOKEN_CHARS];

	Cmd_TokenizeString(text);

	argc = Cmd_Argc();

	if (argc > 0) {
		Cmd_Argv( 0, arg0, sizeof(arg0) );
		

		if (!Cmd_ExecuteCommand(arg0))
		{
		if (argc > 1) {
			Cmd_Argv( 1, arg1, sizeof(arg1) );
			if(Cvar_FindVar(arg0)  ) {
				Cvar_Set( arg0, arg1 ) ;
				return;
			}
		} else {
			if( Cvar_Print( arg0 ) )
				return;
		}
			Con_Printf( "Unknown command \"%s\"\n", arg0 );

		}
	}
}

static cmd_t *Cmd_Find( const char *name )
{
	cmd_t *cmd;

	if( !name ) return 0;

	for( cmd = cmdlist; cmd; cmd=cmd->next ) {
		if( !A_stricmp(cmd->name, name ) ) {
			return(cmd);
		}
	}

	return 0;
}

/* Add a command */
void Cmd_AddCommand( const char *name, void (*function) (void) )
{
	cmd_t *cmd;

	if( !name )
		return;

	cmd = Cmd_Find( name );

	if (cmd) {
		Con_Printf( "Cmd_AddCommand: %s already defined\n", name );
		return;
	}

	cmd = (cmd_t *) malloc(sizeof(cmd_t));
	A_strncpyz( cmd->name, name, sizeof(cmd->name) );
	cmd->function = function;
	cmd->next = cmdlist;
	cmdlist = cmd;
}

/* Execute a command */
int Cmd_ExecuteCommand( const char *name )
{
	cmd_t *cmd;

	if (!name) return 0;

	cmd = Cmd_Find(name);
	if(!cmd) return 0;

	if( cmd->function )
		cmd->function();
	/* TODO */

	return 1;
}

void Cmd_Init( void )
{
	Cmd_AddCommand( "cmdlist", Cmd_cmdlist );
	Cmd_AddCommand( "exec", Cmd_exec );
}

void Cmd_Shutdown( void )
{
	cmd_t *cmd;
	cmd_t *next;

	for(cmd=cmdlist; cmd; cmd=next) {
		next = cmd->next;
		free( cmd );
	}
}

/*
=============================================================================

						COMMAND BUFFER

=============================================================================
*/

static char *cbuf_data;
static unsigned int cbuf_cursize;
static unsigned int cbuf_maxsize;

/*
============
Cbuf_Init
============
*/
void Cbuf_Init (void)
{
	cbuf_cursize = 0;
	cbuf_maxsize = 8192;
	cbuf_data = (char *)malloc( cbuf_maxsize );		/* space for commands and script files */

	Cmd_AddCommand( "wait", Cmd_wait );
	Cmd_AddCommand( "writeconfig",Cmd_Writeconfig);
}

/*
============
Cbuf_AddText

Adds command text at the end of the buffer
============
*/
void Cbuf_AddText( const char *text )
{
	int		l;

	l = strlen( text );

	if( cbuf_cursize + l >= cbuf_maxsize ) {
		Con_Printf( "Cbuf_AddText: overflow\n" );
		return;
	}

	memcpy( cbuf_data+cbuf_cursize, text, l );
	cbuf_cursize += l;
}

/*
============
Cbuf_InsertText

Adds command text immediately after the current command
Adds a \n to the text
FIXME: actually change the command buffer to do less copying
============
*/
void Cbuf_InsertText( const char *text )
{
	char	*temp;
	int		templen;

	/* copy off any commands still remaining in the exec buffer */
	templen = cbuf_cursize;
	if (templen) {
		temp = (char *)malloc (templen);
		memcpy( temp, cbuf_data, templen );
		cbuf_cursize = 0;
	} else
		temp = NULL;	/* shut up compiler */
		
	/* add the entire text of the file */
	Cbuf_AddText (text);
	
	/* add the copied off data */
	if (templen) {
		if((cbuf_cursize+templen + 1) <= cbuf_maxsize)
			memcpy( cbuf_data+cbuf_cursize, temp, templen );
		else ;
			Con_Printf( "Cbuf_InsertText overflowed\n" );
		free( temp );
		cbuf_cursize += templen;
	}
}

/*
============
Cbuf_Execute
============
*/
void Cbuf_Execute (void)
{
	unsigned int	i;
	char			*text;
	char			line[1024];
	unsigned int	quotes;
	
	while( cbuf_cursize ) {
		text = cbuf_data;

		/* find a \n, \r or ; line break */
		quotes = 0;
		for (i=0 ; i< cbuf_cursize ; i++) {
			if( text[i] == '"' )
				quotes++;
			if( !(quotes&1) && text[i] == ';' )
				break;	/* don't break if inside a quoted string */
			if( (text[i] == '\n') || (text[i] == '\r') )
				break;
		}

		memcpy( line, text, i );
		line[i] = 0;

		/*
		 * delete the text from the command buffer and move remaining commands down
		 * this is necessary because commands (exec, alias) can insert data at the
		 * beginning of the text buffer
		 */

		if( i == cbuf_cursize )
			cbuf_cursize = 0;
		else {
			i++;
			cbuf_cursize -= i;
			memcpy (text, text+i, cbuf_cursize);
		}

		/* execute the command line */
		Cmd_ExecuteString( line );

		if( wait ) {
			/*
			 * skip out while text still remains in buffer, leaving it
			 * for next frame
			 */
			wait = 0;
			break;
		}
	}
}

void Cbuf_ExecuteText( cbufExec_t exec_when, const char *text )
{
	char *tmp;

	switch( exec_when ) {
		case EXEC_NOW:
			tmp = copystring(text);
			Cmd_ExecuteString(tmp);
			free(tmp);
		case EXEC_APPEND:
			Cbuf_AddText(text);
			break;
		case EXEC_INSERT:
			Cbuf_InsertText(text);
			break;
		default:
			Con_Printf( "Cbuf_ExecuteText: bad exec_when\n" );
			break;
	}
}

void Cbuf_Free(void)
{
	cbuf_cursize = 0;
	cbuf_maxsize = 0;
	free( cbuf_data );
}