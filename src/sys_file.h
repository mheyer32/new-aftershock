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

#ifndef SYS_FILE_H__
#define SYS_FILE_H__

void *File_Open(const char *path, int mode);
int File_Close(void *file);
int File_GetLen(void *file);
size_t File_Read(void *buf, size_t size, void *stream);
size_t File_Write(const void *buf, size_t size, void *stream);
void *File_FindFirst (const char *path, char *filename, unsigned char musthave, unsigned char canthave);
unsigned char File_FindNext(void *handle, char *filename, unsigned char musthave, unsigned char canthave);
void File_FindClose(void *handle);
void File_FlushBuffers( void *handle );

#endif 



