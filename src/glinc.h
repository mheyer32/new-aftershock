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
#ifndef __GLINC_H__
#define __GLINC_H__
/*
#ifdef WIN32
#  ifndef APIENTRY
#   if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#    define APIENTRY    __stdcall
#   else
#    define APIENTRY
#   endif
#  endif
#  ifndef WINGDIAPI
#   define WINGDIAPI __declspec(dllimport)
#  endif
#  ifndef CALLBACK
#   if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#    define CALLBACK __stdcall
#   else
#    define CALLBACK
#   endif
#  endif
#endif
*/

#include <windows.h>

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;


#define sizei int 


/* Version */
#define GL_VERSION_1_1                    1

/* AccumOp */
#define GL_ACCUM                          0x0100
#define GL_LOAD                           0x0101
#define GL_RETURN                         0x0102
#define GL_MULT                           0x0103
#define GL_ADD                            0x0104

/* AlphaFunction */
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207

/* AttribMask */
#define GL_CURRENT_BIT                    0x00000001
#define GL_POINT_BIT                      0x00000002
#define GL_LINE_BIT                       0x00000004
#define GL_POLYGON_BIT                    0x00000008
#define GL_POLYGON_STIPPLE_BIT            0x00000010
#define GL_PIXEL_MODE_BIT                 0x00000020
#define GL_LIGHTING_BIT                   0x00000040
#define GL_FOG_BIT                        0x00000080
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_ACCUM_BUFFER_BIT               0x00000200
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_VIEWPORT_BIT                   0x00000800
#define GL_TRANSFORM_BIT                  0x00001000
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_HINT_BIT                       0x00008000
#define GL_EVAL_BIT                       0x00010000
#define GL_LIST_BIT                       0x00020000
#define GL_TEXTURE_BIT                    0x00040000
#define GL_SCISSOR_BIT                    0x00080000
#define GL_ALL_ATTRIB_BITS                0x000fffff

/* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_QUAD_STRIP                     0x0008
#define GL_POLYGON                        0x0009

/* BlendingFactorDest */
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305

/* BlendingFactorSrc */
/*      GL_ZERO */
/*      GL_ONE */
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308


/* Boolean */
#define GL_TRUE                           1
#define GL_FALSE                          0



/* ClipPlaneName */
#define GL_CLIP_PLANE0                    0x3000
#define GL_CLIP_PLANE1                    0x3001
#define GL_CLIP_PLANE2                    0x3002
#define GL_CLIP_PLANE3                    0x3003
#define GL_CLIP_PLANE4                    0x3004
#define GL_CLIP_PLANE5                    0x3005

/* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_2_BYTES                        0x1407
#define GL_3_BYTES                        0x1408
#define GL_4_BYTES                        0x1409
#define GL_DOUBLE                         0x140A


/* DrawBufferMode */
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_AUX0                           0x0409
#define GL_AUX1                           0x040A
#define GL_AUX2                           0x040B
#define GL_AUX3                           0x040C

/* ErrorCode */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505

/* FeedBackMode */
#define GL_2D                             0x0600
#define GL_3D                             0x0601
#define GL_3D_COLOR                       0x0602
#define GL_3D_COLOR_TEXTURE               0x0603
#define GL_4D_COLOR_TEXTURE               0x0604

/* FeedBackToken */
#define GL_PASS_THROUGH_TOKEN             0x0700
#define GL_POINT_TOKEN                    0x0701
#define GL_LINE_TOKEN                     0x0702
#define GL_POLYGON_TOKEN                  0x0703
#define GL_BITMAP_TOKEN                   0x0704
#define GL_DRAW_PIXEL_TOKEN               0x0705
#define GL_COPY_PIXEL_TOKEN               0x0706
#define GL_LINE_RESET_TOKEN               0x0707

/* FogMode */
/*      GL_LINEAR */
#define GL_EXP                            0x0800
#define GL_EXP2                           0x0801


/* FogParameter */
/*      GL_FOG_COLOR */
/*      GL_FOG_DENSITY */
/*      GL_FOG_END */
/*      GL_FOG_INDEX */
/*      GL_FOG_MODE */
/*      GL_FOG_START */

/* FrontFaceDirection */
#define GL_CW                             0x0900
#define GL_CCW                            0x0901

/* GetMapTarget */
#define GL_COEFF                          0x0A00
#define GL_ORDER                          0x0A01
#define GL_DOMAIN                         0x0A02

/* GetPixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* GetPointerTarget */
/*      GL_VERTEX_ARRAY_POINTER */
/*      GL_NORMAL_ARRAY_POINTER */
/*      GL_COLOR_ARRAY_POINTER */
/*      GL_INDEX_ARRAY_POINTER */
/*      GL_TEXTURE_COORD_ARRAY_POINTER */
/*      GL_EDGE_FLAG_ARRAY_POINTER */

/* GetTarget */
#define GL_CURRENT_COLOR                  0x0B00
#define GL_CURRENT_INDEX                  0x0B01
#define GL_CURRENT_NORMAL                 0x0B02
#define GL_CURRENT_TEXTURE_COORDS         0x0B03
#define GL_CURRENT_RASTER_COLOR           0x0B04
#define GL_CURRENT_RASTER_INDEX           0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS  0x0B06
#define GL_CURRENT_RASTER_POSITION        0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID  0x0B08
#define GL_CURRENT_RASTER_DISTANCE        0x0B09
#define GL_POINT_SMOOTH                   0x0B10
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_LINE_STIPPLE                   0x0B24
#define GL_LINE_STIPPLE_PATTERN           0x0B25
#define GL_LINE_STIPPLE_REPEAT            0x0B26
#define GL_LIST_MODE                      0x0B30
#define GL_MAX_LIST_NESTING               0x0B31
#define GL_LIST_BASE                      0x0B32
#define GL_LIST_INDEX                     0x0B33
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_POLYGON_STIPPLE                0x0B42
#define GL_EDGE_FLAG                      0x0B43
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_LIGHTING                       0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER       0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE           0x0B52
#define GL_LIGHT_MODEL_AMBIENT            0x0B53
#define GL_SHADE_MODEL                    0x0B54
#define GL_COLOR_MATERIAL_FACE            0x0B55
#define GL_COLOR_MATERIAL_PARAMETER       0x0B56
#define GL_COLOR_MATERIAL                 0x0B57
#define GL_FOG                            0x0B60
#define GL_FOG_INDEX                      0x0B61
#define GL_FOG_DENSITY                    0x0B62
#define GL_FOG_START                      0x0B63
#define GL_FOG_END                        0x0B64
#define GL_FOG_MODE                       0x0B65
#define GL_FOG_COLOR                      0x0B66
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_ACCUM_CLEAR_VALUE              0x0B80
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_MATRIX_MODE                    0x0BA0
#define GL_NORMALIZE                      0x0BA1
#define GL_VIEWPORT                       0x0BA2
#define GL_MODELVIEW_STACK_DEPTH          0x0BA3
#define GL_PROJECTION_STACK_DEPTH         0x0BA4
#define GL_TEXTURE_STACK_DEPTH            0x0BA5
#define GL_MODELVIEW_MATRIX               0x0BA6
#define GL_PROJECTION_MATRIX              0x0BA7
#define GL_TEXTURE_MATRIX                 0x0BA8
#define GL_ATTRIB_STACK_DEPTH             0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH      0x0BB1
#define GL_ALPHA_TEST                     0x0BC0
#define GL_ALPHA_TEST_FUNC                0x0BC1
#define GL_ALPHA_TEST_REF                 0x0BC2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_INDEX_LOGIC_OP                 0x0BF1
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_AUX_BUFFERS                    0x0C00
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_INDEX_CLEAR_VALUE              0x0C20
#define GL_INDEX_WRITEMASK                0x0C21
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_INDEX_MODE                     0x0C30
#define GL_RGBA_MODE                      0x0C31
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_RENDER_MODE                    0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#define GL_POINT_SMOOTH_HINT              0x0C51
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_FOG_HINT                       0x0C54
#define GL_TEXTURE_GEN_S                  0x0C60
#define GL_TEXTURE_GEN_T                  0x0C61
#define GL_TEXTURE_GEN_R                  0x0C62
#define GL_TEXTURE_GEN_Q                  0x0C63
#define GL_PIXEL_MAP_I_TO_I               0x0C70
#define GL_PIXEL_MAP_S_TO_S               0x0C71
#define GL_PIXEL_MAP_I_TO_R               0x0C72
#define GL_PIXEL_MAP_I_TO_G               0x0C73
#define GL_PIXEL_MAP_I_TO_B               0x0C74
#define GL_PIXEL_MAP_I_TO_A               0x0C75
#define GL_PIXEL_MAP_R_TO_R               0x0C76
#define GL_PIXEL_MAP_G_TO_G               0x0C77
#define GL_PIXEL_MAP_B_TO_B               0x0C78
#define GL_PIXEL_MAP_A_TO_A               0x0C79
#define GL_PIXEL_MAP_I_TO_I_SIZE          0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE          0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE          0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE          0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE          0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE          0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE          0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE          0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE          0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE          0x0CB9
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAP_COLOR                      0x0D10
#define GL_MAP_STENCIL                    0x0D11
#define GL_INDEX_SHIFT                    0x0D12
#define GL_INDEX_OFFSET                   0x0D13
#define GL_RED_SCALE                      0x0D14
#define GL_RED_BIAS                       0x0D15
#define GL_ZOOM_X                         0x0D16
#define GL_ZOOM_Y                         0x0D17
#define GL_GREEN_SCALE                    0x0D18
#define GL_GREEN_BIAS                     0x0D19
#define GL_BLUE_SCALE                     0x0D1A
#define GL_BLUE_BIAS                      0x0D1B
#define GL_ALPHA_SCALE                    0x0D1C
#define GL_ALPHA_BIAS                     0x0D1D
#define GL_DEPTH_SCALE                    0x0D1E
#define GL_DEPTH_BIAS                     0x0D1F
#define GL_MAX_EVAL_ORDER                 0x0D30
#define GL_MAX_LIGHTS                     0x0D31
#define GL_MAX_CLIP_PLANES                0x0D32
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_PIXEL_MAP_TABLE            0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH         0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH      0x0D36
#define GL_MAX_NAME_STACK_DEPTH           0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH     0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH        0x0D39
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH  0x0D3B
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_INDEX_BITS                     0x0D51
#define GL_RED_BITS                       0x0D52
#define GL_GREEN_BITS                     0x0D53
#define GL_BLUE_BITS                      0x0D54
#define GL_ALPHA_BITS                     0x0D55
#define GL_DEPTH_BITS                     0x0D56
#define GL_STENCIL_BITS                   0x0D57
#define GL_ACCUM_RED_BITS                 0x0D58
#define GL_ACCUM_GREEN_BITS               0x0D59
#define GL_ACCUM_BLUE_BITS                0x0D5A
#define GL_ACCUM_ALPHA_BITS               0x0D5B
#define GL_NAME_STACK_DEPTH               0x0D70
#define GL_AUTO_NORMAL                    0x0D80
#define GL_MAP1_COLOR_4                   0x0D90
#define GL_MAP1_INDEX                     0x0D91
#define GL_MAP1_NORMAL                    0x0D92
#define GL_MAP1_TEXTURE_COORD_1           0x0D93
#define GL_MAP1_TEXTURE_COORD_2           0x0D94
#define GL_MAP1_TEXTURE_COORD_3           0x0D95
#define GL_MAP1_TEXTURE_COORD_4           0x0D96
#define GL_MAP1_VERTEX_3                  0x0D97
#define GL_MAP1_VERTEX_4                  0x0D98
#define GL_MAP2_COLOR_4                   0x0DB0
#define GL_MAP2_INDEX                     0x0DB1
#define GL_MAP2_NORMAL                    0x0DB2
#define GL_MAP2_TEXTURE_COORD_1           0x0DB3
#define GL_MAP2_TEXTURE_COORD_2           0x0DB4
#define GL_MAP2_TEXTURE_COORD_3           0x0DB5
#define GL_MAP2_TEXTURE_COORD_4           0x0DB6
#define GL_MAP2_VERTEX_3                  0x0DB7
#define GL_MAP2_VERTEX_4                  0x0DB8
#define GL_MAP1_GRID_DOMAIN               0x0DD0
#define GL_MAP1_GRID_SEGMENTS             0x0DD1
#define GL_MAP2_GRID_DOMAIN               0x0DD2
#define GL_MAP2_GRID_SEGMENTS             0x0DD3
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_FEEDBACK_BUFFER_POINTER        0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE           0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE           0x0DF2
#define GL_SELECTION_BUFFER_POINTER       0x0DF3
#define GL_SELECTION_BUFFER_SIZE          0x0DF4
/*      GL_TEXTURE_BINDING_1D */
/*      GL_TEXTURE_BINDING_2D */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_VERTEX_ARRAY_SIZE */
/*      GL_VERTEX_ARRAY_TYPE */
/*      GL_VERTEX_ARRAY_STRIDE */
/*      GL_NORMAL_ARRAY_TYPE */
/*      GL_NORMAL_ARRAY_STRIDE */
/*      GL_COLOR_ARRAY_SIZE */
/*      GL_COLOR_ARRAY_TYPE */
/*      GL_COLOR_ARRAY_STRIDE */
/*      GL_INDEX_ARRAY_TYPE */
/*      GL_INDEX_ARRAY_STRIDE */
/*      GL_TEXTURE_COORD_ARRAY_SIZE */
/*      GL_TEXTURE_COORD_ARRAY_TYPE */
/*      GL_TEXTURE_COORD_ARRAY_STRIDE */
/*      GL_EDGE_FLAG_ARRAY_STRIDE */
/*      GL_POLYGON_OFFSET_FACTOR */
/*      GL_POLYGON_OFFSET_UNITS */

/* GetTextureParameter */
/*      GL_TEXTURE_MAG_FILTER */
/*      GL_TEXTURE_MIN_FILTER */
/*      GL_TEXTURE_WRAP_S */
/*      GL_TEXTURE_WRAP_T */
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_TEXTURE_BORDER                 0x1005
/*      GL_TEXTURE_RED_SIZE */
/*      GL_TEXTURE_GREEN_SIZE */
/*      GL_TEXTURE_BLUE_SIZE */
/*      GL_TEXTURE_ALPHA_SIZE */
/*      GL_TEXTURE_LUMINANCE_SIZE */
/*      GL_TEXTURE_INTENSITY_SIZE */
/*      GL_TEXTURE_PRIORITY */
/*      GL_TEXTURE_RESIDENT */

/* HintMode */
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102

/* HintTarget */
/*      GL_PERSPECTIVE_CORRECTION_HINT */
/*      GL_POINT_SMOOTH_HINT */
/*      GL_LINE_SMOOTH_HINT */
/*      GL_POLYGON_SMOOTH_HINT */
/*      GL_FOG_HINT */
/*      GL_PHONG_HINT */

/* IndexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* LightModelParameter */
/*      GL_LIGHT_MODEL_AMBIENT */
/*      GL_LIGHT_MODEL_LOCAL_VIEWER */
/*      GL_LIGHT_MODEL_TWO_SIDE */

/* LightName */
#define GL_LIGHT0                         0x4000
#define GL_LIGHT1                         0x4001
#define GL_LIGHT2                         0x4002
#define GL_LIGHT3                         0x4003
#define GL_LIGHT4                         0x4004
#define GL_LIGHT5                         0x4005
#define GL_LIGHT6                         0x4006
#define GL_LIGHT7                         0x4007

/* LightParameter */
#define GL_AMBIENT                        0x1200
#define GL_DIFFUSE                        0x1201
#define GL_SPECULAR                       0x1202
#define GL_POSITION                       0x1203
#define GL_SPOT_DIRECTION                 0x1204
#define GL_SPOT_EXPONENT                  0x1205
#define GL_SPOT_CUTOFF                    0x1206
#define GL_CONSTANT_ATTENUATION           0x1207
#define GL_LINEAR_ATTENUATION             0x1208
#define GL_QUADRATIC_ATTENUATION          0x1209

/* InterleavedArrays */
/*      GL_V2F */
/*      GL_V3F */
/*      GL_C4UB_V2F */
/*      GL_C4UB_V3F */
/*      GL_C3F_V3F */
/*      GL_N3F_V3F */
/*      GL_C4F_N3F_V3F */
/*      GL_T2F_V3F */
/*      GL_T4F_V4F */
/*      GL_T2F_C4UB_V3F */
/*      GL_T2F_C3F_V3F */
/*      GL_T2F_N3F_V3F */
/*      GL_T2F_C4F_N3F_V3F */
/*      GL_T4F_C4F_N3F_V4F */

/* ListMode */
#define GL_COMPILE                        0x1300
#define GL_COMPILE_AND_EXECUTE            0x1301

/* ListNameType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_2_BYTES */
/*      GL_3_BYTES */
/*      GL_4_BYTES */

/* LogicOp */
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F

/* MapTarget */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */

/* MaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* MaterialParameter */
#define GL_EMISSION                       0x1600
#define GL_SHININESS                      0x1601
#define GL_AMBIENT_AND_DIFFUSE            0x1602
#define GL_COLOR_INDEXES                  0x1603
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */

/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701
#define GL_TEXTURE                        0x1702

/* MeshMode1 */
/*      GL_POINT */
/*      GL_LINE */

/* MeshMode2 */
/*      GL_POINT */
/*      GL_LINE */
/*      GL_FILL */

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* PixelCopyType */
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802

/* PixelFormat */
#define GL_COLOR_INDEX                    0x1900
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A

/* PixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* PixelStore */
/*      GL_UNPACK_SWAP_BYTES */
/*      GL_UNPACK_LSB_FIRST */
/*      GL_UNPACK_ROW_LENGTH */
/*      GL_UNPACK_SKIP_ROWS */
/*      GL_UNPACK_SKIP_PIXELS */
/*      GL_UNPACK_ALIGNMENT */
/*      GL_PACK_SWAP_BYTES */
/*      GL_PACK_LSB_FIRST */
/*      GL_PACK_ROW_LENGTH */
/*      GL_PACK_SKIP_ROWS */
/*      GL_PACK_SKIP_PIXELS */
/*      GL_PACK_ALIGNMENT */

/* PixelTransfer */
/*      GL_MAP_COLOR */
/*      GL_MAP_STENCIL */
/*      GL_INDEX_SHIFT */
/*      GL_INDEX_OFFSET */
/*      GL_RED_SCALE */
/*      GL_RED_BIAS */
/*      GL_GREEN_SCALE */
/*      GL_GREEN_BIAS */
/*      GL_BLUE_SCALE */
/*      GL_BLUE_BIAS */
/*      GL_ALPHA_SCALE */
/*      GL_ALPHA_BIAS */
/*      GL_DEPTH_SCALE */
/*      GL_DEPTH_BIAS */

/* PixelType */
#define GL_BITMAP                         0x1A00
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */

/* PolygonMode */
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02

/* ReadBufferMode */
/*      GL_FRONT_LEFT */
/*      GL_FRONT_RIGHT */
/*      GL_BACK_LEFT */
/*      GL_BACK_RIGHT */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_LEFT */
/*      GL_RIGHT */
/*      GL_AUX0 */
/*      GL_AUX1 */
/*      GL_AUX2 */
/*      GL_AUX3 */

/* RenderingMode */
#define GL_RENDER                         0x1C00
#define GL_FEEDBACK                       0x1C01
#define GL_SELECT                         0x1C02

/* ShadingModel */
#define GL_FLAT                           0x1D00
#define GL_SMOOTH                         0x1D01


/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* StencilOp */
/*      GL_ZERO */
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
/*      GL_INVERT */

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

/* TextureCoordName */
#define GL_S                              0x2000
#define GL_T                              0x2001
#define GL_R                              0x2002
#define GL_Q                              0x2003

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* TextureEnvMode */
#define GL_MODULATE                       0x2100
#define GL_DECAL                          0x2101
/*      GL_BLEND */
/*      GL_REPLACE */

/* TextureEnvParameter */
#define GL_TEXTURE_ENV_MODE               0x2200
#define GL_TEXTURE_ENV_COLOR              0x2201

/* TextureEnvTarget */
#define GL_TEXTURE_ENV                    0x2300

/* TextureGenMode */
#define GL_EYE_LINEAR                     0x2400
#define GL_OBJECT_LINEAR                  0x2401
#define GL_SPHERE_MAP                     0x2402

/* TextureGenParameter */
#define GL_TEXTURE_GEN_MODE               0x2500
#define GL_OBJECT_PLANE                   0x2501
#define GL_EYE_PLANE                      0x2502

/* TextureMagFilter */
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703

/* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
/*      GL_TEXTURE_BORDER_COLOR */
/*      GL_TEXTURE_PRIORITY */

/* TextureTarget */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_PROXY_TEXTURE_1D */
/*      GL_PROXY_TEXTURE_2D */

/* TextureWrapMode */
#define GL_CLAMP                          0x2900
#define GL_REPEAT                         0x2901

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* ClientAttribMask */
#define GL_CLIENT_PIXEL_STORE_BIT         0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT        0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS         0xffffffff

/* polygon_offset */
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037

/* texture */
#define GL_ALPHA4                         0x803B
#define GL_ALPHA8                         0x803C
#define GL_ALPHA12                        0x803D
#define GL_ALPHA16                        0x803E
#define GL_LUMINANCE4                     0x803F
#define GL_LUMINANCE8                     0x8040
#define GL_LUMINANCE12                    0x8041
#define GL_LUMINANCE16                    0x8042
#define GL_LUMINANCE4_ALPHA4              0x8043
#define GL_LUMINANCE6_ALPHA2              0x8044
#define GL_LUMINANCE8_ALPHA8              0x8045
#define GL_LUMINANCE12_ALPHA4             0x8046
#define GL_LUMINANCE12_ALPHA12            0x8047
#define GL_LUMINANCE16_ALPHA16            0x8048
#define GL_INTENSITY                      0x8049
#define GL_INTENSITY4                     0x804A
#define GL_INTENSITY8                     0x804B
#define GL_INTENSITY12                    0x804C
#define GL_INTENSITY16                    0x804D
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_TEXTURE_LUMINANCE_SIZE         0x8060
#define GL_TEXTURE_INTENSITY_SIZE         0x8061
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064

/* texture_object */
#define GL_TEXTURE_PRIORITY               0x8066
#define GL_TEXTURE_RESIDENT               0x8067
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069

/* vertex_array */
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_INDEX_ARRAY                    0x8077
#define GL_TEXTURE_COORD_ARRAY            0x8078
#define GL_EDGE_FLAG_ARRAY                0x8079
#define GL_VERTEX_ARRAY_SIZE              0x807A
#define GL_VERTEX_ARRAY_TYPE              0x807B
#define GL_VERTEX_ARRAY_STRIDE            0x807C
#define GL_NORMAL_ARRAY_TYPE              0x807E
#define GL_NORMAL_ARRAY_STRIDE            0x807F
#define GL_COLOR_ARRAY_SIZE               0x8081
#define GL_COLOR_ARRAY_TYPE               0x8082
#define GL_COLOR_ARRAY_STRIDE             0x8083
#define GL_INDEX_ARRAY_TYPE               0x8085
#define GL_INDEX_ARRAY_STRIDE             0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE       0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE       0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE     0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE         0x808C
#define GL_VERTEX_ARRAY_POINTER           0x808E
#define GL_NORMAL_ARRAY_POINTER           0x808F
#define GL_COLOR_ARRAY_POINTER            0x8090
#define GL_INDEX_ARRAY_POINTER            0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER    0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER        0x8093
#define GL_V2F                            0x2A20
#define GL_V3F                            0x2A21
#define GL_C4UB_V2F                       0x2A22
#define GL_C4UB_V3F                       0x2A23
#define GL_C3F_V3F                        0x2A24
#define GL_N3F_V3F                        0x2A25
#define GL_C4F_N3F_V3F                    0x2A26
#define GL_T2F_V3F                        0x2A27
#define GL_T4F_V4F                        0x2A28
#define GL_T2F_C4UB_V3F                   0x2A29
#define GL_T2F_C3F_V3F                    0x2A2A
#define GL_T2F_N3F_V3F                    0x2A2B
#define GL_T2F_C4F_N3F_V3F                0x2A2C
#define GL_T4F_C4F_N3F_V4F                0x2A2D

/* Extensions */
#define GL_EXT_vertex_array               1
#define GL_EXT_bgra                       1
#define GL_EXT_paletted_texture           1
#define GL_WIN_swap_hint                  1
#define GL_WIN_draw_range_elements        1
// #define GL_WIN_phong_shading              1
// #define GL_WIN_specular_fog               1

/* EXT_vertex_array */
#define GL_VERTEX_ARRAY_EXT               0x8074
#define GL_NORMAL_ARRAY_EXT               0x8075
#define GL_COLOR_ARRAY_EXT                0x8076
#define GL_INDEX_ARRAY_EXT                0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT        0x8078
#define GL_EDGE_FLAG_ARRAY_EXT            0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT          0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT          0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT        0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT         0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT          0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT        0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT         0x8080
#define GL_COLOR_ARRAY_SIZE_EXT           0x8081
#define GL_COLOR_ARRAY_TYPE_EXT           0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT         0x8083
#define GL_COLOR_ARRAY_COUNT_EXT          0x8084
#define GL_INDEX_ARRAY_TYPE_EXT           0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT         0x8086
#define GL_INDEX_ARRAY_COUNT_EXT          0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT   0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT   0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT 0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT  0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT     0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT      0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT       0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT       0x808F
#define GL_COLOR_ARRAY_POINTER_EXT        0x8090
#define GL_INDEX_ARRAY_POINTER_EXT        0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT 0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT    0x8093
#define GL_DOUBLE_EXT                     GL_DOUBLE

/* EXT_bgra */
#define GL_BGR_EXT                        0x80E0
#define GL_BGRA_EXT                       0x80E1

/* EXT_paletted_texture */

/* These must match the GL_COLOR_TABLE_*_SGI enumerants */
#define GL_COLOR_TABLE_FORMAT_EXT         0x80D8
#define GL_COLOR_TABLE_WIDTH_EXT          0x80D9
#define GL_COLOR_TABLE_RED_SIZE_EXT       0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_EXT     0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_EXT      0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_EXT     0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_EXT 0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_EXT 0x80DF

#define GL_COLOR_INDEX1_EXT               0x80E2
#define GL_COLOR_INDEX2_EXT               0x80E3
#define GL_COLOR_INDEX4_EXT               0x80E4
#define GL_COLOR_INDEX8_EXT               0x80E5
#define GL_COLOR_INDEX12_EXT              0x80E6
#define GL_COLOR_INDEX16_EXT              0x80E7

/* WIN_draw_range_elements */
#define GL_MAX_ELEMENTS_VERTICES_WIN      0x80E8
#define GL_MAX_ELEMENTS_INDICES_WIN       0x80E9

/* WIN_phong_shading */
#define GL_PHONG_WIN                      0x80EA 
#define GL_PHONG_HINT_WIN                 0x80EB 

/* WIN_specular_fog */
#define GL_FOG_SPECULAR_TEXTURE_WIN       0x80EC

/* For compatibility with OpenGL v1.0 */
#define GL_LOGIC_OP GL_INDEX_LOGIC_OP
#define GL_TEXTURE_COMPONENTS GL_TEXTURE_INTERNAL_FORMAT







extern void (	APIENTRY * glAccum) (GLenum op, GLfloat value);
//extern void (APIENTRY * glAlphaFunc) (GLenum func, GLclampf ref);
extern GLboolean (APIENTRY *glAreTexturesResident) (GLsizei n, const GLuint *textures, GLboolean *residences);
extern void (APIENTRY *glArrayElement )(GLint i);
extern void (APIENTRY *glBegin )(GLenum mode);
// extern void (APIENTRY *glBindTexture )(GLenum target, GLuint texture);
extern void (APIENTRY *glBitmap )(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
//extern void (APIENTRY *glBlendFunc) (GLenum sfactor, GLenum dfactor);
extern void (APIENTRY *glCallList) (GLuint list);
extern void (APIENTRY *glCallLists) (GLsizei n, GLenum type, const GLvoid *lists);
extern void (APIENTRY *glClear) (GLbitfield mask);
extern void (APIENTRY *glClearAccum) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void (APIENTRY *glClearColor) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
extern void (APIENTRY *glClearDepth) (GLclampd depth);
extern void (APIENTRY *glClearIndex) (GLfloat c);
extern void (APIENTRY *glClearStencil) (GLint s);
extern void (APIENTRY *glClipPlane) (GLenum plane, const GLdouble *equation);
extern void (APIENTRY *glColor3b) (GLbyte red, GLbyte green, GLbyte blue);
extern void (APIENTRY *glColor3bv) (const GLbyte *v);
extern void (APIENTRY *glColor3d) (GLdouble red, GLdouble green, GLdouble blue);
extern void (APIENTRY *glColor3dv) (const GLdouble *v);
extern void (APIENTRY *glColor3f) (GLfloat red, GLfloat green, GLfloat blue);
extern void (APIENTRY *glColor3fv) (const GLfloat *v);
extern void (APIENTRY *glColor3i) (GLint red, GLint green, GLint blue);
extern void (APIENTRY *glColor3iv) (const GLint *v);
extern void (APIENTRY *glColor3s) (GLshort red, GLshort green, GLshort blue);
extern void (APIENTRY *glColor3sv) (const GLshort *v);
extern void (APIENTRY *glColor3ub) (GLubyte red, GLubyte green, GLubyte blue);
extern void (APIENTRY *glColor3ubv) (const GLubyte *v);
extern void (APIENTRY *glColor3ui) (GLuint red, GLuint green, GLuint blue);
extern void (APIENTRY *glColor3uiv) (const GLuint *v);
extern void (APIENTRY *glColor3us) (GLushort red, GLushort green, GLushort blue);
extern void (APIENTRY *glColor3usv) (const GLushort *v);
extern void (APIENTRY *glColor4b)(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
extern void (APIENTRY *glColor4bv) (const GLbyte *v);
extern void (APIENTRY *glColor4d) (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
extern void (APIENTRY *glColor4dv) (const GLdouble *v);
extern void (APIENTRY *glColor4f) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void (APIENTRY *glColor4fv) (const GLfloat *v);
extern void (APIENTRY *glColor4i) (GLint red, GLint green, GLint blue, GLint alpha);
extern void (APIENTRY *glColor4iv) (const GLint *v);
extern void (APIENTRY *glColor4s) (GLshort red, GLshort green, GLshort blue, GLshort alpha);
extern void (APIENTRY *glColor4sv) (const GLshort *v);
extern void (APIENTRY *glColor4ub) (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
extern void (APIENTRY *glColor4ubv) (const GLubyte *v);
extern void (APIENTRY *glColor4ui) (GLuint red, GLuint green, GLuint blue, GLuint alpha);
extern void (APIENTRY *glColor4uiv) (const GLuint *v);
extern void (APIENTRY *glColor4us) (GLushort red, GLushort green, GLushort blue, GLushort alpha);
extern void (APIENTRY *glColor4usv) (const GLushort *v);
extern void (APIENTRY *glColorMask) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
extern void (APIENTRY *glColorMaterial) (GLenum face, GLenum mode);
extern void (APIENTRY *glColorPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (APIENTRY *glCopyPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
extern void (APIENTRY *glCopyTexImage1D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
extern void (APIENTRY *glCopyTexImage2D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
extern void (APIENTRY *glCopyTexSubImage1D )(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
extern void (APIENTRY *glCopyTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
extern void (APIENTRY *glCullFace) (GLenum mode);
extern void (APIENTRY *glDeleteLists) (GLuint list, GLsizei range);
extern void (APIENTRY *glDeleteTextures )(GLsizei n, const GLuint *textures);
//extern void (APIENTRY *glDepthFunc )(GLenum func);
//extern void (APIENTRY *glDepthMask )(GLboolean flag);
extern void (APIENTRY *glDepthRange) (GLclampd zNear, GLclampd zFar);
//extern void (APIENTRY *glDisable )(GLenum cap);
//extern void (APIENTRY *glDisableClientState) (GLenum array);
extern void (APIENTRY *glDrawArrays) (GLenum mode, GLint first, GLsizei count);
extern void (APIENTRY *glDrawBuffer) (GLenum mode);
extern void (APIENTRY *glDrawElements) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
extern void (APIENTRY *glDrawPixels) (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
extern void (APIENTRY *glEdgeFlag) (GLboolean flag);
extern void (APIENTRY *glEdgeFlagPointer) (GLsizei stride, const GLvoid *pointer);
extern void (APIENTRY *glEdgeFlagv) (const GLboolean *flag);
extern void (APIENTRY *glEnable) (GLenum cap);
//extern void (APIENTRY *glEnableClientState) (GLenum array);
extern void (APIENTRY *glEnd )(void);
extern void (APIENTRY *glEndList) (void);
extern void (APIENTRY *glEvalCoord1d) (GLdouble u);
extern void (APIENTRY *glEvalCoord1dv) (const GLdouble *u);
extern void (APIENTRY *glEvalCoord1f) (GLfloat u);
extern void (APIENTRY *glEvalCoord1fv) (const GLfloat *u);
extern void (APIENTRY *glEvalCoord2d)(GLdouble u, GLdouble v);
extern void (APIENTRY *glEvalCoord2dv) (const GLdouble *u);
extern void (APIENTRY *glEvalCoord2f) (GLfloat u, GLfloat v);
extern void (APIENTRY *glEvalCoord2fv) (const GLfloat *u);
extern void (APIENTRY *glEvalMesh1) (GLenum mode, GLint i1, GLint i2);
extern void (APIENTRY *glEvalMesh2) (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
extern void (APIENTRY *glEvalPoint1) (GLint i);
extern void (APIENTRY *glEvalPoint2) (GLint i, GLint j);
extern void (APIENTRY *glFeedbackBuffer) (GLsizei size, GLenum type, GLfloat *buffer);
extern void (APIENTRY *glFinish )(void);
extern void (APIENTRY *glFlush )(void);
extern void (APIENTRY *glFogf )(GLenum pname, GLfloat param);
extern void (APIENTRY *glFogfv) (GLenum pname, const GLfloat *params);
extern void (APIENTRY *glFogi )(GLenum pname, GLint param);
extern void (APIENTRY *glFogiv) (GLenum pname, const GLint *params);
extern void (APIENTRY *glFrontFace) (GLenum mode);
extern void (APIENTRY *glFrustum) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
extern GLuint (APIENTRY *glGenLists) (GLsizei range);
extern void (APIENTRY *glGenTextures) (GLsizei n, GLuint *textures);
extern void (APIENTRY *glGetBooleanv) (GLenum pname, GLboolean *params);
extern void (APIENTRY *glGetClipPlane) (GLenum plane, GLdouble *equation);
extern void (APIENTRY *glGetDoublev) (GLenum pname, GLdouble *params);
extern GLenum (APIENTRY *glGetError) (void);
extern void (APIENTRY *glGetFloatv) (GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetIntegerv) (GLenum pname, GLint *params);
extern void (APIENTRY *glGetLightfv) (GLenum light, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetLightiv) (GLenum light, GLenum pname, GLint *params);
extern void (APIENTRY *glGetMapdv) (GLenum target, GLenum query, GLdouble *v);
extern void (APIENTRY *glGetMapfv) (GLenum target, GLenum query, GLfloat *v);
extern void (APIENTRY *glGetMapiv) (GLenum target, GLenum query, GLint *v);
extern void (APIENTRY *glGetMaterialfv) (GLenum face, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetMaterialiv) (GLenum face, GLenum pname, GLint *params);
extern void (APIENTRY *glGetPixelMapfv) (GLenum map, GLfloat *values);
extern void (APIENTRY *glGetPixelMapuiv) (GLenum map, GLuint *values);
extern void (APIENTRY *glGetPixelMapusv) (GLenum map, GLushort *values);
extern void (APIENTRY *glGetPointerv) (GLenum pname, GLvoid* *params);
extern void (APIENTRY *glGetPolygonStipple) (GLubyte *mask);
extern const GLubyte * (APIENTRY *glGetString) (GLenum name);
extern void (APIENTRY *glGetTexEnvfv )(GLenum target, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetTexEnviv )(GLenum target, GLenum pname, GLint *params);
extern void (APIENTRY *glGetTexGendv )(GLenum coord, GLenum pname, GLdouble *params);
extern void (APIENTRY *glGetTexGenfv )(GLenum coord, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetTexGeniv )(GLenum coord, GLenum pname, GLint *params);
extern void (APIENTRY *glGetTexImage )(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
extern void (APIENTRY *glGetTexLevelParameterfv) (GLenum target, GLint level, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetTexLevelParameteriv) (GLenum target, GLint level, GLenum pname, GLint *params);
extern void (APIENTRY *glGetTexParameterfv) (GLenum target, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetTexParameteriv) (GLenum target, GLenum pname, GLint *params);
extern void (APIENTRY *glHint )(GLenum target, GLenum mode);
extern void (APIENTRY *glIndexMask )(GLuint mask);
extern void (APIENTRY *glIndexPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (APIENTRY *glIndexd )(GLdouble c);
extern void (APIENTRY *glIndexdv) (const GLdouble *c);
extern void (APIENTRY *glIndexf) (GLfloat c);
extern void (APIENTRY *glIndexfv) (const GLfloat *c);
extern void (APIENTRY *glIndexi) (GLint c);
extern void (APIENTRY *glIndexiv) (const GLint *c);
extern void (APIENTRY *glIndexs) (GLshort c);
extern void (APIENTRY *glIndexsv) (const GLshort *c);
extern void (APIENTRY *glIndexub) (GLubyte c);
extern void (APIENTRY *glIndexubv) (const GLubyte *c);
extern void (APIENTRY *glInitNames) (void);
extern void (APIENTRY *glInterleavedArrays) (GLenum format, GLsizei stride, const GLvoid *pointer);
extern GLboolean (APIENTRY *glIsEnabled) (GLenum cap);
extern GLboolean (APIENTRY *glIsList) (GLuint list);
extern GLboolean (APIENTRY *glIsTexture )(GLuint texture);
extern void (APIENTRY *glLightModelf) (GLenum pname, GLfloat param);
extern void (APIENTRY *glLightModelfv) (GLenum pname, const GLfloat *params);
extern void (APIENTRY *glLightModeli) (GLenum pname, GLint param);
extern void (APIENTRY *glLightModeliv) (GLenum pname, const GLint *params);
extern void (APIENTRY *glLightf) (GLenum light, GLenum pname, GLfloat param);
extern void (APIENTRY	*glLightfv) (GLenum light, GLenum pname, const GLfloat *params);
extern void (APIENTRY *glLighti) (GLenum light, GLenum pname, GLint param);
extern void (APIENTRY *glLightiv) (GLenum light, GLenum pname, const GLint *params);
extern void (APIENTRY *glLineStipple) (GLint factor, GLushort pattern);
extern void (APIENTRY *glLineWidth) (GLfloat width);
extern void (APIENTRY *glListBase) (GLuint base);
extern void (APIENTRY *glLoadIdentity) (void);
extern void (APIENTRY *glLoadMatrixd)(const GLdouble *m);
extern void (APIENTRY *glLoadMatrixf) (const GLfloat *m);
extern void (APIENTRY *glLoadName) (GLuint name);
extern void (APIENTRY *glLogicOp) (GLenum opcode);
extern void (APIENTRY *glMap1d) (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
extern void (APIENTRY *glMap1f) (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
extern void (APIENTRY *glMap2d) (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
extern void (APIENTRY *glMap2f) (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
extern void (APIENTRY *glMapGrid1d) (GLint un, GLdouble u1, GLdouble u2);
extern void (APIENTRY *glMapGrid1f) (GLint un, GLfloat u1, GLfloat u2);
extern void (APIENTRY *glMapGrid2d) (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
extern void (APIENTRY *glMapGrid2f) (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
extern void (APIENTRY *glMaterialf) (GLenum face, GLenum pname, GLfloat param);
extern void (APIENTRY *glMaterialfv) (GLenum face, GLenum pname, const GLfloat *params);
extern void (APIENTRY *glMateriali) (GLenum face, GLenum pname, GLint param);
extern void (APIENTRY *glMaterialiv) (GLenum face, GLenum pname, const GLint *params);
extern void (APIENTRY *glMatrixMode) (GLenum mode);
extern void (APIENTRY *glMultMatrixd) (const GLdouble *m);
extern void (APIENTRY *glMultMatrixf) (const GLfloat *m);
extern void (APIENTRY *glNewList) (GLuint list, GLenum mode);
extern void (APIENTRY *glNormal3b) (GLbyte nx, GLbyte ny, GLbyte nz);
extern void (APIENTRY *glNormal3bv) (const GLbyte *v);
extern void (APIENTRY *glNormal3d) (GLdouble nx, GLdouble ny, GLdouble nz);
extern void (APIENTRY *glNormal3dv) (const GLdouble *v);
extern void (APIENTRY *glNormal3f) (GLfloat nx, GLfloat ny, GLfloat nz);
extern void (APIENTRY *glNormal3fv) (const GLfloat *v);
extern void (APIENTRY *glNormal3i )(GLint nx, GLint ny, GLint nz);
extern void (APIENTRY *glNormal3iv) (const GLint *v);
extern void (APIENTRY *glNormal3s) (GLshort nx, GLshort ny, GLshort nz);
extern void (APIENTRY *glNormal3sv) (const GLshort *v);
extern void (APIENTRY *glNormalPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (APIENTRY *glOrtho) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
extern void (APIENTRY *glPassThrough )(GLfloat token);
extern void (APIENTRY *glPixelMapfv) (GLenum map, GLsizei mapsize, const GLfloat *values);
extern void (APIENTRY *glPixelMapuiv) (GLenum map, GLsizei mapsize, const GLuint *values);
extern void (APIENTRY *glPixelMapusv) (GLenum map, GLsizei mapsize, const GLushort *values);
extern void (APIENTRY *glPixelStoref) (GLenum pname, GLfloat param);
extern void (APIENTRY *glPixelStorei) (GLenum pname, GLint param);
extern void (APIENTRY *glPixelTransferf) (GLenum pname, GLfloat param);
extern void (APIENTRY *glPixelTransferi) (GLenum pname, GLint param);
extern void (APIENTRY *glPixelZoom) (GLfloat xfactor, GLfloat yfactor);
extern void (APIENTRY *glPointSize) (GLfloat size);
extern void (APIENTRY *glPolygonMode) (GLenum face, GLenum mode);
extern void (APIENTRY *glPolygonOffset) (GLfloat factor, GLfloat units);
extern void (APIENTRY *glPolygonStipple) (const GLubyte *mask);
extern void (APIENTRY *glPopAttrib) (void);
extern void (APIENTRY *glPopClientAttrib) (void);
extern void (APIENTRY *glPopMatrix) (void);
extern void (APIENTRY *glPopName) (void);
extern void (APIENTRY *glPrioritizeTextures) (GLsizei n, const GLuint *textures, const GLclampf *priorities);
extern void (APIENTRY *glPushAttrib) (GLbitfield mask);
extern void (APIENTRY *glPushClientAttrib) (GLbitfield mask);
extern void (APIENTRY *glPushMatrix) (void);
extern void (APIENTRY *glPushName) (GLuint name);
extern void (APIENTRY *glRasterPos2d) (GLdouble x, GLdouble y);
extern void (APIENTRY *glRasterPos2dv) (const GLdouble *v);
extern void (APIENTRY *glRasterPos2f) (GLfloat x, GLfloat y);
extern void (APIENTRY *glRasterPos2fv) (const GLfloat *v);
extern void (APIENTRY *glRasterPos2i) (GLint x, GLint y);
extern void (APIENTRY *glRasterPos2iv) (const GLint *v);
extern void (APIENTRY *glRasterPos2s) (GLshort x, GLshort y);
extern void (APIENTRY *glRasterPos2sv) (const GLshort *v);
extern void (APIENTRY *glRasterPos3d) (GLdouble x, GLdouble y, GLdouble z);
extern void (APIENTRY *glRasterPos3dv) (const GLdouble *v);
extern void (APIENTRY *glRasterPos3f) (GLfloat x, GLfloat y, GLfloat z);
extern void (APIENTRY *glRasterPos3fv) (const GLfloat *v);
extern void (APIENTRY *glRasterPos3i) (GLint x, GLint y, GLint z);
extern void (APIENTRY *glRasterPos3iv) (const GLint *v);
extern void (APIENTRY *glRasterPos3s) (GLshort x, GLshort y, GLshort z);
extern void (APIENTRY *glRasterPos3sv) (const GLshort *v);
extern void (APIENTRY *glRasterPos4d) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
extern void (APIENTRY *glRasterPos4dv) (const GLdouble *v);
extern void (APIENTRY *glRasterPos4f) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
extern void (APIENTRY *glRasterPos4fv) (const GLfloat *v);
extern void (APIENTRY *glRasterPos4i) (GLint x, GLint y, GLint z, GLint w);
extern void (APIENTRY *glRasterPos4iv) (const GLint *v);
extern void (APIENTRY *glRasterPos4s) (GLshort x, GLshort y, GLshort z, GLshort w);
extern void (APIENTRY *glRasterPos4sv) (const GLshort *v);
extern void (APIENTRY *glReadBuffer) (GLenum mode);
extern void (APIENTRY *glReadPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
extern void (APIENTRY *glRectd) (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
extern void (APIENTRY *glRectdv) (const GLdouble *v1, const GLdouble *v2);
extern void (APIENTRY *glRectf) (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
extern void (APIENTRY *glRectfv) (const GLfloat *v1, const GLfloat *v2);
extern void (APIENTRY *glRecti) (GLint x1, GLint y1, GLint x2, GLint y2);
extern void (APIENTRY *glRectiv) (const GLint *v1, const GLint *v2);
extern void (APIENTRY *glRects) (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
extern void (APIENTRY *glRectsv) (const GLshort *v1, const GLshort *v2);
extern GLint (APIENTRY *glRenderMode) (GLenum mode);
extern void (APIENTRY *glRotated) (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
extern void (APIENTRY *glRotatef) (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
extern void (APIENTRY *glScaled) (GLdouble x, GLdouble y, GLdouble z);
extern void (APIENTRY *glScalef) (GLfloat x, GLfloat y, GLfloat z);
extern void (APIENTRY *glScissor) (GLint x, GLint y, GLsizei width, GLsizei height);
extern void (APIENTRY *glSelectBuffer) (GLsizei size, GLuint *buffer);
extern void (APIENTRY *glShadeModel) (GLenum mode);
extern void (APIENTRY *glStencilFunc) (GLenum func, GLint ref, GLuint mask);
extern void (APIENTRY *glStencilMask) (GLuint mask);
extern void (APIENTRY *glStencilOp) (GLenum fail, GLenum zfail, GLenum zpass);
extern void (APIENTRY *glTexCoord1d) (GLdouble s);
extern void (APIENTRY *glTexCoord1dv) (const GLdouble *v);
extern void (APIENTRY *glTexCoord1f) (GLfloat s);
extern void (APIENTRY *glTexCoord1fv) (const GLfloat *v);
extern void (APIENTRY *glTexCoord1i) (GLint s);
extern void (APIENTRY *glTexCoord1iv) (const GLint *v);
extern void (APIENTRY *glTexCoord1s) (GLshort s);
extern void (APIENTRY *glTexCoord1sv) (const GLshort *v);
extern void (APIENTRY *glTexCoord2d) (GLdouble s, GLdouble t);
extern void (APIENTRY *glTexCoord2dv) (const GLdouble *v);
extern void (APIENTRY *glTexCoord2f) (GLfloat s, GLfloat t);
extern void (APIENTRY *glTexCoord2fv) (const GLfloat *v);
extern void (APIENTRY *glTexCoord2i) (GLint s, GLint t);
extern void (APIENTRY *glTexCoord2iv) (const GLint *v);
extern void (APIENTRY *glTexCoord2s) (GLshort s, GLshort t);
extern void (APIENTRY *glTexCoord2sv) (const GLshort *v);
extern void (APIENTRY *glTexCoord3d)(GLdouble s, GLdouble t, GLdouble r);
extern void (APIENTRY *glTexCoord3dv) (const GLdouble *v);
extern void (APIENTRY *glTexCoord3f) (GLfloat s, GLfloat t, GLfloat r);
extern void (APIENTRY *glTexCoord3fv) (const GLfloat *v);
extern void (APIENTRY *glTexCoord3i) (GLint s, GLint t, GLint r);
extern void (APIENTRY *glTexCoord3iv) (const GLint *v);
extern void (APIENTRY *glTexCoord3s) (GLshort s, GLshort t, GLshort r);
extern void (APIENTRY *glTexCoord3sv) (const GLshort *v);
extern void (APIENTRY *glTexCoord4d) (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
extern void (APIENTRY *glTexCoord4dv) (const GLdouble *v);
extern void (APIENTRY *glTexCoord4f) (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
extern void (APIENTRY *glTexCoord4fv) (const GLfloat *v);
extern void (APIENTRY *glTexCoord4i) (GLint s, GLint t, GLint r, GLint q);
extern void (APIENTRY *glTexCoord4iv) (const GLint *v);
extern void (APIENTRY *glTexCoord4s) (GLshort s, GLshort t, GLshort r, GLshort q);
extern void (APIENTRY *glTexCoord4sv) (const GLshort *v);
extern void (APIENTRY *glTexCoordPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
//extern void (APIENTRY *glTexEnvf) (GLenum target, GLenum pname, GLfloat param);
//extern void (APIENTRY *glTexEnvfv) (GLenum target, GLenum pname, const GLfloat *params);
//extern void (APIENTRY *glTexEnvi )(GLenum target, GLenum pname, GLint param);
//extern void (APIENTRY *glTexEnviv) (GLenum target, GLenum pname, const GLint *params);
extern void (APIENTRY *glTexGend )(GLenum coord, GLenum pname, GLdouble param);
extern void (APIENTRY *glTexGendv) (GLenum coord, GLenum pname, const GLdouble *params);
extern void (APIENTRY *glTexGenf )(GLenum coord, GLenum pname, GLfloat param);
extern void (APIENTRY *glTexGenfv) (GLenum coord, GLenum pname, const GLfloat *params);
extern void (APIENTRY *glTexGeni )(GLenum coord, GLenum pname, GLint param);
extern void (APIENTRY *glTexGeniv) (GLenum coord, GLenum pname, const GLint *params);
extern void (APIENTRY *glTexImage1D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
extern void (APIENTRY *glTexImage2D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
extern void (APIENTRY *glTexParameterf) (GLenum target, GLenum pname, GLfloat param);
extern void (APIENTRY *glTexParameterfv) (GLenum target, GLenum pname, const GLfloat *params);
extern void (APIENTRY *glTexParameteri) (GLenum target, GLenum pname, GLint param);
extern void (APIENTRY *glTexParameteriv) (GLenum target, GLenum pname, const GLint *params);
extern void (APIENTRY *glTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
extern void (APIENTRY *glTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
extern void (APIENTRY *glTranslated )(GLdouble x, GLdouble y, GLdouble z);
extern void (APIENTRY *glTranslatef) (GLfloat x, GLfloat y, GLfloat z);
extern void (APIENTRY *glVertex2d) (GLdouble x, GLdouble y);
extern void (APIENTRY *glVertex2dv) (const GLdouble *v);
extern void (APIENTRY *glVertex2f )(GLfloat x, GLfloat y);
extern void (APIENTRY *glVertex2fv) (const GLfloat *v);
extern void (APIENTRY *glVertex2i )(GLint x, GLint y);
extern void (APIENTRY *glVertex2iv) (const GLint *v);
extern void (APIENTRY *glVertex2s )(GLshort x, GLshort y);
extern void (APIENTRY *glVertex2sv) (const GLshort *v);
extern void (APIENTRY *glVertex3d) (GLdouble x, GLdouble y, GLdouble z);
extern void (APIENTRY *glVertex3dv) (const GLdouble *v);
extern void (APIENTRY *glVertex3f) (GLfloat x, GLfloat y, GLfloat z);
extern void (APIENTRY *glVertex3fv) (const GLfloat *v);
extern void (APIENTRY *glVertex3i) (GLint x, GLint y, GLint z);
extern void (APIENTRY *glVertex3iv) (const GLint *v);
extern void (APIENTRY *glVertex3s) (GLshort x, GLshort y, GLshort z);
extern void (APIENTRY *glVertex3sv) (const GLshort *v);
extern void (APIENTRY *glVertex4d) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
extern void (APIENTRY *glVertex4dv) (const GLdouble *v);
extern void (APIENTRY *glVertex4f) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
extern void (APIENTRY *glVertex4fv) (const GLfloat *v);
extern void (APIENTRY *glVertex4i) (GLint x, GLint y, GLint z, GLint w);
extern void (APIENTRY *glVertex4iv) (const GLint *v);
extern void (APIENTRY *glVertex4s )(GLshort x, GLshort y, GLshort z, GLshort w);
extern void (APIENTRY *glVertex4sv) (const GLshort *v);
extern void (APIENTRY *glVertexPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void (APIENTRY *glViewport) (GLint x, GLint y, GLsizei width, GLsizei height);




// WGL Stuff :



BOOL   (APIENTRY * awglCopyContext)(HGLRC, HGLRC, UINT);
HGLRC  (APIENTRY * awglCreateContext)(HDC);
HGLRC (APIENTRY * awglCreateLayerContext)(HDC, int);
BOOL  (APIENTRY * awglDeleteContext)(HGLRC);
HGLRC (APIENTRY * awglGetCurrentContext)(VOID);
HDC   (APIENTRY * awglGetCurrentDC)(VOID);
PROC  (APIENTRY * awglGetProcAddress)(LPCSTR);
BOOL  (APIENTRY * awglMakeCurrent)(HDC, HGLRC);
BOOL  (APIENTRY * awglShareLists)(HGLRC, HGLRC);
BOOL  (APIENTRY * awglUseFontBitmapsA)(HDC, DWORD, DWORD, DWORD);
BOOL  (APIENTRY * awglUseFontBitmapsW)(HDC, DWORD, DWORD, DWORD);
BOOL  (APIENTRY * awglUseFontBitmaps)(HDC, DWORD, DWORD, DWORD);
BOOL  (APIENTRY * awglSwapLayerBuffers)(HDC, UINT);






#define GL_ARB_multitexture                 1
#define GL_EXT_abgr                         1
#define GL_EXT_bgra                         1
#define GL_EXT_clip_volume_hint             1
#define GL_EXT_compiled_vertex_array        1
#define GL_EXT_cull_vertex                  1
#define GL_EXT_packed_pixels                1
#define GL_EXT_point_parameters             1
#define GL_EXT_stencil_wrap                 1
#define GL_EXT_texture_env_add              1
#define GL_EXT_texture_env_combine          1
#define GL_EXT_vertex_array                 1
#define GL_NV_texgen_reflection             1
#define GL_NV_texture_env_combine4          1
#define GL_WIN_swap_hint                    1

/* EXT_abgr */
#define GL_ABGR_EXT                         0x8000

/* EXT_packed_pixels */
#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036

/* EXT_vertex_array */
#define GL_VERTEX_ARRAY_EXT                 0x8074
#define GL_NORMAL_ARRAY_EXT                 0x8075
#define GL_COLOR_ARRAY_EXT                  0x8076
#define GL_INDEX_ARRAY_EXT                  0x8077
#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093

/* EXT_bgra */
#define GL_BGR_EXT                          0x80E0
#define GL_BGRA_EXT                         0x80E1

/* EXT_clip_volume_hint */
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0

/* EXT_point_parameters */
#define GL_POINT_SIZE_MIN_EXT               0x8126
#define GL_POINT_SIZE_MAX_EXT               0x8127
#define GL_POINT_FADE_THRESHOLD_SIZE_EXT    0x8128
#define GL_DISTANCE_ATTENUATION_EXT         0x8129

/* EXT_compiled_vertex_array */
#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9

/* EXT_cull_vertex */
#define GL_CULL_VERTEX_EXT                  0x81AA
#define GL_CULL_VERTEX_EYE_POSITION_EXT     0x81AB
#define GL_CULL_VERTEX_OBJECT_POSITION_EXT  0x81AC

/* ARB_multitexture */
#define GL_ACTIVE_TEXTURE_ARB               0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_TEXTURE5_ARB                     0x84C5
#define GL_TEXTURE6_ARB                     0x84C6
#define GL_TEXTURE7_ARB                     0x84C7
#define GL_TEXTURE8_ARB                     0x84C8
#define GL_TEXTURE9_ARB                     0x84C9
#define GL_TEXTURE10_ARB                    0x84CA
#define GL_TEXTURE11_ARB                    0x84CB
#define GL_TEXTURE12_ARB                    0x84CC
#define GL_TEXTURE13_ARB                    0x84CD
#define GL_TEXTURE14_ARB                    0x84CE
#define GL_TEXTURE15_ARB                    0x84CF
#define GL_TEXTURE16_ARB                    0x84D0
#define GL_TEXTURE17_ARB                    0x84D1
#define GL_TEXTURE18_ARB                    0x84D2
#define GL_TEXTURE19_ARB                    0x84D3
#define GL_TEXTURE20_ARB                    0x84D4
#define GL_TEXTURE21_ARB                    0x84D5
#define GL_TEXTURE22_ARB                    0x84D6
#define GL_TEXTURE23_ARB                    0x84D7
#define GL_TEXTURE24_ARB                    0x84D8
#define GL_TEXTURE25_ARB                    0x84D9
#define GL_TEXTURE26_ARB                    0x84DA
#define GL_TEXTURE27_ARB                    0x84DB
#define GL_TEXTURE28_ARB                    0x84DC
#define GL_TEXTURE29_ARB                    0x84DD
#define GL_TEXTURE30_ARB                    0x84DE
#define GL_TEXTURE31_ARB                    0x84DF

/* EXT_stencil_wrap */
#define GL_INCR_WRAP_EXT                    0x8507
#define GL_DECR_WRAP_EXT                    0x8508

/* NV_texgen_reflection */
#define GL_NORMAL_MAP_NV                    0x8511
#define GL_REFLECTION_MAP_NV                0x8512

/* EXT_texture_env_combine */
#define GL_COMBINE_EXT                      0x8570
#define GL_COMBINE_RGB_EXT                  0x8571
#define GL_COMBINE_ALPHA_EXT                0x8572
#define GL_RGB_SCALE_EXT                    0x8573
#define GL_ADD_SIGNED_EXT                   0x8574
#define GL_INTERPOLATE_EXT                  0x8575
#define GL_CONSTANT_EXT                     0x8576
#define GL_PRIMARY_COLOR_EXT                0x8577
#define GL_PREVIOUS_EXT                     0x8578
#define GL_SOURCE0_RGB_EXT                  0x8580
#define GL_SOURCE1_RGB_EXT                  0x8581
#define GL_SOURCE2_RGB_EXT                  0x8582
#define GL_SOURCE0_ALPHA_EXT                0x8588
#define GL_SOURCE1_ALPHA_EXT                0x8589
#define GL_SOURCE2_ALPHA_EXT                0x858A
#define GL_OPERAND0_RGB_EXT                 0x8590
#define GL_OPERAND1_RGB_EXT                 0x8591
#define GL_OPERAND2_RGB_EXT                 0x8592
#define GL_OPERAND0_ALPHA_EXT               0x8598
#define GL_OPERAND1_ALPHA_EXT               0x8599
#define GL_OPERAND2_ALPHA_EXT               0x859A

/* NV_texture_env_combine4 */
#define GL_COMBINE4_NV                      0x8503
#define GL_SOURCE3_RGB_NV                   0x8583
#define GL_SOURCE3_ALPHA_NV                 0x858B
#define GL_OPERAND3_RGB_NV                  0x8593
#define GL_OPERAND3_ALPHA_NV                0x859B

/*************************************************************/

/* EXT_vertex_array */
typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

/* ARB_multitexture */
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum target);

/* EXT_compiled_vertex_array */
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

/* EXT_cull_vertex */
typedef void (APIENTRY * PFNGLCULLPARAMETERDVEXTPROC) (GLenum pname, GLdouble* params);
typedef void (APIENTRY * PFNGLCULLPARAMETERFVEXTPROC) (GLenum pname, GLfloat* params);

/* WIN_swap_hint */
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

/* EXT_point_parameter */
typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (GLenum pname, const GLfloat *params);


// ARB_MULTITEXTURE 
extern  PFNGLACTIVETEXTUREARBPROC glActiveTextureARB ;
extern	PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB ;
extern 	PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern	PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB;



// COMPILED VERTEX ARRAY
extern	PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
extern	PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

// 3DFX_gamma 
#include <windows.h>
typedef int (APIENTRY *SETDEVICEGAMMARAMP3DFXPROC) (HDC DC,void *gamma);
typedef int (APIENTRY *GETDEVICEGAMMARAMP3DFXPROC) (HDC DC,void *gamma);


extern SETDEVICEGAMMARAMP3DFXPROC wglSetDeviceGammaRamp3DFX;
extern GETDEVICEGAMMARAMP3DFXPROC wglGetDeviceGammaRamp3DFX;



// WGL_swap_contol

typedef int (APIENTRY *WGLSETSWAPINTERVALPROC) (int interval);
typedef int (APIENTRY *WGLGETSWAPINTERVALPROC) (void );

extern WGLSETSWAPINTERVALPROC wglSwapIntervalEXT;
extern WGLGETSWAPINTERVALPROC wglGetSwapIntervalEXT;


// WGL_ARB_extensions_string 

#define WGL_ARB_extensions_string 1


typedef const char * (APIENTRY * WGLGETEXTENSIONSSTRING_ARB_PROC ) ( HDC hdc );
 

extern WGLGETEXTENSIONSSTRING_ARB_PROC wglGetExtensionsStringARB;


// Polygon-Offset 

#define GL_POLYGON_OFFSET               0x8037
#define GL_POLYGON_OFFSET_FACTOR        0x8038
#define GL_POLYGON_OFFSET_BIAS          0x8039


// ARB_texture_compression 

#define GL_ARB_texture_compression 1



#define	GL_COMPRESSED_ALPHA_ARB				0x84E9
#define	GL_COMPRESSED_LUMINANCE_ARB			0x84EA
#define	GL_COMPRESSED_LUMINANCE_ALPHA_ARB	0x84EB
#define	GL_COMPRESSED_INTENSITY_ARB			0x84EC
#define	GL_COMPRESSED_RGB_ARB				0x84ED
#define	GL_COMPRESSED_RGBA_ARB				0x84EE

#define GL_TEXTURE_COMPRESSION_HINT_ARB		0x84EF

#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define	GL_TEXTURE_COMPRESSED_ARB			0x86A1

#define	GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
#define	GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3


extern void (APIENTRY * glCompressedTexImage3DARB)(enum target, int level,
                                 int internalformat, int width,
                                 sizei height, sizei depth,
                                 int border, sizei imageSize,
                                 const void *data);
extern void (APIENTRY *glCompressedTexImage2DARB)(enum target, int level,
                                 int internalformat, sizei width,
                                 sizei height, int border, 
                                 sizei imageSize, const void *data);
extern void (APIENTRY *glCompressedTexImage1DARB)(enum target, int level,
                                 int internalformat, sizei width,
                                 int border, sizei imageSize,
                                 const void *data);
extern void (APIENTRY *glCompressedTexSubImage3DARB)(enum target, int level, 
                                    int xoffset, int yoffset,
                                    int zoffset, sizei width,
                                    sizei height, sizei depth,
                                    enum format, sizei imageSize,
                                    const void *data);
extern void (APIENTRY *glCompressedTexSubImage2DARB)(enum target, int level, 
                                    int xoffset, int yoffset,
                                    sizei width, sizei height,
                                    enum format, sizei imageSize,
                                    const void *data);
extern void (APIENTRY *glCompressedTexSubImage1DARB)(enum target, int level, 
                                    int xoffset, sizei width,
                                    enum format, sizei imageSize,
                                    const void *data);
extern void (APIENTRY *glGetCompressedTexImageARB)(enum target, int lod,
                                  const void *img);


// GL_S3_s3tc


#define  GL_COMPRESSED_RGB_S3TC_DXT1_EXT                   0x83F0
#define  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT                  0x83F1
#define  GL_COMPRESSED_RGBA_S3TC_DXT3_EXT                  0x83F2
#define  GL_COMPRESSED_RGBA_S3TC_DXT5_EXT                  0x83F3


// GL_3DFX_texture_compression_FXT1

#define GL_3DFX_texture_compression_FXT1 1

#define GL_COMPRESSED_RGB_FXT1_3DFX			  0x86B0
#define GL_COMPRESSED_RGBA_FXT1_3DFX		  0x86B1


// NV_register_combiners
// I think one time  we will use this 
// this is very powerful

#define GL_NV_register_combiners 1

#define GL_COMBINER0_NV                       0x8550
#define GL_COMBINER1_NV                       0x8551
#define GL_COMBINER2_NV                       0x8552
#define GL_COMBINER3_NV                       0x8553
#define GL_COMBINER4_NV                       0x8554
#define GL_COMBINER5_NV                       0x8555
#define GL_COMBINER6_NV                       0x8556
#define GL_COMBINER7_NV                       0x8557

#define GL_VARIABLE_A_NV                      0x8523
#define GL_VARIABLE_B_NV                      0x8524
#define GL_VARIABLE_C_NV                      0x8525
#define GL_VARIABLE_D_NV                      0x8526
#define GL_VARIABLE_E_NV                      0x8527
#define GL_VARIABLE_F_NV                      0x8528
#define GL_VARIABLE_G_NV                      0x8529

#define GL_CONSTANT_COLOR0_NV                 0x852a
#define GL_CONSTANT_COLOR1_NV                 0x852b
#define GL_PRIMARY_COLOR_NV                   0x852c
#define GL_SECONDARY_COLOR_NV                 0x852d
#define GL_SPARE0_NV                          0x852e
#define GL_SPARE1_NV                          0x852f

#define GL_UNSIGNED_IDENTITY_NV               0x8536
#define GL_UNSIGNED_INVERT_NV                 0x8537
#define GL_EXPAND_NORMAL_NV                   0x8538
#define GL_EXPAND_NEGATE_NV                   0x8539
#define GL_HALF_BIAS_NORMAL_NV                0x853a
#define GL_HALF_BIAS_NEGATE_NV                0x853b
#define GL_SIGNED_IDENTITY_NV                 0x853c
#define GL_SIGNED_NEGATE_NV                   0x853d

#define GL_E_TIMES_F_NV                       0x8531
#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV     0x8532

#define GL_SCALE_BY_TWO_NV                    0x853e
#define GL_SCALE_BY_FOUR_NV                   0x853f
#define GL_SCALE_BY_ONE_HALF_NV               0x8540

#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV       0x8541

#define GL_DISCARD_NV                         0x8530

#define GL_COMBINER_INPUT_NV                  0x8542
#define GL_COMBINER_MAPPING_NV                0x8543
#define GL_COMBINER_COMPONENT_USAGE_NV        0x8544

#define GL_COMBINER_AB_DOT_PRODUCT_NV         0x8545
#define GL_COMBINER_CD_DOT_PRODUCT_NV         0x8546
#define GL_COMBINER_MUX_SUM_NV                0x8547
#define GL_COMBINER_SCALE_NV                  0x8548
#define GL_COMBINER_BIAS_NV                   0x8549
#define GL_COMBINER_AB_OUTPUT_NV              0x854a
#define GL_COMBINER_CD_OUTPUT_NV              0x854b
#define GL_COMBINER_SUM_OUTPUT_NV             0x854c

#define GL_NUM_GENERAL_COMBINERS_NV           0x854e
#define GL_COLOR_SUM_CLAMP_NV                 0x854f

#define GL_MAX_GENERAL_COMBINERS_NV           0x854d

void (APIENTRY *glCombinerParameterfvNV)(GLenum pname,
                          const GLfloat *params);

void (APIENTRY *glCombinerParameterivNV)(GLenum pname,
                          const GLint *params);

void (APIENTRY *glCombinerParameterfNV)(GLenum pname,
                         GLfloat param);

void (APIENTRY *glCombinerParameteriNV)(GLenum pname,
                         GLint param);

extern void (APIENTRY *glCombinerInputNV)(GLenum stage,
                    GLenum portion,
                    GLenum variable,
                    GLenum input,
                    GLenum mapping,
                    GLenum componentUsage);

extern void (APIENTRY *glCombinerOutputNV)(GLenum stage,
                     GLenum portion, 
                     GLenum abOutput,
                     GLenum cdOutput,
                     GLenum sumOutput,
                     GLenum scale,
                     GLenum bias,
                     GLboolean abDotProduct,
                     GLboolean cdDotProduct,
                     GLboolean muxSum);

extern void (APIENTRY *glFinalCombinerInputNV)(GLenum variable,
                         GLenum input,
                         GLenum mapping,
                         GLenum componentUsage);

extern void (APIENTRY *glGetCombinerInputParameterfvNV)(GLenum stage,
                                  GLenum portion,
                                  GLenum variable,
                                  GLenum pname,
                                  GLfloat *params);

extern void (APIENTRY *glGetCombinerInputParameterivNV)(GLenum stage,
                                  GLenum portion,
                                  GLenum variable,
                                  GLenum pname,
                                  GLint *params);

extern void (APIENTRY *glGetCombinerOutputParameterfvNV)(GLenum stage,
                                   GLenum portion, 
                                   GLenum pname,
                                   GLfloat *params);

extern void (APIENTRY *glGetCombinerOutputParameterivNV)(GLenum stage,
                                   GLenum portion, 
                                   GLenum pname,
                                   GLint *params);

extern void (APIENTRY *glGetFinalCombinerInputParameterfvNV)(GLenum variable,
                                       GLenum pname,
                                       GLfloat *params);

extern void (APIENTRY *glGetFinalCombinerInputParameterivNV)(GLenum variable,
                                       GLenum pname,
                                       GLfloat *params);







#endif /*__GLINC_H__*/
