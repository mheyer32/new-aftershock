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


#include "util.h"
#include "a_shared.h"	
#include "sys_main.h"

#include "render.h"		/* R_DrawString */
#include "console.h"	/* console_t */
#include "c_var.h"		/* Cvar_Get, Cvar_Print */
#include "command.h"	/* Cbuf_AddText */
#include "keycodes.h"
#include "keys.h"
#include "shader.h"
#include "opengl.h"
#include "io.h"


#define BORDER			9

extern unsigned int TotalTime;
extern unsigned int FrameTime;
extern int white;

console_t console;

/* cvars */
static cvar_t *developer;
static cvar_t *logfile;
static cvar_t *scr_conspeed;


extern cvar_t *cl_paused;

static int conlog = 0;

/* clears the console */
static void Cmd_clear(void)
{
	int i;

	for(i=0; i<console.numFilledLines; i++)
		memset( console.chars[i],0, CON_LINE_WIDTH );

	console.vScrollPos = console.numFilledLines = console.newline = 0;
}

/* dumps the console buffer to a file */
static void Cmd_condump(void)
{
/*	file_t fp;
	int i;
	char arg[MAX_TOKEN_CHARS];

	if (Cmd_Argc() > 1) {
		Cmd_Argv( 1, arg, sizeof(arg) );
		fp = FileOpen(arg, OPEN_WRITEONLY);
		if (fp) {
			for (i = 0; i<console.numFilledLines; i++) {
				FilePrintf(fp, "%s"LINE_SEPARATOR, console.chars[i]);
			}
			FileFlush(fp);
			FileClose(fp);
			Con_Printf( "Dumped console text to %s.\n", arg );
		} else {
			Con_Printf( "ERROR: couldn't open\n" );
		}
	} else {
		Con_Printf( "usage: condump <filename>\n" );
	}*/
}

/* echoes a line on the console */
static void Cmd_echo(void)
{
	int		i;
	char	arg[MAX_TOKEN_CHARS];
	
	for (i=1 ; i<Cmd_Argc() ; i++) {
		Cmd_Argv( i, arg, sizeof(arg) );
		Con_Printf( "%s ", arg );
	}

	Con_Printf( "\n" );
}

/* displays the history list */
static void Cmd_history(void)
{
	int i;

	for (i = 0; i < console.numHist; i++) {
		Con_Printf( "%s\n", console.history[i] );
	}
}

/*
 * private functions
 */

static void Con_SetHScrollPos( void )
{
	int widthInChars;

	widthInChars = ((winX - 2 * BORDER) / SMALLCHAR_WIDTH - 1 - strlen( console.prompt ));

	if( console.cursorPos < widthInChars )
		console.hScrollPos = 0;
	else
		console.hScrollPos = console.cursorPos - widthInChars;
}

/* sets an entry from the history list as the new command */
static void Con_History(int up)
{
	int i;
	int histpos;
	int len;

	if (!up)
		return;

	if (up > 0) {
		for (i = 0; i < up; i++) {
			if (console.curHist < console.numHist) {
				console.curHist++;
			}
		}
	} else {
		for (i = 0; i > up; i--) {
			if (console.curHist > 0) {
				console.curHist--;
			}
		}
	}

	histpos = console.numHist - console.curHist;
	memset( console.minibuffer,0, CON_LINE_WIDTH );
	strcpy( console.minibuffer, console.history[histpos] );
	len = strlen( console.minibuffer );
	console.cursorPos = len;
	Con_SetHScrollPos();
}

/* adds a command to the history list */
static void Con_AddToHistory(const char *command)
{
	int i;

	console.curHist = 0;
	if (console.numHist == CON_HIST_SIZE) {
		/* history list is full, so delete first entry */
		for (i = 0; i < console.numHist; i++) {
			strcpy((char *) console.history[i], (char *) console.history[i + 1]);
		}
		strcpy((char *) console.history[console.numHist], command);
	} else {
		strcpy((char *) console.history[console.numHist], command);
		console.numHist++;
	}
}

/* adds a character to the minibuffer */
static void Con_AddToMinibuffer(const char input)
{
	int i;
	int len = strlen(console.minibuffer);

	if (len == CON_MINIBUFFER_SIZE - 1)
		return;

	if( !Key_GetOverstrikeMode() ) {
		for(i = len; i > console.cursorPos; i--) {
			console.minibuffer[i] = console.minibuffer[i-1];
		}
	}

	console.minibuffer[console.cursorPos] = input;
	console.cursorPos++;

	if( console.cursorPos == len + 1 ) {
		console.minibuffer[console.cursorPos] = 0;
	}

	Con_SetHScrollPos();
}

/* adds a string to the console buffer */
static void Con_AddToBuffer(const char *input)
{
	unsigned int i;
	unsigned int j;
	unsigned int len = strlen(input);
	unsigned int newline_len;

	for (i = 0; i < len; i++) {
		if (console.newline == CON_BUFFER_SIZE) {
			for (j = 0; j < CON_BUFFER_SIZE-1; j++) {
				strcpy(console.chars[j], console.chars[j + 1]);
			}

			memset(console.chars[CON_BUFFER_SIZE-1],0, CON_LINE_WIDTH);
			console.numFilledLines--;
			console.newline = CON_BUFFER_SIZE-1;
		}

		newline_len = strlen(console.chars[console.newline]);
		if (newline_len == CON_LINE_WIDTH - 1) {
			console.numFilledLines++;
			console.newline++;
			newline_len = 0;

			if(console.newline == CON_BUFFER_SIZE) {
				for (j = 0; j < CON_BUFFER_SIZE-1; j++) {
					strcpy(console.chars[j], console.chars[j + 1]);
				}

				memset(console.chars[CON_BUFFER_SIZE-1],0, CON_LINE_WIDTH);
				console.numFilledLines--;
				console.newline = CON_BUFFER_SIZE-1;
			}
		}
		
		if ((input[i] == '\n') || (input[i] == '\r')) {
			console.numFilledLines++;
			console.newline++;
		} else {
			if (input[i] == '\t') {
				/* TODO: this may overflow the line buffer */
				for (j = 0; j < 4; j++) {
					console.chars[console.newline][newline_len++] = ' ';
				}
			} else {
				console.chars[console.newline][newline_len] = input[i];
			}
		}
	}
}

/*
 * Process an instruction
 * called after the user hits enter
 */
static void Con_ProcessCmd(void)
{

	char *cmd;

	if(strlen(console.minibuffer)) {
		cmd = copystring(console.minibuffer);

		/* clear the minibuffer */
		console.cursorPos = 0;
		console.hScrollPos = 0;
		memset(console.minibuffer,0, CON_MINIBUFFER_SIZE);

		/* add command to history list */
		Con_AddToHistory(cmd);

		/* print command */
		Con_Printf("%s%s\n", console.prompt, cmd);

		/* execute */
		Cbuf_AddText(cmd);
		Cbuf_AddText("\n");
		Cbuf_Execute();

		free(cmd);
	} else {
		Con_Printf("%s\n", console.prompt);
	}
}

/* tries to complete a partial command/cvar */
static void Con_Complete(void)
{
	cmd_t			*cmd;
	cvar_t			*cvar;
	int				len;
	int				len2;
	char			*minibuf;
	char			*buf;
	char			**list;
	unsigned int	num;
	char			next;
	unsigned int	i;
	unsigned int	j;

	len = strlen( console.minibuffer );
	minibuf = console.minibuffer;

	if (len) {
		/* find number of commands/cvars that begin with minibuf */		
		num = 0;
		for (cvar = cvarlist; cvar; cvar=cvar->next) {
			if( !A_stricmpn( minibuf, cvar->name, len ) ) {
				num++;
			}
		}

		for(cmd = cmdlist; cmd; cmd=cmd->next) {
			if( !A_stricmpn( minibuf, cmd->name, len ) ) {
				num++;
			}
		}

		/* allocate a list of char pointers */
		list = (char **)malloc( num*sizeof(char *) );

		/* fill the list */
		num = 0;
		for (cvar = cvarlist; cvar; cvar=cvar->next) {
			if( !A_stricmpn( minibuf, cvar->name, len ) ) {
				list[num++] = copystring(cvar->name);
			}
		}

		for(cmd = cmdlist; cmd; cmd=cmd->next) {
			if( !A_stricmpn( minibuf, cmd->name, len ) ) {
				list[num++] = copystring(cmd->name);
			}
		}

		/* now go and complete the command */
		while(1) {
			next = 0;
			for (i=0; i<num; i++) {
				len2 = strlen(list[i]);
				if ( !A_stricmpn( minibuf, list[i], len ) ) {
					memcpy(minibuf, list[i], len);
					if (len2 == len) {
						minibuf[len] = ' ';
						console.cursorPos++;
						next = 0;
						break;
					} else if (len2 > len) {
						if (!next) {
							next = list[i][len];
						} else if (list[i][len] != next) {
							buf = copystring(minibuf);
							Con_Printf("%s%s\n", console.prompt, buf);
							free(buf);
							for (j=0; j<num; j++) {
								Con_Printf("%s\n", list[j]);
							}
							next = 0;
							break;
						}
					}
				}
			}
			if(next) {
				minibuf[len++] = next;
				console.cursorPos++;
			} else {
				break;
			}
		}

		/* free the list */
		for (i=0; i<num; i++) {
			free(list[i]);
		}
		free(list);
	}
}

/*
 * public functions
 */
void Con_Init(void)
{
	int i;

	console.numHist = 0;
	console.curHist = 0;
	console.isOpen = 0;
	console.numFilledLines = 0;
	console.height = 0;
	console.vScrollPos = 0;
	console.hScrollPos = 0;
	console.cursorPos = 0;
	console.shader = &shader_console;
	console.prompt = "]";
	console.newline = 0;

	memset(console.minibuffer,0, CON_MINIBUFFER_SIZE);

	for(i=0; i<CON_BUFFER_SIZE; i++)
		memset(console.chars[i],0, CON_LINE_WIDTH);

	developer = Cvar_Get( "developer", "0", 0 );
	logfile = Cvar_Get( "logfile", "0", 0 );
#ifndef _WIN32
/* TODO: documentation */
	nostdout = Cvar_Get( "nostdout", "0", 0 );
#endif
	scr_conspeed = Cvar_Get( "scr_conspeed", "3", 0 );

	Cmd_AddCommand( "clear", Cmd_clear );
	Cmd_AddCommand( "condump", Cmd_condump );
	Cmd_AddCommand( "echo", Cmd_echo );
	Cmd_AddCommand( "history", Cmd_history );
	Cmd_AddCommand( "toggleconsole", Con_Toggle );
}

void Con_Shutdown(void)
{/*
	if( conlog ) {
		FileFlush( conlog );
		FileClose( conlog );
		conlog = 0;
		Cvar_Set2( "logfile", "0" );
	}*/
}

void Con_Toggle(void)
{
	if( console.isOpen ) {
		console.isOpen = 0;
		Key_SetCatcher( Key_GetCatcher() & ~KEYCATCH_CONSOLE );
		Cvar_Set2( "cl_paused", "1",0 );
	} else {
		console.isOpen = 1;
		Key_SetCatcher( Key_GetCatcher() | KEYCATCH_CONSOLE );
		Cvar_Set2( "cl_paused", "0",0 );
	}

	memset(console.minibuffer,0, CON_MINIBUFFER_SIZE);
	console.cursorPos = 0;
	console.hScrollPos = 0;
}

/* draws the console */
void Con_Draw(void)
{
	int i;
	int y;
	int line_idx;
	int ch;
	char buf[2];
	

	if (console.isOpen) {
		if (console.height < winY / 2) {
			console.height += (int)(5.0*scr_conspeed->value);
		}
	} else {
		if (console.height > 0.f) {
			console.height -= (int)(5.0*scr_conspeed->value);
		}
	}

	if (console.height > winY / 2) {
		console.height = winY / 2;
	} else if (console.height < 0) {
		console.height = 0;
	}

	if (!console.height)
		return;

	y = console.height;

	/* background */
	R_SetColor( colorWhite );
	R_DrawStretchPic( 0.0f, 0.0f, (float)winX, y, 0.f, 1.f - (float)console.height / (float)winY, 1.f, 1.f, *console.shader );

	y -= 2;

	if( y < -SMALLCHAR_HEIGHT ) return;	/* offscreen */

	/* bottom line */
	R_SetColor(colorYellow);
	// TODO
	//R_DrawStretchPic(0.f, y, (float)winX, 2.f, 0.f, 0.f, 0.f, 0.f,1, shader_white);
	y -= (BORDER + SMALLCHAR_HEIGHT);

	if( y < -SMALLCHAR_HEIGHT ) return;	/* offscreen */

	/* version string */
	R_DrawString( winX - BORDER - strlen(AFTERSHOCK_VERSION) * SMALLCHAR_WIDTH, y, AFTERSHOCK_VERSION, colorWhite );

	y -= SMALLCHAR_HEIGHT;

	if( y < -SMALLCHAR_HEIGHT ) return;	/* offscreen */

	/* prompt */
	R_DrawString(BORDER, y, console.prompt, colorWhite);

	/* minibuffer */
	R_DrawString(BORDER+strlen(console.prompt)*SMALLCHAR_WIDTH, y, console.minibuffer+console.hScrollPos, colorWhite);

	/* cursor */
	if ((Sys_Get_Time ()/BLINK_DIVISOR) & 1) {
		if(Key_GetOverstrikeMode()) {
			ch = 11;
		} else {
			ch = 10;
		}

		buf[0] = ch;
		buf[1] = 0;
		R_DrawString(BORDER + (console.cursorPos - console.hScrollPos + strlen(console.prompt)) * SMALLCHAR_WIDTH, y, buf, colorWhite);
		/* TODO */
//		R_DrawChar(BORDER + (console.cursorPos - console.hScrollPos + strlen(console.prompt)) * SMALLCHAR_WIDTH, y, ch, colorWhite);
	}

	y -= SMALLCHAR_HEIGHT;

	if( y < -SMALLCHAR_HEIGHT ) return;	/* offscreen */

	/* last line */
	if(console.numFilledLines) {
		if (console.vScrollPos > 0) {
			buf[0] = '^';
			buf[1] = 0;
			for( i=0; i<20; i++ ) {
				R_DrawString(BORDER+i*4*SMALLCHAR_WIDTH, y, buf, colorYellow);
			}
		} else {
			line_idx = console.numFilledLines - 1;
			if(line_idx >= 0) {
				R_DrawString(BORDER, y, console.chars[line_idx], colorWhite);
			}
		}
	}

	y -= SMALLCHAR_HEIGHT;

	/* draw visible lines */
	if(console.numFilledLines > 1) {

		for (i = console.numFilledLines-2; i >= 0; i--, y -= SMALLCHAR_HEIGHT) {

			if( y < -SMALLCHAR_HEIGHT ) return;	/* offscreen */

			line_idx = i - console.vScrollPos;
			if(line_idx < 0) break;

			R_DrawString(BORDER, y, console.chars[line_idx], colorWhite);
		}
	}
}

/* prints a line on the console */
void Con_Printf(const char *format, ...)
{
	char buffer[MAX_STRING_CHARS * 2];
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	

/*	if(logfile->integer && !conlog) {
		time_t t;
		struct tm *local_time;

		conlog = FileOpen("aconsole.log", OPEN_APPEND);
		if(conlog) {
			time(&t);
			local_time = localtime(&t);
			Con_Printf("Opened log %s\n", asctime(local_time));
		} else {
			Cvar_Set2( "logfile", "0" );
			Con_Printf("can't open the log file %s\n", Sys_GetErrorString());
		}
	} else if(!logfile->integer && conlog) {
		FileFlush(conlog);
		FileClose(conlog);
		conlog = 0;
	}
*/
/*	if(conlog) {
		FileWriteString(buffer, conlog);
	}
*/

	Con_AddToBuffer(buffer);


}

/* prints a line on the console only if developer is set */
void Con_DPrintf(const char *format, ...)
{
	char buffer[1024];
	va_list args;

	if(developer->integer) {

		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);

		Con_Printf("%s", buffer);
	}
}

static void Con_Paste( void )
{
	/*char pasteBuffer[CON_MINIBUFFER_SIZE];
	int pasteLen;
	int i;

	Sys_GetClipboardData(pasteBuffer, sizeof(pasteBuffer));
	pasteLen = strlen( pasteBuffer );
	for ( i = 0 ; i < pasteLen ; i++ ) {
		Con_KeyEvent( pasteBuffer[i]|K_CHAR_FLAG );
	}*/
}

static void Con_Delete( void )
{
	int len;
	int i;

	len = strlen( console.minibuffer );
	if (console.cursorPos < len) {
		for(i=console.cursorPos; i<len-1; i++) {
			console.minibuffer[i] = console.minibuffer[i+1];
		}
		console.minibuffer[i] = 0;
	}
}

void Con_KeyEvent( int key )
{
	int len;
	int i;

	switch(key) {
		case K_UPARROW:
		case K_KP_UPARROW:
			Con_History(1);
			break;

		case K_DOWNARROW:
		case K_KP_DOWNARROW:
			Con_History(-1);
			break;

		case K_LEFTARROW:
		case K_KP_LEFTARROW:
			if (console.cursorPos) {
				console.cursorPos--;
			}
			Con_SetHScrollPos();
			break;

		case K_RIGHTARROW:
		case K_KP_RIGHTARROW:
			len = (int)strlen(console.minibuffer);
			if (console.cursorPos < strlen(console.minibuffer)) {
				console.cursorPos++;
			}
			Con_SetHScrollPos();
			break;

		case K_BACKSPACE:
			len = strlen(console.minibuffer);
			if (console.hScrollPos > 0)
				console.hScrollPos--;
			if (len && console.cursorPos) {
				for(i=console.cursorPos-1; i<len-1; i++) {
					console.minibuffer[i] = console.minibuffer[i+1];
				}
				console.minibuffer[i] = 0;
				console.cursorPos--;
			}
			break;

		case K_TAB:
			Con_Complete();
			break;

		case K_INS:
		case K_KP_INS:
			if( Key_IsDown( K_SHIFT ) ) {
				Con_Paste();
			} else {
				Key_SetOverstrikeMode( !Key_GetOverstrikeMode() );
			}
			break;

		case K_DEL:
		case K_KP_DEL:
			Con_Delete();
			break;

		case K_PGUP:
		case K_KP_PGUP:
			if (console.vScrollPos < console.numFilledLines - 1) {
				console.vScrollPos++;
			}
			break;

		case K_PGDN:
		case K_KP_PGDN:
			if (console.vScrollPos > 0) {
				console.vScrollPos--;
			}
			break;

		case K_HOME:
		case K_KP_HOME:
			if (Key_IsDown(K_CTRL)) {
				console.vScrollPos = console.numFilledLines - 1;
			} else {
				console.hScrollPos = 0;
				console.cursorPos = 0;
			}
			break;

		case K_END:
		case K_KP_END:
			if (Key_IsDown(K_CTRL)) {
				console.vScrollPos = 0;
			} else {
				console.cursorPos = strlen(console.minibuffer);
				Con_SetHScrollPos();
			}
			break;

		case K_ENTER:
			Con_ProcessCmd();
			break;
		case K_SPACE:
			Con_AddToMinibuffer(' ');
			break;

		default:
			if ( key & K_CHAR_FLAG ) {
				key &= ~K_CHAR_FLAG;

				switch (key) {
					case 1:		/* ^A */
						console.cursorPos = 0;
						console.hScrollPos = 0;
						break;

					case 3:		/* ^C */
						memset(console.minibuffer,0, CON_MINIBUFFER_SIZE);
						console.cursorPos = 0;
						console.hScrollPos = 0;
						break;

					case 5:		/* ^E */
						console.cursorPos = strlen(console.minibuffer);
						Con_SetHScrollPos();
						break;

					case 12:		/* ^L */
						Cmd_clear();
						break;

					case 14:		/* ^N */
						Con_History(1);
						break;

					case 16:		/* ^P */
						Con_History(-1);
						break;

					case 22:		/* ^V */
						Con_Paste();
						break;

					case 127:		/* delete */
						Con_Delete();
						break;

					default:
						Con_AddToMinibuffer((char)key);
						break;
				}
			}
	}
}