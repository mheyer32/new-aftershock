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





// TODO !!!




static int  Keys[256];

static int overstrike_mode =0;
static int ActiveKeyCatcher=0;

typedef struct {
	int keynum;
	char keyname [256];
	char command [256];

}bind_t;

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








int Key_IsDown( int keynum ) 
{
	return Keys[keynum]; 
}

void Key_ClearStates( void ) 
{
	memset(Keys,0,256*sizeof(int));
}



int Key_GetOverstrikeMode(void)
{
	return overstrike_mode;

}
void Key_SetOverstrikeMode (int state )
{
	overstrike_mode=state;
}



// Key Input Handlin is not complete


int Parse_KeyInput (int Vkey,int *isLetter)
{
	int key=0;

	*isLetter=0;
// letters:
	if (Vkey>=0x41 && Vkey<=0x5A)
	{
		key='a'+Vkey-0x41;
		*isLetter=1;
		return key;
	}

// F keys:
	if (Vkey>=0x70 && Vkey<=0x7E)
	{
		key=K_F1 + Vkey - 0x70;

		return key;


	}

	
// numbers:
	if (Vkey>=0x30 && Vkey<=0x39)
	{

		key='0'+Vkey-0x30;
		*isLetter=1;

		return key;


	}

	switch (Vkey)
	{
	case VK_TAB :
		return K_TAB;
		break;
	case VK_RETURN:
		return	K_ENTER;
		break;
	case VK_ESCAPE:
		return K_ESCAPE;
		break;
	case  VK_BACK   :
		return K_BACKSPACE;
		break;
	case  VK_SPACE:
		return K_SPACE;
		break;

	case VK_LSHIFT :
	case VK_RSHIFT :
		return K_SHIFT;
		break;

	case VK_LCONTROL:
	case VK_RCONTROL:
		return K_CTRL;
		break;



	case VK_UP:
		return	K_UPARROW;
		break;

	case VK_DOWN:
		return K_DOWNARROW;
		break;
	case VK_LEFT:
		return K_LEFTARROW;
		break;
	case VK_RIGHT:
		return 	K_RIGHTARROW;
		break;

	case 220 :
		// TILDE : 
		return 1000;
		break;


	case 33:
		return K_PGUP;
		break;

	case 34:
		return K_PGDN;
		break;

	}




	return -1;


}


int Handle_KeyInput(int Key,int state)
{

	int isLetter;
	int res=Parse_KeyInput(Key,&isLetter);

	if (res==-1)
		return -1;


	if (state==1)
	{
		Keys[res]=1;

	}
	else
	{
		Keys[res]=0;
	}
	

	if(isLetter==1)
	{
		return res| K_CHAR_FLAG;
	}

	return res;


}



void OnMouseDown (int wParam )
{

		Cbuf_Execute();
	if (MK_LBUTTON & wParam )
		{
			Keys[K_MOUSE1]=1;
			UI_main(UI_KEY_EVENT,K_MOUSE1,0,0,0,0,0,0);
		}
	if (MK_RBUTTON & wParam )
		{
			Keys[K_MOUSE2]=1;
			UI_main(UI_KEY_EVENT,K_MOUSE2,0,0,0,0,0,0);
		}
	if (MK_MBUTTON & wParam )
		{
			Keys[K_MOUSE3]=1;
			UI_main(UI_KEY_EVENT,K_MOUSE3,0,0,0,0,0,0);
		}



}

void OnMouseUp (int wParam)
{
		if (MK_LBUTTON & wParam )
			{
				Keys[K_MOUSE1]=0;
				
			}
		if (MK_RBUTTON & wParam )
			{
				Keys[K_MOUSE2]=0;
			
			}
		if (MK_MBUTTON & wParam )
			{
				Keys[K_MOUSE3]=0;
			
			}




}


void Key_SetCatcher( int catcher ) 
{

	ActiveKeyCatcher=catcher;
	
}

int Key_GetCatcher( void )
{
	return ActiveKeyCatcher;
}

void OnKey_Action (int wParam, int state )
{
	
	int key=Handle_KeyInput(wParam,state);

	
	if ( ActiveKeyCatcher & KEYCATCH_CONSOLE  && state )
		Con_KeyEvent(key);

	else if ( ActiveKeyCatcher & KEYCATCH_UI && state)
	{
		Cbuf_Execute();
		UI_main(UI_KEY_EVENT,key,0,0,0,0,0,0);
	}

	if ( state && key==1000 )
	{
		Con_Toggle ();

	}



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

	for (kn=keynames ; kn->name ; kn++) {
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

	return(-1);
}

void Key_GetBindingBuf( int keynum, char *buf, int buflen ) 
{

	A_strncpyz(buf,key_bindings[keynum].command,buflen);



}

void Key_SetBinding( int keynum, const char *binding ) 
{
	

}

void  Cmd_Bind (void )
{
	char key [MAX_STRING_CHARS];
	char command [MAX_STRING_CHARS];
	int keynum;


	
	if (Cmd_Argc()>=2)
	{
		Cmd_Argv(1,key,MAX_STRING_CHARS);
		Cmd_Argv(2,command,MAX_STRING_CHARS);

		
		keynum=Key_StringToKeynum(key);


		if (keynum<0 || keynum >255 )
		{
			Con_Printf(" unknown key : %s ",key);
			return;
		}
			


		A_strncpyz( key_bindings[keynum].keyname,key,256);
		A_strncpyz( key_bindings[keynum].command,command,256);

		key_bindings[keynum].keynum=keynum;






	}
	else {
		Con_Printf("usage : bind key command ");
	}


}

void Cmd_Unbindall (void )
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














