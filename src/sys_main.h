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

#ifndef SYS_MAIN_H__
#define SYS_MAIN_H__

#ifdef _WIN32 

#include "sys_win.h" 

#define Sys_Get_Time  WIN_Get_Time 

#define Sys_GetRemaining_Memory WIN_GetRemaining_Memory
#define Sys_SetPriority (p) WIN_SetPriority(p)


#else 

#error "NOT RUNNING UNDER WIN32 : NO OTHER PLATFORMS SUPPORTED !"

#endif 


#endif 