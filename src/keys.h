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


#ifndef KEYS_H__
#define KEYS_H__

int Key_Init ( void );
void Key_Shutdown (void );

void Key_WriteBindings( int file );
void Key_SetBinding( int keynum, const char *binding ) ;
void Key_GetBindingBuf( int keynum, char *buf, int buflen ) ;
void Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) ;

void Key_SetCatcher( int catcher ) ;
int Key_GetCatcher( void );

void OnMouseDown (int wParam );
void OnMouseUp (int wParam);
void OnKey_Action (int wParam, int state );

void Key_ClearStates( void ) ;
int Key_IsDown( int keynum ) ;
int Key_GetOverstrikeMode(void);
void Key_SetOverstrikeMode (int state );

void Key_Update_MousePosition ( int cur_x ,int cur_y );

void Key_Update_Keystate ( int key ,int state ,aboolean is_char );
void Key_Update_Mousestate ( int wParam, int state);


void Key_MouseUp (int wParam);
void Key_MouseDown (int wParam);


#endif 