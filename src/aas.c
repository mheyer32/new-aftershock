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

#include "a_shared.h"
#include "io.h"
#include "console.h"



#define AAS_ID	(*(int * )"EAAS")
#define AAS_VERSION 5


// TODO !! 

typedef struct 
{
	int id;
	int version ;


}aas_header_t;

// aas files don`t seem to have a lump system (?) 
// since the ints after the version are too big or negative ( correct me if I`m wrong )






int AAS_Load_Map ( char * mapname )
{

	int len,file;
	void * buf ;
	char fname [MAX_APATH ];
	aas_header_t *header;


	COM_StripExtension(mapname,fname);
	strcat(fname,".aas");

	len =FS_OpenFile (fname,& file ,FS_READ );



	if (!len )
	{
		Con_Printf ("Could not find aas file : %s\n" , mapname );
		return 0;
	}


	buf =malloc (len);
	
	FS_Read (buf ,len, file );

	FS_FCloseFile (file );

	
	header=buf; 



	if ( header->id != AAS_ID )
	{
		Con_Printf ("Is not an .aas file : %s \n",fname);
		return 0;
	}

	if (header->version != AAS_VERSION )
	{
		Con_Printf ("Has wrong version : %s \n",fname );
		
	}





	free (buf );


	return 1;



}