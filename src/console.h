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


#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#if _MSC_VER > 1000
# pragma once
#endif

#define CON_HIST_SIZE		60
#define CON_BUFFER_SIZE		512
#define CON_LINE_WIDTH		78
#define CON_MINIBUFFER_SIZE	256
#define CON_PROMPT			"]"

typedef struct console_s {
	int height;
	aboolean isOpen;
	char history[CON_HIST_SIZE][CON_LINE_WIDTH];
	char chars[CON_BUFFER_SIZE][CON_LINE_WIDTH];
	char minibuffer[CON_MINIBUFFER_SIZE];
	int vScrollPos;
	int hScrollPos;
	int curHist;
	int newline;
	int numFilledLines;
	int numHist;
	int cursorPos;
} console_t;

extern console_t console;

void Con_Init(void);
void Con_Shutdown(void);
void Con_Draw(void);			/* draws the console */
void Con_Printf(const char *, ...);	/* prints a line on a console */
void Con_DPrintf(const char *, ...);
void Con_KeyEvent( int key );
void Con_Toggle( void );

#endif		