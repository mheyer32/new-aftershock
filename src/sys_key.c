#include <windows.h>
#include "a_shared.h"
#include "keycodes.h"
#include "keys.h"

void Sys_Keyboard_Event (unsigned int Vkey, aboolean down)
{
	int key = -1;
	int is_char = 0;

	if (Vkey >= 0x30 && Vkey <= 0x39)
	{
		is_char = 1;
		key = Vkey - 0x30 + '0';
	}
	else if (Vkey >= 0x41 && Vkey <= 0x5A)
	{
		is_char = 1;
		key = Vkey - 0x41 + 'a';
	}
	else if (Vkey >= 0x70 && Vkey <= 0x7E) 
	{
		key = Vkey - 0x70 + K_F1;
	}
	else {
		switch (Vkey)
		{
			case VK_TAB:
				key = K_TAB;
				break;

			case VK_RETURN:
				key = K_ENTER;
				break;

			case VK_ESCAPE:
				key = K_ESCAPE;
				break; 	// Vic: ESCAPE = SPACE?

			case VK_SPACE:	
				key = K_SPACE;
				break;

			case VK_BACK:
				key = K_BACKSPACE;
				break;

			case VK_PAUSE:
				key = K_PAUSE;
				break;

			case VK_UP:
				key = K_UPARROW;
				break;

			case VK_DOWN:
				key = K_DOWNARROW;
				break;

			case VK_LEFT:
				key = K_LEFTARROW;
				break;

			case VK_RIGHT:
				key = K_RIGHTARROW;
				break;

			case 18:
				key = K_ALT;
				break;

			case VK_CONTROL:
				key = K_CTRL;
				break;

			case VK_SHIFT:
				key = K_SHIFT;
				break;

			case VK_INSERT:
				key = K_INS;
				break;

			case VK_DELETE:
				key = K_DEL;
				break;

			case 34:
				key = K_PGDN;
				break;

			case 33:
				key = K_PGUP;
				break;

			case VK_HOME:
				key = K_HOME;
				break;

			case VK_END:
				key = K_END;
				break;

			case VK_NUMPAD8:
				key = K_KP_UPARROW;
				break;
			case VK_NUMPAD9:
				key = K_KP_PGUP;
				break;

			case VK_NUMPAD4:
				key = K_KP_LEFTARROW;
				break;

			case VK_NUMPAD5:
				key = K_KP_5;
				break;

			case VK_NUMPAD6: 
				key = K_KP_RIGHTARROW;
				break;

			case VK_NUMPAD1:
				key = K_KP_END;
				break;

			case VK_NUMPAD2:
				key = K_KP_DOWNARROW;
				break;

			case VK_NUMPAD3:
				key = K_KP_PGDN;
				break;

			case VK_DIVIDE:
				key = K_KP_SLASH;
				break;

			case VK_ADD:
				key = K_KP_PLUS;
				break;

			case VK_NUMLOCK:
				key = K_KP_NUMLOCK;
				break;

			case 192: // TILDE: Vic : was 220 (?)
				key = K_TILDE;
				break;

			case 189: // minus
				key = '-';
				is_char = atrue;
				break;

			case 187: // plus
				key = '+';
				is_char = atrue;
				break;

			case 191:
				key = '/';
				is_char = atrue;
				break;

			case 220:
				key = '\\';
				is_char = atrue;
				break;
			
			case 190:
				key = '.';
				is_char = atrue;
				break;

			default:
				break;
				
		//K_KP_STAR,
		//K_KP_EQUALS,

		//K_MOUSE1,
		//K_MOUSE2,
		//K_MOUSE3,
		//K_MOUSE4,
		//K_MOUSE5,

		//K_MWHEELDOWN,
		//K_MWHEELUP,

		/*K_JOY1,
		K_JOY2,
		K_JOY3,
		K_JOY4,
		K_JOY5,
		K_JOY6,
		K_JOY7,
		K_JOY8,
		K_JOY9,
		K_JOY10,
		K_JOY11,
		K_JOY12,
		K_JOY13,
		K_JOY14,
		K_JOY15,
		K_JOY16,
		K_JOY17,
		K_JOY18,
		K_JOY19,
		K_JOY20,
		K_JOY21,
		K_JOY22,
		K_JOY23,
		K_JOY24,
		K_JOY25,
		K_JOY26,
		K_JOY27,
		K_JOY28,
		K_JOY29,
		K_JOY30,
		K_JOY31,
		K_JOY32,

		K_AUX1,
		K_AUX2,
		K_AUX3,
		K_AUX4,
		K_AUX5,
		K_AUX6,
		K_AUX7,
		K_AUX8,
		K_AUX9,
		K_AUX10,
		K_AUX11,
		K_AUX12,
		K_AUX13,
		K_AUX14,
		K_AUX15,
		K_AUX16,
		*/
		}
	}

	Key_Update_Keystate (key, down, is_char);
}
