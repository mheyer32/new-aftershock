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




#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#if _MSC_VER > 1000
# pragma once
#endif

typedef struct cmd_s {
	char name[64];
	void (*function) (void);
	struct cmd_s *next;
} cmd_t;

extern cmd_t *cmdlist;

void Cbuf_Init (void);
/* allocates an initial text buffer */

void Cbuf_AddText (const char *text);
/*
 * as new commands are generated from the console or keybindings,
 * the text is added to the end of the command buffer.
 */

void Cbuf_InsertText (const char *text);
/* when a command wants to issue other commands immediately, the text is
 * inserted at the beginning of the buffer, before any remaining unexecuted
 * commands.
 */

void Cbuf_Execute (void);
/* Pulls off \n terminated lines of text from the command buffer and sends
 * them through Cmd_ExecuteString.  Stops when the buffer is empty.
 * Normally called once per frame, but may be explicitly invoked.
 * Do not call inside a command function!
 */

void Cbuf_Free(void);

void Cbuf_ExecuteText(int exec_when, const char *text);

int			Cmd_Argc (void);
void		Cmd_Argv( int n, char *buffer, int bufferLength );
void		Cmd_Args( char *buffer, int bufferLength );
void		Cmd_AddCommand(const char *, void (*function) (void));
int     	Cmd_ExecuteCommand(const char *);
void		Cmd_TokenizeString (char *);
void		Cmd_Init(void);
void		Cmd_Shutdown(void);




#endif /* COMMAND_H_INCLUDED */