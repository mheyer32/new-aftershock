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
#include "pak.h"
#include "bsp.h"
#include "shader.h"
#include "opengl.h"
#include "io.h"
#include "tex.h"
#include "console.h"
#include "render.h"

#define MAXSHADERS 1024



#define DEFAULT_TEXT_SHADER_NAME "gfx/2d/bigchars"
#define WHITE_SHADER_NAME "white"
#define CONSOLE_SHADER_NAME "console"


typedef struct {
	char * filename ;
	char * textures;
}shaderbind_t;



static byte * shaders_in_use =NULL;
char shaderlist[4096];
int numshaders=0,numshadersloaded=0;
shaderbind_t * shaderbinds=NULL;
extern int *ids;


int shader_white =-1;
int shader_text =-1;
int shader_console =-1;


/* Maps shader keywords to functions */

static int shader_lookup(const char *name);
static void shader_skip(void);
static void shader_read(void);
static void shader_readpass(shader_t *shader, shaderpass_t *pass);
static void shader_parsetok(shader_t *shader, shaderpass_t *pass, shaderkey_t *keys, char *tok);
static void shader_makedefaults(void);
static int shader_gettexref(const char *fname);
static char* nexttok(void);
static char* nextarg(void);
static void Syntax(void);
static void shader_parsefunc(char **args, shaderfunc_t *func);

char *shaderbuf, *curpos, *endpos;
static int *shaderfound;  /* Shader found in shader script files */

/****************** shader keyword functions ************************/

static void
shader_cull(shader_t *shader, shaderpass_t *pass, int numargs, char **args)
{
    if (!strcmp(args[0], "disable") || !strcmp(args[0], "none"))
	shader->flags |= SHADER_NOCULL;
}

static void
shader_surfaceparm(shader_t *shader, shaderpass_t *pass, int numargs,
		   char **args)
{
    if (!strcmp(args[0], "trans"))
	{
	shader->flags |= SHADER_TRANSPARENT;
	shader->contents |=CONTENTS_TRANSLUCENT;
	}
    else if (!strcmp(args[0], "sky"))
	{
	shader->flags |= SHADER_SKY;
	shader->contents |= SURF_SKY;
	}
	else if (!strcmp(args[0], "nomipmaps"))
	{
	shader->contents|=SURF_NOMIPMAP;
	}
	else if (!strcmp(args[0], "nomarks"))
		shader->contents|=SURF_NOMARKS;
	else if (!strcmp(args[0], "nonsolid"))
		shader->contents|=SURF_NONSOLID;
	else if (!strcmp(args[0], "nodraw"))
		shader->contents|=SURF_NODRAW;
	else if (!strcmp(args[0], "nodlight"))
		shader->contents|=SURF_NODLIGHT;
	else if (!strcmp(args[0], "structural"))
		shader->contents|=CONTENTS_STRUCTURAL;
	else if (!strcmp(args[0], "metalsteps"))
		shader->contents|=SURF_METALSTEPS	;
	else if (!strcmp(args[0], "playerclip" ))
		shader->contents|=CONTENTS_PLAYERCLIP;
	else if (!strcmp(args[0], "alphashadow"))
		shader->contents|=SURF_ALPHASHADOW;
	else if (!strcmp(args[0], "nolightmap" ))
		shader->contents|=SURF_NOLIGHTMAP;
	else if (!strcmp(args[0], "noimpact" ))
		shader->contents|=SURF_NOIMPACT;
	else if (!strcmp(args[0] , "lava" ))
		shader->contents|= CONTENTS_LAVA ;
	else if (!strcmp(args[0], "fog" ))
		shader->contents|= CONTENTS_FOG;
	else if (!strcmp(args[0],"nodrop"))
		shader->contents|=CONTENTS_NODROP;
	else if (!strcmp(args[0], "detail"))
		shader->contents|=CONTENTS_DETAIL;
	else if (!strcmp(args[0], "donotenter"))
		shader->contents|=CONTENTS_DONOTENTER;
	else 
	{

		Con_Printf("Unknown surfaceparam : %s\n",args [0]);
	}

}

static void
shader_skyparms(shader_t *shader, shaderpass_t *pass, int numargs,
		char **args)
{
    shader->skyheight = atof(args[1]);
	// TODO : nearbox , farbox;
}

static void
shader_nomipmaps(shader_t *shader, shaderpass_t *pass, int numargs,
		 char **args)
{
    shader->flags |= SHADER_NOMIPMAPS;
}

static void
shader_nopicmip(shader_t *shader, shaderpass_t *pass, int numargs,
		 char **args)
{
    shader->flags |= SHADER_NOPICMIP;
}

static void
shader_deformvertexes(shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
   
	if (!stricmp(args[0], "wave"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_params[0] =atof (args[1]);
		shader_parsefunc(&args[2], &shader->deformv_wavefunc);
		
		shader->deform_vertices=DEFORMV_WAVE;
	}
	else if (!stricmp(args[0],"normal"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices=DEFORMV_NORMAL;
		shader->deform_params[0] =atof (args[1]); // Div
		shader_parsefunc(&args[2], &shader->deformv_wavefunc);
		
	}
	else if (!stricmp(args[0],"bulge"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices=DEFORMV_BULGE;
	
		shader->deform_params[0] = atof (args[1]); // Width 
		shader->deform_params[1] =atof (args[2]); // Height
		shader->deform_params[2] = atof (args[3]); // Speed 
	}
	else if (!stricmp (args[0] , "move"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices=DEFORMV_MOVE;

		shader->deform_params[0] = atof (args[1]); // x 
		shader->deform_params[1] = atof (args[2]); // y
		shader->deform_params[2] = atof (args[3]); // z
		
		shader_parsefunc(&args[4], &shader->deformv_wavefunc);
	}
	else if (!stricmp ( args[0] ,"autosprite"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices= DEFORMV_AUTOSPRITE;

	}
	else if (!stricmp (args[0],"autosprite2"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices= DEFORMV_AUTOSPRITE2;
	}
	else 
	{
		Con_Printf ("WARNING: Unknown deformv param : %s \n",args[0]);
	}


}

static void shader_sort (shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	if( !stricmp( args[0], "portal" ) ) {
			shader->sort = SHADER_SORT_PORTAL;
	} else if( !stricmp( args[0], "sky" ) ) {
		shader->sort = SHADER_SORT_SKY;
	} else if( !stricmp( args[0], "opaque" ) ) {
		shader->sort = SHADER_SORT_OPAQUE;
	} else if( !stricmp( args[0], "banner" ) ) {
		shader->sort = SHADER_SORT_BANNER;
	} else if( !stricmp( args[0], "underwater" ) ) {
		shader->sort = SHADER_SORT_UNDERWATER;
	} else if( !stricmp( args[0], "additive" ) ) {
		shader->sort = SHADER_SORT_ADDITIVE;
	} else if( !stricmp( args[0], "nearest" ) ) {
		shader->sort = SHADER_SORT_NEAREST;
	} else {
		shader->sort = atoi(args[0]);
	}

}

static void shader_q3map (shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	// Just do nothing 


}

static void shader_portal(shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	shader->sort = SHADER_SORT_PORTAL;
}

static void shader_entitymergable (shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	// TODO !

}
static void shader_polygonoffset (shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	shader->flags|=SHADER_POLYGONOFFSET;
}

static shaderkey_t shaderkeys[] =
{
    {"cull", 1, 1, shader_cull},
    {"surfaceparm", 1, 1, shader_surfaceparm},
    {"skyparms", 3, 3, shader_skyparms},
    {"nomipmaps", 0, 0, shader_nomipmaps},
	{"nopicmip",0,0,shader_nopicmip},
	{"polygonoffset",0,0,shader_polygonoffset },
	{"sort",1,1,shader_sort},
    {"deformvertexes", 1, 9, shader_deformvertexes},
	{"q3map_lightimage",0,9,shader_q3map},
	{"q3map_globaltexture",0,9,shader_q3map},
	{"q3map_surfacelight",0,9,shader_q3map},
	{"qer_editorimage",0,9,shader_q3map},
	{"qer_trans",0,9,shader_q3map},
	{"qer_nocarve",0,9,shader_q3map},
	{"q3map_sun",0,9,shader_q3map},
	{"q3map_lightsubdivide",0,9,shader_q3map},
	{"light",0,1,shader_q3map }, // (?)
	{"portal",0,0,shader_portal},
	{"entitymergable",0,0,shader_entitymergable},
    {NULL, 0, 0, NULL}  /* Sentinel */
};

/****************** shader pass keyword functions *******************/

static void
shaderpass_map(shader_t *shader, shaderpass_t *pass, int numargs, char **args)
{
	if (!stricmp(args[0], "$lightmap"))
	{
		pass->tc_gen=TC_GEN_LIGHTMAP;
		pass->flags |= SHADER_LIGHTMAP;

	}
	else if (!stricmp (args[0],"$whiteimage"))
	{
		pass->tc_gen=TC_GEN_BASE;
		pass->texref=complete_texture_load("white",0 |  shader->flags & SURF_NOMIPMAP ? TEXFILE_NOMIPMAPS : 0  );
	}
	else
    {
			pass->tc_gen=TC_GEN_BASE;
			pass->texref=complete_texture_load(args[0],0 |  shader->flags & SURF_NOMIPMAP ? TEXFILE_NOMIPMAPS : 0  | shader->flags & SHADER_NOPICMIP ? TEXFILE_NOSCALEDOWN : 0);
    }
	


}

static void
shaderpass_rgbgen(shader_t *shader, shaderpass_t *pass, int numargs,
		  char **args)
{


	if (!stricmp(args[0] , "identitylighting"))
	{
		pass->rgbgen=RGB_GEN_IDENTITY_LIGHTING;

	}
	else if (!stricmp(args[0] ,"identity"))
	{
		pass->rgbgen=RGB_GEN_IDENTITY;
	}
	else if (!stricmp (args[0],"wave"))
	{
		if (numargs!=6)
			Syntax();

		shader_parsefunc(&args[1], &pass->rgbgen_func);
		pass->rgbgen=RGB_GEN_WAVE;
	}
	else if (!stricmp(args[0],"entity"))
	{
		pass->rgbgen=RGB_GEN_ENTITY;
	}
	else if (!stricmp(args[0],"oneMinusEntity"))
	{
		pass->rgbgen=RGB_GEN_ONE_MINUS_ENTITY;
	}
	else if (!stricmp(args[0],"Vertex"))
	{
		
		pass->rgbgen=RGB_GEN_VERTEX;

	}
	else if (!stricmp(args[0],"oneMinusVertex"))
	{
		pass->rgbgen=RGB_GEN_ONE_MINUS_VERTEX;
	}
	else if (!stricmp(args[0],"lightingDiffuse"))
	{
		pass->rgbgen=RGB_GEN_LIGHTING_DIFFUSE ;

	}
	else {
		Con_Printf ("WARNING : Unknown rgb_gen param : % s \n",args[0]);
	}

}

static void
shaderpass_blendfunc(shader_t *shader, shaderpass_t *pass, int numargs,
		     char **args)
{
    pass->flags |= SHADER_BLEND;
    
    if (numargs == 1)
    {
	if (!strcmp(args[0], "blend"))
	{
	    pass->blendsrc = GL_SRC_ALPHA;
	    pass->blenddst = GL_ONE_MINUS_SRC_ALPHA;
	}
	else if (!strcmp(args[0], "filter"))
	{
	    pass->blendsrc = GL_DST_COLOR;
	    pass->blenddst = GL_ZERO;
	}
	else if (!strcmp(args[0], "add"))
	{
	    pass->blendsrc = pass->blenddst = GL_ONE;
	}
	else
	    Syntax();
    }
    else
    {
	int i;
	uint_t *blend;
	for (i=0; i < 2; ++i)
	{
	    blend = i == 0 ? &pass->blendsrc : &pass->blenddst;
	    if (!strcmp(args[i], "gl_zero"))
		*blend = GL_ZERO;
	    else if (!strcmp(args[i], "gl_one"))
		*blend = GL_ONE;
	    else if (!strcmp(args[i], "gl_dst_color"))
		*blend = GL_DST_COLOR;
	    else if (!strcmp(args[i], "gl_one_minus_src_alpha"))
		*blend = GL_ONE_MINUS_SRC_ALPHA;
	    else if (!strcmp(args[i], "gl_src_alpha"))
		*blend = GL_SRC_ALPHA;
	    else if (!strcmp(args[i], "gl_src_color"))
		*blend = GL_SRC_COLOR;
	    else if (!strcmp(args[i], "gl_one_minus_dst_color"))
		*blend = GL_ONE_MINUS_DST_COLOR;
	    else if (!strcmp(args[i], "gl_one_minus_src_color"))
		*blend = GL_ONE_MINUS_SRC_COLOR;
	    else if (!strcmp(args[i], "gl_dst_alpha"))
		*blend = GL_DST_ALPHA;
	    else if (!strcmp(args[i], "gl_one_minus_dst_alpha"))
		*blend = GL_ONE_MINUS_DST_ALPHA;
	    else
		Syntax();
	}
    }
}

static void
shaderpass_depthfunc(shader_t *shader, shaderpass_t *pass, int numargs,
		     char **args)
{
    if (!strcmp(args[0], "equal"))
	pass->depthfunc = GL_EQUAL;
    else
	Syntax();
}

static void
shaderpass_depthwrite(shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
    /* FIXME: Why oh why is depthwrite enabled in the sky shaders ???? */
    if (shader->flags & SHADER_SKY) return;
    
    shader->flags |= SHADER_DEPTHWRITE;
    pass->flags |= SHADER_DEPTHWRITE;
}

static void
shaderpass_alphafunc(shader_t *shader, shaderpass_t *pass, int numargs,
		     char **args)
{
    pass->flags |= SHADER_ALPHAFUNC;
    
    if (!stricmp(args[0], "gt0"))
    {
	pass->alphafunc = GL_GREATER;
	pass->alphafuncref = 0.0f;
    }
	else if (!stricmp (args[0],"lt128"))
	{
	pass->alphafunc = GL_LESS;
	pass->alphafuncref = 0.5f;

	}
    else if (!stricmp(args[0], "ge128"))
    {
	pass->alphafunc = GL_GEQUAL;
	pass->alphafuncref = 0.5f;
    }
    else
	{
		Con_Printf ("WARNING: Unknown alphafunc param : %s \n",args[0]);

	}
}

static void
shaderpass_tcmod(shader_t *shader, shaderpass_t *pass, int numargs,
		 char **args)
{
    pass->flags |= SHADER_TCMOD;
    
;

	if (pass->num_tc_mod == MAX_TC_MOD )
		Error ("MAX_TC_MOD exceeded !");
	

	if (!stricmp(args[0],"rotate"))
	{
		pass->tc_mod[pass->num_tc_mod].type=SHADER_TCMOD_ROTATE;
		pass->tc_mod[pass->num_tc_mod].args[0] = atof(args[1]);
	}
	else if (!stricmp (args[0],"scale"))
	{
		if (numargs != 3) Syntax();

		pass->tc_mod[pass->num_tc_mod].type=SHADER_TCMOD_SCALE;
		pass->tc_mod[pass->num_tc_mod].args[0]=atof (args[1]);
		pass->tc_mod[pass->num_tc_mod].args[1]=atof (args[2]);
	
	}
	else if (!stricmp(args[0],"scroll"))
	{

		if (numargs != 3) Syntax();

		pass->tc_mod[pass->num_tc_mod].type =SHADER_TCMOD_SCROLL;
		pass->tc_mod[pass->num_tc_mod].args[0]=atof (args[1]);
		pass->tc_mod[pass->num_tc_mod].args[1]=atof (args[2]);

	}
	else if (!stricmp (args[0],"stretch"))
	{
		if (numargs != 6) Syntax();
		pass->tc_mod[pass->num_tc_mod].type =SHADER_TCMOD_STRETCH;
		shader_parsefunc(&args[1], &pass->tc_mod_stretch);

	}
	else if (!stricmp (args[0],"transform"))
	{
			int i;
		if (numargs != 7) Syntax();
		pass->tc_mod[pass->num_tc_mod].type= SHADER_TCMOD_TRANSFORM;
		for (i=0; i < 6; ++i)
			pass->tc_mod[pass->num_tc_mod].args[i] = atof(args[i+1]);
	}
	else if (!stricmp (args[0],"turb"))
	{
		int i, a1;
		if (numargs == 5)
			a1 = 1;
		else if (numargs == 6)
			a1 = 2;
		else
			Syntax();
		pass->tc_mod[pass->num_tc_mod].type= SHADER_TCMOD_TURB;
	for (i=0; i < 4; ++i)
	    pass->tc_mod[pass->num_tc_mod].args[i] = atof(args[i+a1]);
   
	}
	else 
	{
		Con_Printf ("WARNING: Unknown tc_mod : %s \n",args[0]);
		pass->tc_mod[pass->num_tc_mod].type=-1;
	}

	pass->num_tc_mod++ ;
}

static void
shaderpass_animmap(shader_t *shader, shaderpass_t *pass, int numargs,
		   char **args)
{
    int i;
	pass->tc_gen= TC_GEN_BASE;
    pass->flags |= SHADER_ANIMMAP;
    pass->anim_fps = atof(args[0]);
    pass->anim_numframes = numargs - 1;
    for (i=1; i < numargs; ++i)
	{

		pass->anim_frames[i-1]=complete_texture_load(args[i],0 |  shader->flags & SURF_NOMIPMAP ? TEXFILE_NOMIPMAPS : 0 | shader->flags & SHADER_NOPICMIP ? TEXFILE_NOSCALEDOWN : 0 );
	}
}

static void
shaderpass_clampmap(shader_t *shader, shaderpass_t *pass, int numargs,
		    char **args)
{
	pass->tc_gen= TC_GEN_BASE;
    pass->texref = complete_texture_load(args[0],0| TEXFILE_CLAMP | shader->flags & SURF_NOMIPMAP ? TEXFILE_NOMIPMAPS : 0 | shader->flags & SHADER_NOPICMIP ? TEXFILE_NOSCALEDOWN : 0 );
}

static void
shaderpass_tcgen(shader_t *shader, shaderpass_t *pass, int numargs,
		 char **args)
{
    
	if (!stricmp(args[0],"base"))
	{
		pass->tc_gen=TC_GEN_BASE;
	}
	else if (!stricmp(args[0],"lightmap"))
	{
		pass->tc_gen=TC_GEN_LIGHTMAP;
	}
	else if (!stricmp(args[0],"environment"))
	{
		pass->tc_gen=TC_GEN_ENVIRONMENT ;
	}
	else if (!stricmp(args[0],"vector"))
	{
		pass->tc_gen=TC_GEN_VECTOR ;

		pass->tc_gen_args[0] = atof(args[1]);
		pass->tc_gen_args[1] = atof(args[2]);
		pass->tc_gen_args[2] = atof(args[3]);
		pass->tc_gen_args[3] = atof(args[4]);
		pass->tc_gen_args[4] = atof(args[5]);
		pass->tc_gen_args[5] = atof(args[6]);
	}
	else
	{
		Con_Printf ("unknown tcgenparam : %s \n",args[0]);
	}
}

static void shaderpass_alphagen (shader_t *shader, shaderpass_t *pass, int numargs,
		 char **args)
{

	if (!stricmp (args[0],"portal"))
	{
		pass->alpha_gen=ALPHA_GEN_PORTAL;
	}
	else
	{
		Con_Printf ("unknown alphagen param : %s \n",args[0]);
	}



}

static shaderkey_t shaderpasskeys[] =
{
    {"map", 1, 1, shaderpass_map},
    {"rgbgen", 1, 6, shaderpass_rgbgen},
    {"blendfunc", 1, 2, shaderpass_blendfunc},
    {"depthfunc", 1, 1, shaderpass_depthfunc},
    {"depthwrite", 0, 0, shaderpass_depthwrite},
    {"alphafunc", 1, 1, shaderpass_alphafunc},
    {"tcmod", 2, 7, shaderpass_tcmod},
    {"animmap", 3, SHADER_ARGS_MAX, shaderpass_animmap},
    {"clampmap", 1, 1, shaderpass_clampmap},
    {"tcgen", 1, 10, shaderpass_tcgen},
	{"alphagen",0,10,shaderpass_alphagen},
	
    {NULL, 0, 0, NULL}  /* Sentinel */
};

/* *************************************************************** */


static void  Fill_Shaderbinds(void)
{
	int i=0,len=0;
	char *Listpos=shaderlist;
	char * filebuf=malloc(150000);
    char *tok;


	for(i=0;i<numshaders;i++)
	{
	

		shaderbinds[i].filename=malloc(128);
		shaderbinds[i].textures=malloc(128);
		memset(shaderbinds[i].filename,0,128);
		memset(shaderbinds[i].textures,0,128);
		strcpy(shaderbinds[i].filename,Listpos);

		len = pak_readfile(shaderbinds[i].filename, 150000, filebuf);


		curpos = filebuf;
		endpos = curpos + len;

		strcat(shaderbinds[i].textures," ");
		

		while ((tok = nexttok()) != NULL)
		{
			shaderbinds[i].textures=realloc(shaderbinds[i].textures,strlen(shaderbinds[i].textures)+128);
			strcat(shaderbinds[i].textures,tok);
			strcat(shaderbinds[i].textures," ");
			shader_skip();


		}



		Listpos+=strlen(Listpos)+1;
	}

	free(filebuf);
}

static void load_Standard_Shaders (void)
{

	shader_white = shader_read_extern(WHITE_SHADER_NAME,SHADER_2D);
	shader_text=shader_read_extern (DEFAULT_TEXT_SHADER_NAME,SHADER_2D);
	shader_console =shader_read_extern (CONSOLE_SHADER_NAME,SHADER_2D);

}





void shader_init(void)
{

	r_shaders=malloc(MAXSHADERS*sizeof(shader_t ));
	shaders_in_use =malloc (MAXSHADERS *sizeof(byte));
	memset(shaders_in_use,0,MAXSHADERS);
	

	shaderbuf = (char*)malloc(SHADERBUF_SIZE);

	numshaders=FS_GetFileList ("",".shader",shaderlist,4096);

	if (numshaders==0)
		Error("NO SHADERS FOUND !");


	shaderbinds=malloc(numshaders*sizeof(shaderbind_t));
	Fill_Shaderbinds();
	tex_init();
	load_Standard_Shaders ();	

}


void shader_shutdown (void)
{

	int i;

	if (r_shaders)
		free(r_shaders);

	if (shaders_in_use )
		free (shaders_in_use);

	if (shaderbuf)
		free(shaderbuf);

	if (shaderbinds)
	{
		for (i=0;i<numshaders;i++)
		{

			free(shaderbinds[i].filename);
			free(shaderbinds[i].textures);
		}
		free (shaderbinds);
	}
	tex_shutdown();

}




static char *Find_ShaderFile_forTexture(const char *tex)
{
	int i=0;
	char * tmp=malloc(128);

	strcpy(tmp," ");
	strcat(tmp,tex);
	strcat(tmp," ");



	for (i=0;i<numshaders;i++)
	{
		if (strlen(shaderbinds[i].textures)>1)
		{
			
		if (strstr(shaderbinds[i].textures,tmp))
		{
			free(tmp);
			return shaderbinds[i].filename;
		}
		}



	}


	free(tmp);
	return NULL;



}

static int find_free_Shader(void)
{
	int i=0;


	for (i=0;i<MAXSHADERS;i++)
	{
		if (!shaders_in_use[i])
			return i;


	}

	return -1;


}


// TODO :OPTIMIZE 
/*
void
shader_readall(int numshaders)
{
    int  i;

    Con_Printf("Initializing Shaders\n");
   
	r_lshaders=malloc (numshaders*sizeof (int));


	for (i=0;i<numshaders;i++)
	{

		r_lshaders[i]=shader_read_extern (r_shaderrefs[i].name,SHADER_BSP);
		if (r_lshaders[i]<0) Con_Printf (" Could not load shader %s \n",r_shaderrefs[i].name);

	}
    Con_Printf("done.\n");
}
*/


// Declares the shader as free and frees all texutres 
void shader_free (int num )
{
	int i,j;
	shader_t * s;

	if (num <0 || num > MAXSHADERS)
		return ;

	if (shaders_in_use [num] )
	{
		
		shaders_in_use [num]=0;
		s=&r_shaders[num];
		
		for (i=0;i<s->numpasses;i++)
		{

			if (s->pass[i].flags & SHADER_LIGHTMAP) 
				continue;
			else if (s->pass[i].flags & SHADER_ANIMMAP)
			{
				for (j=0;j<s->pass[i].anim_numframes;j++)
				{
					texture_free(s->pass[i].anim_frames[j]);
				}
			}
			else 
			{
				texture_free (s->pass[i].texref);
			}
  

		}

	}

}

void
shader_freeall(void)
{

	int i;

    for (i=0;i<MAXSHADERS;i++)
	{
		if (shaders_in_use[i])
		{
			shader_free(i);
			
		}
	}
}


static void shader_check_multitexture (shader_t *s )
{
	int i;

	if (s->numpasses==1)
		return;

	if (!r_allowExtensions->integer)
		return ;

	if (glconfig.maxActiveTextures<2 )
		return ;


	if (1)
	{
		for (i=1;i<s->numpasses;i++)
		{

			if (s->pass[i].blendsrc == GL_DST_COLOR && s->pass[i].blenddst== GL_ZERO )
				continue;

			if (s->pass[i].blendsrc == GL_ZERO && s->pass[i].blenddst == GL_SRC_COLOR )
				continue;

			if (glconfig.textureEnvAddAvailable)
				if (s->pass[i].blendsrc== GL_ONE  && s->pass[i].blenddst == GL_ONE )
					continue;


			break;

		}

		if (i== s->numpasses )
		{
			// supported !
			s->flags |= SHADER_MULTITEXTURE ;
		}

	}
	else 
	{
		// testing for TEX_ENV_COMBINE should be placed here :
	}




}

int 
shader_read_extern(const char *name, int type)
{
    int id,len=0,i;
    char *tok;
	char * Filename=NULL,*filebuf=NULL;
	int found=0;
	char *shadername=malloc(128);

	strcpy(shadername,name);


	for (i=0;i<numshadersloaded;i++)
	{
		if (!strcmp(r_shaders[i].name,name))
			return i;

	}

	id =find_free_Shader();
	if (id<0) 
	{
		Error (" shader overflow ! " );


	}

	Filename=Find_ShaderFile_forTexture(name);

	if (!Filename) 
	{
	
		goto failed;
	}

	filebuf =malloc(500000);

	pak_open(Filename);
	len=pak_getlen();
	pak_read(filebuf,len,1);


	curpos = filebuf;
	endpos = curpos + len;


	/* Set defaults */
	r_shaders[id].flags = 0;
	r_shaders[id].numpasses = 0;
	r_shaders[id].contents=CONTENTS_SOLID;
	r_shaders[id].sort =0;
	r_shaders[id].deform_vertices=DEFORMV_NONE;
	r_shaders[id].portalstate= 0;



    while ((tok = nexttok()) != NULL  && curpos<endpos)
    {
	
	
	if (stricmp(tok,shadername))
	{
	    shader_skip();
	    continue;
	}

	found=1;
	
	
	/* Opening brace */
	tok = nexttok();
	if (tok[0] != '{') Syntax();

	while ((tok = nexttok()) != NULL)
	{
	    if (tok[0] == '{') /* Start new pass */
	    {
		int pass = r_shaders[id].numpasses++;
		shader_readpass(&r_shaders[id], &r_shaders[id].pass[pass]);
	    }

	    else if (tok[0] == '}') /* End of shader */
		break;

	    else
		shader_parsetok(&r_shaders[id], NULL, shaderkeys, tok);
	}
	 shader_check_multitexture(&r_shaders[id]);
		
	 if (!r_shaders[id].sort)
	 {
		 if (r_shaders[id].pass[0].flags & SHADER_BLEND )
			 r_shaders[id].sort= SHADER_SORT_ADDITIVE ;
		 else 
			r_shaders[id].sort = SHADER_SORT_OPAQUE ;
	 }


    }



	if (!found)
	{

failed:


	r_shaders[id].flags =0;
	switch ( type )
	{

	case SHADER_2D :
		

		r_shaders[id].flags = SHADER_NOCULL | SHADER_NEEDCOLOURS | SHADER_NOPICMIP;
	    r_shaders[id].numpasses = 1;
	    r_shaders[id].pass[0].flags =  SHADER_BLEND ;
		r_shaders[id].pass[0].blendsrc=GL_SRC_ALPHA;
		r_shaders[id].pass[0].blenddst=GL_ONE_MINUS_SRC_ALPHA;
	    r_shaders[id].pass[0].texref = complete_texture_load (name ,TEXFILE_NOMIPMAPS | TEXFILE_NOSCALEDOWN);
	    r_shaders[id].pass[0].depthfunc = GL_ALWAYS;
	    r_shaders[id].pass[0].rgbgen = RGB_GEN_VERTEX;
		r_shaders[id].sort = SHADER_SORT_ADDITIVE;
		
		break;



	case SHADER_BSP:
		 r_shaders[id].flags = SHADER_NOCULL | SHADER_NEEDCOLOURS;
	    r_shaders[id].numpasses = 1;
	    r_shaders[id].pass[0].flags = SHADER_DEPTHWRITE;
	    r_shaders[id].pass[0].texref =complete_texture_load (name ,0); 
	    r_shaders[id].pass[0].depthfunc = GL_LEQUAL;
	    r_shaders[id].pass[0].rgbgen = RGB_GEN_VERTEX;	 
		r_shaders[id].sort = SHADER_SORT_OPAQUE;


		break;

	case SHADER_MD3:

		r_shaders[id].flags = SHADER_NOCULL ;
	    r_shaders[id].numpasses = 1;
	    r_shaders[id].pass[0].flags = SHADER_DEPTHWRITE;
	    r_shaders[id].pass[0].texref = complete_texture_load (name ,0);
	    r_shaders[id].pass[0].depthfunc = GL_LESS;
	    r_shaders[id].pass[0].rgbgen = RGB_GEN_IDENTITY;
		r_shaders[id].sort = SHADER_SORT_OPAQUE;

		break;



	default:
		free(shadername);
		if (filebuf)
		free(filebuf);
		return -1;
		break;

	}


		strcpy(r_shaders[id].name,name);

	


		free(shadername);
		if (filebuf)
		free(filebuf);


			numshadersloaded++;
		shaders_in_use[id]=1;
		return id;	


	}
	else 
	{

		strcpy(r_shaders[id].name,name);
		numshadersloaded++;
		shaders_in_use[id]=1;
		return id;

	}


	free(filebuf);
	free(shadername);
	return -1;



}



static void
shader_skip(void)
{
    char *tok;
    int brace_count;

    /* Opening brace */
    tok = nexttok();
	if (!tok) return ;
    if (tok[0] != '{') 
	{
		tok = nexttok();
	}

    for (brace_count = 1; brace_count > 0 && curpos < endpos; curpos++)
    {
	if (*curpos == '{')
	    brace_count++;
	else if (*curpos == '}')
	    brace_count--;
    }
}

static void
shader_readpass(shader_t *shader, shaderpass_t *pass)
{
    char *tok;

    /* Set defaults */
    pass->flags = 0;
    pass->texref = -1;
    pass->depthfunc = GL_LEQUAL;
    pass->rgbgen = RGB_GEN_IDENTITY;
    pass->num_tc_mod = 0;
	pass->alpha_gen =	ALPHA_GEN_DEFAULT ;
    
    while ((tok = nexttok()) != NULL)
    {
	if (tok[0] == '}') /* End of pass */
	    break;

	else
	    shader_parsetok(shader, pass, shaderpasskeys, tok);
    }
}

static void
shader_parsetok(shader_t *shader, shaderpass_t *pass, shaderkey_t *keys,
		char *tok)
{
    shaderkey_t *key;
    char *c, *args[SHADER_ARGS_MAX];
    int numargs;

    /* Lowercase the token */
    c = tok;
    while (*c++) *c =  LOWERCASE(*c);
    
    /* FIXME: This should be done with a hash table! */

    for (key = keys; key->keyword != NULL; key++)
    {
	if (stricmp(tok, key->keyword) == 0)
	{
		
	    for (numargs=0; (c = nextarg()) != NULL; numargs++)
	    {
		/* Lowercase the argument */
		args[numargs] = c;
		while (*c) {*c = LOWERCASE(*c); c++;}
	    }
	    if (numargs < key->minargs || numargs > key->maxargs)
		Syntax();
	    
	    if (key->func)
		key->func(shader, pass, numargs, args);
	    return;
	}
    }



	Con_Printf ("Shader_Parsetok : unknown param : % s \n",tok);
    /* Unidentified keyword: no error for now, just advance to end of line */
    while (*curpos != '\n')
	if (++curpos == endpos) break;    
}



static char*
nexttok(void)
{
    char *tok;
    
    while (curpos < endpos)
    {
	/* Skip leading whitespace */
	while (*curpos == ' ' || *curpos == '\t' || *curpos == '\n' ||
	      *curpos == '\r' || *curpos=='\0')
	    if (++curpos == endpos) return NULL;
		

	/* Check for comment */
	if (curpos[0] == '/' && curpos[1] == '/')
	{
	    /* Skip to end of comment line */
	    while (*curpos++ != '\n')
		if (curpos == endpos) return NULL;
	    /* Restart with leading whitespace */
	    continue;
	}
	if ( *curpos=='/' && curpos[1] == '*' ) {
			while ( *curpos && ( *curpos != '*' || curpos[1] != '/' ) ) {
				curpos++;
			}
	}

	/* Seek to end of token */
	tok = curpos;
	while (*curpos != ' ' && *curpos != '\t' && *curpos != '\n' &&
	      *curpos != '\r')
	    if (++curpos == endpos) break;

	/* Zero whitespace character and advance by one */
	*curpos++ = '\0';
	return tok;
    }
    return NULL;
}

static char *
nextarg(void)
{
    char *arg;

    while (curpos < endpos)
    {
	/* Skip leading whitespace */
	while (*curpos == ' ' || *curpos == '\t')
	    if (++curpos == endpos) return NULL;

	/* Check for newline or comment */
	if (*curpos == '\n' || *curpos == '\r' ||
	    (curpos[0] == '/' && curpos[1] == '/'))
	    return NULL;
	
	/* Seek to end of token */
	arg = curpos;
	while (*curpos != ' ' && *curpos != '\t' && *curpos != '\n' &&
	      *curpos != '\r')
	    if (++curpos == endpos) break;

	/* Zero whitespace character and advance by one */
	*curpos++ = '\0';
	return arg;
    }
    return NULL;
}


void
Syntax(void)
{
    Error("Syntax error\n");
}

static void
shader_parsefunc(char **args, shaderfunc_t *func)
{
	if (!strcmp(args[0], "sin"))
	    func->func = SHADER_FUNC_SIN;
	else if (!strcmp(args[0], "triangle"))
	    func->func = SHADER_FUNC_TRIANGLE;
	else if (!strcmp(args[0], "square"))
	    func->func = SHADER_FUNC_SQUARE;
	else if (!strcmp(args[0], "sawtooth"))
	    func->func = SHADER_FUNC_SAWTOOTH;
	else if (!strcmp(args[0], "inversesawtooth"))
	    func->func = SHADER_FUNC_INVERSESAWTOOTH;
	else
	    Syntax();

	func->args[0] = atof(args[1]);
	func->args[1] = atof(args[2]);
	func->args[2] = atof(args[3]);
		func->args[3] = 1;

}



int  R_RegisterShaderNoMip( const char *name ) 
{

	return shader_read_extern(name,SHADER_2D);
	

}

int R_RegisterShader ( const char * name )
{


	return shader_read_extern (name,SHADER_BSP );

}
