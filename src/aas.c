

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