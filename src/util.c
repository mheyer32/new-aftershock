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
#include <windows.h>
#include "vec.h"


extern HWND hWnd;
extern HGLRC hRC;
void Error(const char *fmt, ...)
{
	char buf [1024];
    va_list ap;

    va_start(ap, fmt);    
    sprintf(buf, fmt, ap);
    va_end(ap);

		
	awglMakeCurrent( NULL, NULL );
	awglDeleteContext( hRC );
   
	 if (MessageBox(NULL,buf,"ERROR",MB_OK   ) ==MB_OK );
 

    exit(1);
}


int strcmpEXT (char * str,char * str2 ,int len)
{
	int i;
	for (i=0;i<len;i++)
	{
		if (*(str+i)>*(str2+i))
			return 1;
		if (*(str+i)<*(str2+i))
			return -1;

	}
	return 0;
}






int Search (const char * name ,char * in, char * buf ,int bufsize)
{

	int i=0,len=0,ret=0;

	char * actpos=in;
	char * tmppos=NULL;
	char * token=malloc(128);
	char * nextpos=NULL;
	char * bufpos=buf;

	memset(buf,0,bufsize);

	if (strlen(name)<1)
	{
		free(token);
		return 0;
	}

	for (i=0;strstr(actpos,name)!=NULL; )
	{

		memset(token,0,128);
		ret++;
		tmppos=strstr(actpos,name);

		while (*tmppos != ' ')
		{
			tmppos--;


		}
		tmppos++;
		nextpos=tmppos;
		len=0;
		while (*nextpos != ' ')
		{
			len++;
			nextpos++;


		}
		memcpy(token,tmppos,len);
		strcat(bufpos,token);
		bufpos+=strlen(token)+1;
		actpos=nextpos;
		


	}


	free(token);
	return ret;


}


char *copystring(const char *s)
{
	char *b;

	if (!s)
		return (0);
	b = (char *)malloc(strlen(s) + 1);
	strcpy(b, s);

	return (b);
}


