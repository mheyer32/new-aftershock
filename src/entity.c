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
#include "util.h"
#include "entity.h"
#include <stdio.h>



static char * entitydata =NULL ;
static char * entitypos =NULL;



void Start_Entity_Parsing (char * entdata , int len  )
{

	if (entdata  && len )
	{
		entitydata =malloc (len );
		memcpy( entitydata , entdata,len );
		entitypos= entitydata;		
	}
	else 
	{
		Error (" Start_Entity_Parsing : NULL ");
	}


}

void End_Entity_Parsing (void )
{

	free (entitydata );

	entitydata=NULL;
	entitypos =NULL;



}



int Get_Entity_Token ( char *buffer, int bufferSize )
{
	
	char *token;
	
	COM_BeginParseSession ();

	while (entitypos)
	{
	token = COM_Parse ( & entitypos );
	
	if (! token [0]) // new Line 
		continue;
			
	A_strncpyz ( buffer , token , bufferSize ); // success
	
	return 1;
		
	}	

	return 0;
	
}