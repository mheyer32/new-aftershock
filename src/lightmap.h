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
#ifndef __LIGHTMAP_H__
#define __LIGHTMAP_H__

#define	LIGHTMAP_WIDTH		128
#define	LIGHTMAP_HEIGHT		128

extern int r_numLightmaps;
extern uint_t *r_lightmaps;

void lightmap_loadobjs(void);
void lightmap_freeobjs(void);

#endif /*__LIGHTMAP_H__*/
