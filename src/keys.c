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

#include <windows.h>
#include "keycodes.h"
#include "a_shared.h"
#include "ui.h"
#include "command.h"
#include "console.h"
#include "io.h"
#include "client.h"

static aboolean Keys[256];
static int overstrike_mode = 0;
static int ActiveKeyCatcher = 0;

typedef struct {
	char keyname[MAX_APATH];
	char command[MAX_APATH];
} bind_t;

bind_t key_bindings[256];

typedef struct keyname_s {
	char	*name;
	int		keynum;
} keyname_t;

static keyname_t keynames[] = {
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},
	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},
	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},
	{"COMMAND", K_COMMAND},
	{"CAPSLOCK", K_CAPSLOCK},
	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},
	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGUP", K_PGUP},
	{"PGDN", K_PGDN},
	{"HOME", K_HOME},
	{"END", K_END},
	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},
	{"MOUSE4", K_MOUSE4},
	{"MOUSE5", K_MOUSE5},
	{"MWHEELUP", K_MWHEELUP},
	{"MWHEELDOWN", K_MWHEELDOWN},
	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},
	{"JOY5", K_JOY5},
	{"JOY6", K_JOY6},
	{"JOY7", K_JOY7},
	{"JOY8", K_JOY8},
	{"JOY9", K_JOY9},
	{"JOY10", K_JOY10},
	{"JOY11", K_JOY11},
	{"JOY12", K_JOY12},
	{"JOY13", K_JOY13},
	{"JOY14", K_JOY14},
	{"JOY15", K_JOY15},
	{"JOY16", K_JOY16},
	{"JOY17", K_JOY17},
	{"JOY18", K_JOY18},
	{"JOY19", K_JOY19},
	{"JOY20", K_JOY20},
	{"JOY21", K_JOY21},
	{"JOY22", K_JOY22},
	{"JOY23", K_JOY23},
	{"JOY24", K_JOY24},
	{"JOY25", K_JOY25},
	{"JOY26", K_JOY26},
	{"JOY27", K_JOY27},
	{"JOY28", K_JOY28},
	{"JOY29", K_JOY29},
	{"JOY30", K_JOY30},
	{"JOY31", K_JOY31},
	{"JOY32", K_JOY32},
	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"KP_HOME", K_KP_HOME},
	{"KP_UPARROW", K_KP_UPARROW},
	{"KP_PGUP", K_KP_PGUP},
	{"KP_LEFTARROW", K_KP_LEFTARROW},
	{"KP_5", K_KP_5},
	{"KP_RIGHTARROW", K_KP_RIGHTARROW},
	{"KP_END", K_KP_END},
	{"KP_DOWNARROW", K_KP_DOWNARROW},
	{"KP_PGDN", K_KP_PGDN},
	{"KP_ENTER", K_KP_ENTER},
	{"KP_INS", K_KP_INS},
	{"KP_DEL", K_KP_DEL},
	{"KP_SLASH", K_KP_SLASH},
	{"KP_MINUS", K_KP_MINUS},
	{"KP_PLUS", K_KP_PLUS},
	{"KP_EQUALS", K_KP_EQUALS},
	{"PAUSE", K_PAUSE},
//	{"SEMICOLON", K_SEMICOLON},
	{NULL, 0}
};

static void  Cmd_Bind (void);
static void Cmd_Unbindall (void);

int Key_Init(void)
{
	memset (Keys, 0, 256 * sizeof(aboolean));

	Cmd_AddCommand("bind", Cmd_Bind);
	Cmd_AddCommand("unbindall", Cmd_Unbindall);

	return 1;
}

void Key_Shutdown (void )
{
}

int Key_IsDown (int keynum) 
{
	return (int)Keys[keynum]; 
}

void Key_ClearStates (void)
{
	memset(Keys, 0, 256 * sizeof(int));
}

int Key_GetOverstrikeMode(void)
{
	return overstrike_mode;
}

void Key_SetOverstrikeMode (int state)
{
	overstrike_mode = state;
}

void Key_Update_Keystate (int key, int state, aboolean is_char)
{
	if ((key < 0) || (key >= 256))
		return;

	if ((key == K_TILDE) && state)
	{
		Con_Toggle();
		return;
	}

	if ((Keys[key] != state) || state)
	{
		Keys[key] = state;

		if (is_char)
		{
			if (Keys[K_SHIFT])
			{
				switch (key)
				{
					case '1':
						key = '!';
						break;

					case '2':
						key = '@';
						break;

					case '3':
						key = '#';
						break;

					case '4':
						key = '$';
						break;

					case '5':
						key = '%';
						break;

					case '6':
						key = '^';
						break;

					case '7':
						key = '&';
						break;

					case '8':
						key = '*';
						break;

					case '9':
						key = '(';
						break;

					case '0':
						key = ')';
						break;

					case '-':
						key = '_';
						break;

					case '=':
						key = '+';
						break;

					default:
						key = toupper(key);
						break;
				}
			}
			
			key |= K_CHAR_FLAG;
		}

		if ((ActiveKeyCatcher & KEYCATCH_CONSOLE) && state)
		{
			Con_KeyEvent(key);
		}
		else if ((ActiveKeyCatcher & KEYCATCH_UI) && state)
		{
			UI_KeyEvent(key);
		}
		else if (ActiveKeyCatcher & KEYCATCH_MESSAGE)
		{
		}
		else 
		{
			CL_KeyEvent(key);
		}
	}
}

static int lastx = 0, lasty = 0;

void Key_Update_MousePosition(int cur_x, int cur_y)
{
	int x, y;

	if ((cur_x != lastx) || (cur_y != lasty))
	{
		x = cur_x - lastx;
		y = cur_y - lasty;

		if (ActiveKeyCatcher & KEYCATCH_CONSOLE )
		{
		}
		else if ( ActiveKeyCatcher & KEYCATCH_UI )
		{
			UI_MouseEvent (x, y);
			lastx = cur_x;
			lasty = cur_y;
		}
		else if ( ActiveKeyCatcher & KEYCATCH_MESSAGE )
		{
		}
		else
		{
		}

	}
}

// TODO !!!
void Key_MouseDown (int wParam )
{
	Cbuf_Execute();

	if (MK_LBUTTON & wParam)
	{
		Keys[K_MOUSE1] = 1;
		UI_main(UI_KEY_EVENT, K_MOUSE1, 0, 0, 0, 0, 0, 0);
	}

	if (MK_RBUTTON & wParam)
	{
		Keys[K_MOUSE2] = 1;
		UI_main(UI_KEY_EVENT, K_MOUSE2, 0, 0, 0, 0, 0, 0);
	}

	if (MK_MBUTTON & wParam)
	{
		Keys[K_MOUSE3] = 1;
		UI_main(UI_KEY_EVENT, K_MOUSE3, 0, 0, 0, 0, 0, 0);
	}
}

// TODO !!!
void Key_MouseUp (int wParam)
{
	if (MK_LBUTTON & wParam)
		Keys[K_MOUSE1] =0;
	
	if (MK_RBUTTON & wParam)
		Keys[K_MOUSE2] = 0;
	
	if (MK_MBUTTON & wParam)
		Keys[K_MOUSE3] = 0;
}


void Key_SetCatcher(int catcher) 
{
	ActiveKeyCatcher = catcher;
}

int Key_GetCatcher(void)
{
	return ActiveKeyCatcher;
}

void Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) 
{
	keyname_t *kn;
	char charbuf[2];

	if (keynum < 0 || keynum > 255) {
		A_strncpyz(buf, "<KEY NOT FOUND>", buflen);
		return;
	}

	if (keynum > 32 && keynum < 127) {
		charbuf[0] = keynum;
		charbuf[1] = 0;
		A_strncpyz(buf, charbuf, buflen);
		return;
	}

	for (kn = keynames ; kn->name ; kn++) {
		if (keynum == kn->keynum) {
			A_strncpyz(buf, kn->name, buflen);
			return;
		}
	}

	A_strncpyz(buf, "<OUT OF RANGE>", buflen);
}


char *Key_KeynumToString(int keynum)
{
	keyname_t *kn;
	static char buf[2];

	if (keynum < 0 || keynum > 255)
		return("<KEY NOT FOUND>");

	if (keynum > 32 && keynum < 127) {
		buf[0] = keynum;
		buf[1] = 0;
		return (buf);
	}

	for (kn=keynames ; kn->name ; kn++) {
		if (keynum == kn->keynum) {
			return(kn->name);
		}
	}

	return("<OUT OF RANGE>");
}

int Key_StringToKeynum(const char *a)
{
	keyname_t	*kn;

	if(!a || !a[0])	return(-1);
	if(!a[1])	return(a[0]);

	for(kn = keynames; kn->name; kn++) {
		if(!A_stricmp(a, kn->name)) {
			return(kn->keynum);
		}
	}

	return -1;
}

void Key_GetBindingBuf( int keynum, char *buf, int buflen ) 
{
	A_strncpyz(buf, key_bindings[keynum].command, buflen);
}

void Key_SetBinding( int keynum, const char *binding ) 
{
	bind_t *bind;

	if (keynum < 0 || keynum >= K_LAST_KEY )
		return;

	bind = &key_bindings [keynum];

	A_strncpyz (bind->command, binding, MAX_APATH);
}

static void  Cmd_Bind (void )
{
	char key [MAX_STRING_CHARS];
	char command [MAX_STRING_CHARS];
	int keynum;

	if (Cmd_Argc()>=2)
	{
		Cmd_Argv(1, key, MAX_STRING_CHARS);
		Cmd_Argv(2, command, MAX_STRING_CHARS);

		
		keynum = Key_StringToKeynum(key);

		if (keynum < 0 || keynum > 255 )
		{
			Con_Printf("unknown key: %s ", key);
			return;
		}
			
		A_strncpyz( key_bindings[keynum].keyname, key, MAX_APATH);
		A_strncpyz( key_bindings[keynum].command, command, MAX_APATH);
	}
	else {
		Con_Printf("usage : bind key command ");
	}
}

static void Cmd_Unbindall (void )
{
	int i;

	for (i=0;i<256;i++)
	{
		A_strncpyz(key_bindings[i].keyname," ",256);
		A_strncpyz(key_bindings[i].command," ",256);
	}
}

void Key_WriteBindings( int file )
{
	int		i;
	char	buf[1024];

	for (i=0 ; i<256 ; i++) {
		if (key_bindings[i].command && *key_bindings[i].command) {
			Com_sprintf( buf, sizeof(buf), "bind %s \"%s\"""\r\n", Key_KeynumToString(i), key_bindings[i].command );
			FS_Write( buf, strlen(buf), file );
		}
	}
}

