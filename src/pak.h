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
#ifndef __PAK_H__
#define __PAK_H__

void Pak_Init (void);
void Pak_Shutdown (void);

int Pak_OpenPak (const char *path);
int Pak_OpenFile (const char *path);
int Pak_GetFileLen (const char *path);
int Pak_ReadFile (const char *path, int len, void *buf);
int Pak_GetFileList (const char *dir, const char *extension, char *str, int bufsize );

aboolean Pak_FileExists (const char *file);

#endif /*__PAK_H__*/
