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
#ifndef __TEX_H__
#define __TEX_H__

#define TEX_MISSING ((uint_t)-1)
#define MAX_TEX 1024



extern uint_t *r_textures;

extern int num_dynamic_tex;



void tex_init(void );
void tex_shutdown(void );
void texture_free (int id );
int complete_texture_load (const char * name ,const int flags);

#endif /*__TEX_H__*/
