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
#include "cmap.h"
#include "shader.h"
#include "opengl.h"
#include "io.h"
#include "tex.h"
#include "console.h"
#include "render.h"

#define MAXSHADERS 512



#define DEFAULT_TEXT_SHADER_NAME "gfx/2d/bigchars"
#define WHITE_SHADER_NAME "white"
#define CONSOLE_SHADER_NAME "console"

static int numshaders = 0;
int shadercount = 0;
extern int *ids;

int shader_white = -1;
int shader_text = -1;
int shader_console = -1;

typedef struct {
	char name[MAX_APATH];
	int offset;
} cache_t;

cache_t *shadercache = NULL;

/* Maps shader keywords to functions */
static int shader_lookup(const char *name);
void Shader_Skip ( char ** ptr );
void Shader_Parsetok(shader_t *shader, shaderpass_t *pass, shaderkey_t *keys,
		char *token,char ** ptr);
void Shader_MakeCache (void );

static void shader_makedefaults(void);
static int shader_gettexref(const char *fname);
static char *nexttok(void);
static char *nextarg(void);
static void Syntax(void);
static void shader_parsefunc(char **args, shaderfunc_t *func);

char *shaderbuf, *curpos, *endpos;

/****************** shader keyword functions ************************/

static void shader_cull(shader_t *shader, shaderpass_t *pass, int numargs, char **args)
{
	if (!stricmp(args[0], "disable") || !stricmp(args[0], "none")) {
		shader->cull = SHADER_CULL_DISABLE;
	}
	else if (numargs > 1)
	{
		if (!stricmp (args[1], "front"))
			shader->cull = SHADER_CULL_FRONT;
		else if (!stricmp (args[1], "back"))
			shader->cull = SHADER_CULL_BACK;
	}
	else
		shader->cull = SHADER_CULL_FRONT;
}

static void shader_surfaceparm(shader_t *shader, shaderpass_t *pass, int numargs,
		   char **args)
{
    if (!strcmp(args[0], "trans"))
	{
		shader->flags |= SHADER_TRANSPARENT;
//	shader->contents |=CONTENTS_TRANSLUCENT;
	}
    else if (!strcmp(args[0], "sky"))
	{
		shader->flags |= SHADER_SKY;
//	shader->contents |= SURF_SKY;
	}
	else if (!strcmp(args[0], "nomipmaps"))
	{
		shader->flags |= SHADER_NOMIPMAPS;
//	shader->contents|=SURF_NOMIPMAP;
	}
	// Is this needed ??
	/*
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
	*/

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
	shader->flags |= SHADER_NOMIPMAPS;
}

static void
shader_deformvertexes(shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	if (!stricmp(args[0], "wave"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_params[0] = atof (args[1]);
		shader_parsefunc(&args[2], &shader->deformv_wavefunc);
		
		shader->deform_vertices = DEFORMV_WAVE;
	}
	else if (!stricmp(args[0], "normal"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices = DEFORMV_NORMAL;
		shader->deform_params[0] = atof (args[1]); // Div
		shader_parsefunc(&args[2], &shader->deformv_wavefunc);
	}
	else if (!stricmp(args[0], "bulge"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices = DEFORMV_BULGE;
	
		shader->deform_params[0] = atof (args[1]); // Width 
		shader->deform_params[1] = atof (args[2]); // Height
		shader->deform_params[2] = atof (args[3]); // Speed 
	}
	else if (!stricmp (args[0], "move"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices=DEFORMV_MOVE;

		shader->deform_params[0] = atof (args[1]); // x 
		shader->deform_params[1] = atof (args[2]); // y
		shader->deform_params[2] = atof (args[3]); // z
		
		shader_parsefunc(&args[4], &shader->deformv_wavefunc);
	}
	else if (!stricmp (args[0], "autosprite"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices = DEFORMV_AUTOSPRITE;

	}
	else if (!stricmp (args[0], "autosprite2"))
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices = DEFORMV_AUTOSPRITE2;
	}
	else 
	{
		shader->flags |= SHADER_DEFORMVERTS;
		shader->deform_vertices = DEFORMV_NONE;
		Con_Printf ("WARNING: Unknown deformv param: %s\n", args[0]);
	}
}

static void shader_fogparams (shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{

	shader->flags |= SHADER_FOG;

	shader->fog_params[0] = atof(args[0]); // R
	shader->fog_params[1] = atof(args[1]); // G
	shader->fog_params[2] = atof(args[2]); // B
	shader->fog_params[3] = atof(args[3]); // Dist
}

static void shader_sort (shader_t *shader, shaderpass_t *pass, int numargs,
		      char **args)
{
	if(!stricmp( args[0], "portal" ) ) {
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
	// TODO ! (? )

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
	{"q3map_flare",0,9,shader_q3map},
	{"tesssize",0,9,shader_q3map},
	{"qer_editorimage",0,9,shader_q3map},
	{"qer_trans",0,9,shader_q3map},
	{"qer_nocarve",0,9,shader_q3map},
	{"q3map_sun",0,9,shader_q3map},
	{"q3map_lightsubdivide",0,9,shader_q3map},
	{"light",0,1,shader_q3map }, // (?)
	{"portal",0,0,shader_portal},
	{"entitymergable",0,0,shader_entitymergable},
	{"fogparams",4,4,shader_fogparams},
    {NULL, 0, 0, NULL}  /* Sentinel */
};

/****************** shader pass keyword functions *******************/

static void
shaderpass_map(shader_t *shader, shaderpass_t *pass, int numargs, char **args)
{
	if (!stricmp(args[0], "$lightmap"))
	{
		pass->tc_gen = TC_GEN_LIGHTMAP;
		pass->flags |= SHADER_LIGHTMAP;
	}
	else if (!stricmp (args[0], "$whiteimage"))
	{
		pass->tc_gen = TC_GEN_BASE;
		pass->texref = R_Load_Texture("white", shader->flags);
	}
	else
    {
		pass->tc_gen = TC_GEN_BASE;
		pass->texref = R_Load_Texture(args[0], shader->flags);
    }
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
		pass->anim_frames[i-1] = R_Load_Texture(args[i],shader->flags );
	}
}

static void
shaderpass_clampmap(shader_t *shader, shaderpass_t *pass, int numargs,
		    char **args)
{
	pass->tc_gen= TC_GEN_BASE;
    pass->texref = R_Load_Texture(args[0],shader->flags | SHADER_CLAMP);
}

static void
shaderpass_rgbgen(shader_t *shader, shaderpass_t *pass, int numargs,
		  char **args)
{
	if (!stricmp(args[0], "identitylighting"))
	{
		pass->rgbgen = RGB_GEN_IDENTITY_LIGHTING;
	}
	else if (!stricmp(args[0], "identity"))
	{
		pass->rgbgen = RGB_GEN_IDENTITY;
	}
	else if (!stricmp (args[0], "wave"))
	{
		if (numargs != 6)
			Syntax();

		shader_parsefunc(&args[1], &pass->rgbgen_func);
		pass->rgbgen = RGB_GEN_WAVE;
	}
	else if (!stricmp(args[0],"entity"))
	{
		pass->rgbgen = RGB_GEN_ENTITY;
	}
	else if (!stricmp(args[0],"oneMinusEntity"))
	{
		pass->rgbgen = RGB_GEN_ONE_MINUS_ENTITY;
	}
	else if (!stricmp(args[0],"Vertex"))
	{
		pass->rgbgen = RGB_GEN_VERTEX;
	}
	else if (!stricmp(args[0],"oneMinusVertex"))
	{
		pass->rgbgen = RGB_GEN_ONE_MINUS_VERTEX;
	}
	else if (!stricmp(args[0], "lightingDiffuse"))
	{
		pass->rgbgen = RGB_GEN_LIGHTING_DIFFUSE;
	}
	else if (!stricmp (args[0], "exactvertex"))
	{
		pass->rgbgen = RGB_GEN_EXACT_VERTEX;
	}
	else {
		Con_Printf ("WARNING : Unknown rgb_gen param: % s \n",args[0]);
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
    
	if (pass->num_tc_mod == MAX_TC_MOD) {
		Con_Printf ("MAX_TC_MOD exceeded!");
		return;
	}

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
		int i, a1=0;
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

		pass->tc_gen_s[0] = atof(args[1]);
		pass->tc_gen_s[1] = atof(args[2]);
		pass->tc_gen_s[2] = atof(args[3]);
		pass->tc_gen_t[0] = atof(args[4]);
		pass->tc_gen_t[1] = atof(args[5]);
		pass->tc_gen_t[2] = atof(args[6]);
	}
	else
	{
		Con_Printf ("unknown tcgenparam : %s \n",args[0]);
	}
}

static void shaderpass_alphagen (shader_t *shader, shaderpass_t *pass, int numargs,
		 char **args)
{
	if (!stricmp (args[0], "portal"))
	{
		pass->alpha_gen = ALPHA_GEN_PORTAL;
	}
	else if (!stricmp (args[0], "vertex"))
	{
		pass->alpha_gen = ALPHA_GEN_VERTEX;
	}
	else if (!stricmp (args[0], "entity"))
	{
		pass->alpha_gen = ALPHA_GEN_ENTITY;
	}
	else if (!stricmp (args[0], "wave"))
	{
		if (numargs != 6)
			Syntax();

		shader_parsefunc(&args[1], &pass->alphagen_func);

		pass->alpha_gen = ALPHA_GEN_WAVE;
	}
	else if (!stricmp (args[0], "lightingspecular"))
	{
		pass->alpha_gen = ALPHA_GEN_LIGHTINGSPECULAR;
	}
	else
	{
		pass->alpha_gen = ALPHA_GEN_DEFAULT;
		Con_Printf ("Unknown alphagen param: %s\n", args[0]);
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
	{"alphagen", 0, 10, shaderpass_alphagen},
	
    {NULL, 0, 0, NULL}  /* Sentinel */
};

/* *************************************************************** */


static void Load_Standard_Shaders (void)
{
	shader_white = R_LoadShader(WHITE_SHADER_NAME, SHADER_2D);
	shader_text = R_LoadShader (DEFAULT_TEXT_SHADER_NAME, SHADER_2D);
	shader_console = R_LoadShader (CONSOLE_SHADER_NAME, SHADER_2D);
}

int Shader_Init (void )
{
	int i,size = 0, dirlen, numdirs;
	char dirlist [256 * MAX_APATH];
	char *dirptr, *pscripts;
	int file;
	char filename[MAX_APATH];

	Con_Printf ("Initializing Shaders:\n");

	r_shaders = (shader_t * )malloc(MAXSHADERS * sizeof(shader_t ));

	numdirs = FS_GetFileList ("scripts", "shader", dirlist, 256 * MAX_APATH);

	if (!numdirs )
		Error ("Could not find any shaders !");

	/* find the size of all shader scripts */
	dirptr = dirlist;
	for (i=0; i<numdirs; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		Com_sprintf(filename, sizeof(filename), "scripts/%s", dirptr);

		size += FS_FileSize(filename) + 1;
	}

	/* space for the terminator */
	size++;

	/* allocate the memory */
	pscripts = shaderbuf = (char * ) malloc (size);

	/* now load all the scripts */
	dirptr = dirlist;
	for (i=0; i<numdirs; i++, dirptr += dirlen+1) {
		dirlen = strlen(dirptr);
		Com_sprintf( filename, sizeof(filename), "scripts/%s", dirptr );

		size = FS_OpenFile( filename, &file, FS_READ );
		if( !file || !size ) continue;

		Con_Printf( "...loading '%s'\n", filename );

		FS_Read( pscripts, size, file );
		FS_FCloseFile( file );

		pscripts += size;

		/* make sure there's a whitespace between two files */
		*(pscripts++) = '\n';
	}

	/* terminate this big string */
	*pscripts = 0;

	Shader_MakeCache();

	Con_Printf ("...Done\n");

	Tex_Init ();

	Load_Standard_Shaders ();	

	return 1;
}

void Shader_MakeCache (void)
{
	char *ptr;
	char *token;
	int i;

	ptr = shaderbuf;

	numshaders = 0;

	COM_BeginParseSession ();

	while (ptr)
	{
		token = COM_ParseExt (&ptr, 1);
		if (!token[0]) continue;
		Shader_Skip (&ptr);
		
		numshaders++;
	}

	// John Clinton: fix memory overflow
	numshaders++;	
	
	shadercache = (cache_t *)malloc(numshaders * sizeof (cache_t));

	ptr = shaderbuf;
	i = 0;

	// John Clinton: added numshaders test
	while (ptr && (i < numshaders))
	{
		token = COM_ParseExt (&ptr, 1);
		shadercache[i].offset = ptr - shaderbuf;

		if (!token[0]) continue;

		A_strncpyz (shadercache[i].name, token, MAX_APATH);

		Shader_Skip (&ptr);	
		i++;
	}
}

void Shader_Skip (char **ptr)
{	
	char *tok,*tmp;
    int brace_count;

    // Opening brace
    tok = COM_ParseExt (ptr,1);
	
	if (!ptr) return ;
    
	if (tok[0] != '{') 
	{
		tok = COM_ParseExt(ptr,1);
	}

	tmp = *ptr;

    for (brace_count = 1; brace_count > 0 ; tmp++)
    {
		if (!tmp[0]) break ;
		if (tmp[0] == '{')
			brace_count++;
		else if (tmp[0] == '}')
			brace_count--;
    }

	*ptr = tmp;
}

int Shader_GetOffset (const char *name )
{
	int i;

	// HASHTABLE !!!
	for (i = 0; i < numshaders; i++)
	{
		if (!stricmp (shadercache[i].name, name))
			return shadercache[i].offset;
	}

	return -1;
}


int Shader_Shutdown (void)
{
	free (r_shaders);
	free (shaderbuf);

	free (shadercache);

	Tex_Shutdown();

	return 0;
}

void Shader_Readpass(shader_t *shader, shaderpass_t *pass, char ** ptr)
{
    char *token;

    // Set defaults
    pass->flags = 0;
    pass->texref = -1;
	pass->anim_numframes = 0;
    pass->depthfunc = GL_LEQUAL;
    pass->rgbgen = RGB_GEN_NONE;
    pass->num_tc_mod = 0;
	pass->alpha_gen =ALPHA_GEN_DEFAULT;
	pass->tc_gen = TC_GEN_BASE;
	pass->tc_mod[0].type = SHADER_TCMOD_NONE;
	pass->num_tc_mod = 0;
	
	while (ptr)
	{
		token = COM_ParseExt (ptr, 1);
		
		if (!token[0]) continue;

		if (token[0] == '}')
			break;
		else
			Shader_Parsetok (shader, pass, shaderpasskeys,token,ptr);
	}

	// Check some things 
	if (pass->rgbgen == RGB_GEN_NONE) {
		pass->rgbgen = RGB_GEN_IDENTITY;
	}
}

void Shader_Parsetok(shader_t *shader, shaderpass_t *pass, shaderkey_t *keys, char *token, char **ptr)
{
    shaderkey_t *key;
    char *c, *args[SHADER_ARGS_MAX];
	static char buf[SHADER_ARGS_MAX][128];
    int numargs;

	for (key = keys; key->keyword != NULL; key++)
	{
		if (!stricmp (token, key->keyword))
		{
			numargs = 0;

			while (ptr)
			{
				c = COM_Parse (ptr);
				
				if (!c[0]) // NEW LINE 
					break;

				c = strlwr(c); // Lowercase ( FIXME !)
				
				strcpy(buf[numargs], c);
				args[numargs] = buf[numargs++];
			}

			if (numargs < key->minargs || numargs > key->maxargs)
				Syntax();

			if (key->func)
				key->func(shader, pass, numargs, args);
				return;
		}
	}

	// we could not find the keyword
	Con_Printf ("Shader_Parsetok: Unknown keyword: %s\n", token);
   
	// Next Line
	while (ptr)
	{
		token = COM_Parse (ptr);
		if (!token[0])
			break;
	}
}

void Shader_Finish (shader_t *s)
{
	// check if we can use Render_Backend_Flush_Multitexture_Lightmapped
	if (r_allowExtensions->integer && gl_ext_info._ARB_Multitexture)
	{
		if (s->numpasses == 2)
		{
			if (!(s->pass[0].flags & SHADER_BLEND) && s->pass[1].flags & SHADER_BLEND)
			{
				if (s->pass[0].flags & SHADER_LIGHTMAP && ((s->pass[1].blendsrc == GL_DST_COLOR && s->pass[1].blenddst == GL_ZERO) || 
					(s->pass[1].blendsrc == GL_ZERO && s->pass[1].blenddst == GL_SRC_COLOR)))
				{
					if (! (s->pass[1].flags & SHADER_LIGHTMAP ))
						s->flush = SHADER_FLUSH_MULTITEXTURE_LIGHTMAP;
				}
			}
		}
	}

	// TODO: check if we can use Render_Backend_Flush_Multitexture_Combine

	// FIXME: is this how we handle transparent?
	if (! (s->flags & SHADER_DEPTHWRITE) &&
	! (s->flags & SHADER_TRANSPARENT) &&
	! (s->flags & SHADER_SKY) && (s->numpasses > 0))
	{
		int i;
		
		for (i = 0; i < s->numpasses; i++)
			if (!(s->pass[i].flags & SHADER_BLEND)) {
				s->pass[0].flags |= SHADER_DEPTHWRITE;
				break;
			}
	}

	s->sortkey = (s->flags & SHADER_POLYGONOFFSET) + (s->cull << 1 ) + ( s->flush << 4 );
}

int R_LoadShader (const char *name, int type)
{
	char *ptr;
	int offset, i;
	shader_t *s = &r_shaders[shadercount];

	if (strlen (name) > MAX_APATH)
		return -1;

	// Test if already loaded
	for (i = 0; i < shadercount; i++)
	{
		if (!strcmp (name, r_shaders[i].name))
			return i;
	}

	offset = Shader_GetOffset(name);

	// the shader is in the shader scripts
	if (offset > -1)
	{
		char *token;
		ptr = shaderbuf + offset;

		// set defaults
		s->flags = 0;
		s->sort = 0;
		s->numpasses = 0;
		s->deform_vertices = DEFORMV_NONE;
		s->skyheight = 512.0f;
		s->flush = SHADER_FLUSH_GENERIC;
		s->cull = SHADER_CULL_DISABLE;

		token = COM_ParseExt(&ptr, 1);

		if (!ptr || token[0] != '{')
			return -1;

		while (ptr)
		{
			token = COM_ParseExt (&ptr, 1);

			if (!token[0])
				continue;

			if (token[0] == '{')
			{
				int pass = s->numpasses++;
				Shader_Readpass(s, &s->pass[pass],&ptr);
			}
			else if (token[0] == '}')
			{
				break;
			}
			else
			{
				Shader_Parsetok(s, NULL, shaderkeys, token,&ptr);
			}
		}

		Shader_Finish(s);
	}
	else		// make a default shader
	{
		switch (type)
		{
		case SHADER_2D:
			s->flags = SHADER_NOPICMIP;
			s->numpasses = 1;
			s->pass[0].flags = SHADER_BLEND;
			s->pass[0].blendsrc = GL_SRC_ALPHA;
			s->pass[0].blenddst = GL_ONE_MINUS_SRC_ALPHA;
			s->pass[0].texref = R_Load_Texture (name, SHADER_NOMIPMAPS | SHADER_NOPICMIP);
			s->pass[0].depthfunc = GL_ALWAYS;
			s->pass[0].rgbgen = RGB_GEN_VERTEX;
			s->sort = SHADER_SORT_ADDITIVE;
			s->deform_vertices = DEFORMV_NONE;
			s->flush = SHADER_FLUSH_GENERIC;
			s->cull = SHADER_CULL_DISABLE;
			break;

		case SHADER_BSP:
			s->flags = 0;
			s->numpasses = 1;
			s->pass[0].flags = SHADER_DEPTHWRITE;
			s->pass[0].texref = R_Load_Texture (name, 0); 
	        s->pass[0].depthfunc = GL_LEQUAL;
			s->pass[0].rgbgen = RGB_GEN_VERTEX;	 
		    s->sort = SHADER_SORT_OPAQUE;
			s->deform_vertices = DEFORMV_NONE;
			s->flush = SHADER_FLUSH_GENERIC;
			s->cull = SHADER_CULL_FRONT;
			break;

		case SHADER_MD3:
			s->flags = 0;
			s->numpasses = 1;
			s->pass[0].flags = SHADER_DEPTHWRITE;
			s->pass[0].texref = R_Load_Texture (name, 0);
			s->pass[0].depthfunc = GL_LESS;
			s->pass[0].rgbgen = RGB_GEN_IDENTITY;
			s->sort = SHADER_SORT_OPAQUE;
			s->deform_vertices = DEFORMV_NONE;
			s->flush = SHADER_FLUSH_GENERIC;
			s->cull = SHADER_CULL_FRONT;
			break;

		default:
			return -1;
		}
	}

	strcpy (s->name, name);

	return shadercount++;
}

void Syntax(void)
{
    Con_Printf("Syntax error\n");
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
	return R_LoadShader(name,SHADER_2D);
}

int R_RegisterShader ( const char * name )
{
	return R_LoadShader (name,SHADER_BSP );
}
