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
#include "opengl.h"
#include "glinc.h"
#include <windows.h>
#include "render.h"
#include "console.h"
#include "ui.h"
#include "keys.h"
#include "sys_key.h"

glconfig_t glconfig;
ext_info_t gl_ext_info;

static int opengl_initialized =0;

#if !defined _WIN32

#define _3DFX_DRIVER_NAME	"libMesaVoodooGL.so"
#define OPENGL_DRIVER_NAME	"libGL.so"

#else

#define _3DFX_DRIVER_NAME	"3dfxvgl"
#define OPENGL_DRIVER_NAME	"opengl32"

#endif 

HINSTANCE GL_dll=NULL;




 void (	APIENTRY * glAccum) (GLenum op, GLfloat value);
static  void (APIENTRY * glAlphaFunc) (GLenum func, GLclampf ref);
 GLboolean (APIENTRY *glAreTexturesResident) (GLsizei n, const GLuint *textures, GLboolean *residences);
 void (APIENTRY *glArrayElement )(GLint i);
 void (APIENTRY *glBegin )(GLenum mode);
static  void (APIENTRY *glBindTexture )(GLenum target, GLuint texture);
 void (APIENTRY *glBitmap )(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
static  void (APIENTRY *glBlendFunc) (GLenum sfactor, GLenum dfactor);
 void (APIENTRY *glCallList) (GLuint list);
 void (APIENTRY *glCallLists) (GLsizei n, GLenum type, const GLvoid *lists);
 void (APIENTRY *glClear) (GLbitfield mask);
 void (APIENTRY *glClearAccum) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
 void (APIENTRY *glClearColor) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
 void (APIENTRY *glClearDepth) (GLclampd depth);
 void (APIENTRY *glClearIndex) (GLfloat c);
 void (APIENTRY *glClearStencil) (GLint s);
 void (APIENTRY *glClipPlane) (GLenum plane, const GLdouble *equation);
 void (APIENTRY *glColor3b) (GLbyte red, GLbyte green, GLbyte blue);
 void (APIENTRY *glColor3bv) (const GLbyte *v);
 void (APIENTRY *glColor3d) (GLdouble red, GLdouble green, GLdouble blue);
 void (APIENTRY *glColor3dv) (const GLdouble *v);
 void (APIENTRY *glColor3f) (GLfloat red, GLfloat green, GLfloat blue);
 void (APIENTRY *glColor3fv) (const GLfloat *v);
 void (APIENTRY *glColor3i) (GLint red, GLint green, GLint blue);
 void (APIENTRY *glColor3iv) (const GLint *v);
 void (APIENTRY *glColor3s) (GLshort red, GLshort green, GLshort blue);
 void (APIENTRY *glColor3sv) (const GLshort *v);
 void (APIENTRY *glColor3ub) (GLubyte red, GLubyte green, GLubyte blue);
 void (APIENTRY *glColor3ubv) (const GLubyte *v);
 void (APIENTRY *glColor3ui) (GLuint red, GLuint green, GLuint blue);
 void (APIENTRY *glColor3uiv) (const GLuint *v);
 void (APIENTRY *glColor3us) (GLushort red, GLushort green, GLushort blue);
 void (APIENTRY *glColor3usv) (const GLushort *v);
 void (APIENTRY *glColor4b)(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
 void (APIENTRY *glColor4bv) (const GLbyte *v);
 void (APIENTRY *glColor4d) (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
 void (APIENTRY *glColor4dv) (const GLdouble *v);
 void (APIENTRY *glColor4f) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
 void (APIENTRY *glColor4fv) (const GLfloat *v);
 void (APIENTRY *glColor4i) (GLint red, GLint green, GLint blue, GLint alpha);
 void (APIENTRY *glColor4iv) (const GLint *v);
 void (APIENTRY *glColor4s) (GLshort red, GLshort green, GLshort blue, GLshort alpha);
 void (APIENTRY *glColor4sv) (const GLshort *v);
 void (APIENTRY *glColor4ub) (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
 void (APIENTRY *glColor4ubv) (const GLubyte *v);
 void (APIENTRY *glColor4ui) (GLuint red, GLuint green, GLuint blue, GLuint alpha);
 void (APIENTRY *glColor4uiv) (const GLuint *v);
 void (APIENTRY *glColor4us) (GLushort red, GLushort green, GLushort blue, GLushort alpha);
 void (APIENTRY *glColor4usv) (const GLushort *v);
 void (APIENTRY *glColorMask) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
 void (APIENTRY *glColorMaterial) (GLenum face, GLenum mode);
 void (APIENTRY *glColorPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
 void (APIENTRY *glCopyPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
 void (APIENTRY *glCopyTexImage1D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
 void (APIENTRY *glCopyTexImage2D) (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
 void (APIENTRY *glCopyTexSubImage1D )(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
 void (APIENTRY *glCopyTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
static  void (APIENTRY *glCullFace) (GLenum mode);
 void (APIENTRY *glDeleteLists) (GLuint list, GLsizei range);
 void (APIENTRY *glDeleteTextures )(GLsizei n, const GLuint *textures);
static void (APIENTRY *glDepthFunc )(GLenum func);
static void (APIENTRY *glDepthMask )(GLboolean flag);
 void (APIENTRY *glDepthRange) (GLclampd zNear, GLclampd zFar);
static void (APIENTRY *glDisable )(GLenum cap);
static void (APIENTRY *glDisableClientState) (GLenum array);
 void (APIENTRY *glDrawArrays) (GLenum mode, GLint first, GLsizei count);
 void (APIENTRY *glDrawBuffer) (GLenum mode);
 void (APIENTRY *glDrawElements) (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
 void (APIENTRY *glDrawPixels) (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
 void (APIENTRY *glEdgeFlag) (GLboolean flag);
 void (APIENTRY *glEdgeFlagPointer) (GLsizei stride, const GLvoid *pointer);
 void (APIENTRY *glEdgeFlagv) (const GLboolean *flag);
 void (APIENTRY *glEnable) (GLenum cap);
static void (APIENTRY *glEnableClientState) (GLenum array);
 void (APIENTRY *glEnd )(void);
 void (APIENTRY *glEndList) (void);
 void (APIENTRY *glEvalCoord1d) (GLdouble u);
 void (APIENTRY *glEvalCoord1dv) (const GLdouble *u);
 void (APIENTRY *glEvalCoord1f) (GLfloat u);
 void (APIENTRY *glEvalCoord1fv) (const GLfloat *u);
 void (APIENTRY *glEvalCoord2d)(GLdouble u, GLdouble v);
 void (APIENTRY *glEvalCoord2dv) (const GLdouble *u);
 void (APIENTRY *glEvalCoord2f) (GLfloat u, GLfloat v);
 void (APIENTRY *glEvalCoord2fv) (const GLfloat *u);
 void (APIENTRY *glEvalMesh1) (GLenum mode, GLint i1, GLint i2);
 void (APIENTRY *glEvalMesh2) (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
 void (APIENTRY *glEvalPoint1) (GLint i);
 void (APIENTRY *glEvalPoint2) (GLint i, GLint j);
 void (APIENTRY *glFeedbackBuffer) (GLsizei size, GLenum type, GLfloat *buffer);
 void (APIENTRY *glFinish )(void);
 void (APIENTRY *glFlush )(void);
 void (APIENTRY *glFogf )(GLenum pname, GLfloat param);
 void (APIENTRY *glFogfv) (GLenum pname, const GLfloat *params);
 void (APIENTRY *glFogi )(GLenum pname, GLint param);
 void (APIENTRY *glFogiv) (GLenum pname, const GLint *params);
 void (APIENTRY *glFrontFace) (GLenum mode);
 void (APIENTRY *glFrustum) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
 GLuint (APIENTRY *glGenLists) (GLsizei range);
 void (APIENTRY *glGenTextures) (GLsizei n, GLuint *textures);
 void (APIENTRY *glGetBooleanv) (GLenum pname, GLboolean *params);
 void (APIENTRY *glGetClipPlane) (GLenum plane, GLdouble *equation);
 void (APIENTRY *glGetDoublev) (GLenum pname, GLdouble *params);
 GLenum (APIENTRY *glGetError) (void);
 void (APIENTRY *glGetFloatv) (GLenum pname, GLfloat *params);
 void (APIENTRY *glGetIntegerv) (GLenum pname, GLint *params);
 void (APIENTRY *glGetLightfv) (GLenum light, GLenum pname, GLfloat *params);
 void (APIENTRY *glGetLightiv) (GLenum light, GLenum pname, GLint *params);
 void (APIENTRY *glGetMapdv) (GLenum target, GLenum query, GLdouble *v);
 void (APIENTRY *glGetMapfv) (GLenum target, GLenum query, GLfloat *v);
 void (APIENTRY *glGetMapiv) (GLenum target, GLenum query, GLint *v);
 void (APIENTRY *glGetMaterialfv) (GLenum face, GLenum pname, GLfloat *params);
 void (APIENTRY *glGetMaterialiv) (GLenum face, GLenum pname, GLint *params);
 void (APIENTRY *glGetPixelMapfv) (GLenum map, GLfloat *values);
 void (APIENTRY *glGetPixelMapuiv) (GLenum map, GLuint *values);
 void (APIENTRY *glGetPixelMapusv) (GLenum map, GLushort *values);
 void (APIENTRY *glGetPointerv) (GLenum pname, GLvoid* *params);
 void (APIENTRY *glGetPolygonStipple) (GLubyte *mask);
 const GLubyte * (APIENTRY *glGetString) (GLenum name);
 void (APIENTRY *glGetTexEnvfv )(GLenum target, GLenum pname, GLfloat *params);
 void (APIENTRY *glGetTexEnviv )(GLenum target, GLenum pname, GLint *params);
 void (APIENTRY *glGetTexGendv )(GLenum coord, GLenum pname, GLdouble *params);
 void (APIENTRY *glGetTexGenfv )(GLenum coord, GLenum pname, GLfloat *params);
 void (APIENTRY *glGetTexGeniv )(GLenum coord, GLenum pname, GLint *params);
 void (APIENTRY *glGetTexImage )(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
 void (APIENTRY *glGetTexLevelParameterfv) (GLenum target, GLint level, GLenum pname, GLfloat *params);
 void (APIENTRY *glGetTexLevelParameteriv) (GLenum target, GLint level, GLenum pname, GLint *params);
 void (APIENTRY *glGetTexParameterfv) (GLenum target, GLenum pname, GLfloat *params);
 void (APIENTRY *glGetTexParameteriv) (GLenum target, GLenum pname, GLint *params);
 void (APIENTRY *glHint )(GLenum target, GLenum mode);
 void (APIENTRY *glIndexMask )(GLuint mask);
 void (APIENTRY *glIndexPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
 void (APIENTRY *glIndexd )(GLdouble c);
 void (APIENTRY *glIndexdv) (const GLdouble *c);
 void (APIENTRY *glIndexf) (GLfloat c);
 void (APIENTRY *glIndexfv) (const GLfloat *c);
 void (APIENTRY *glIndexi) (GLint c);
 void (APIENTRY *glIndexiv) (const GLint *c);
 void (APIENTRY *glIndexs) (GLshort c);
 void (APIENTRY *glIndexsv) (const GLshort *c);
 void (APIENTRY *glIndexub) (GLubyte c);
 void (APIENTRY *glIndexubv) (const GLubyte *c);
 void (APIENTRY *glInitNames) (void);
 void (APIENTRY *glInterleavedArrays) (GLenum format, GLsizei stride, const GLvoid *pointer);
 GLboolean (APIENTRY *glIsEnabled) (GLenum cap);
 GLboolean (APIENTRY *glIsList) (GLuint list);
 GLboolean (APIENTRY *glIsTexture )(GLuint texture);
 void (APIENTRY *glLightModelf) (GLenum pname, GLfloat param);
 void (APIENTRY *glLightModelfv) (GLenum pname, const GLfloat *params);
 void (APIENTRY *glLightModeli) (GLenum pname, GLint param);
 void (APIENTRY *glLightModeliv) (GLenum pname, const GLint *params);
 void (APIENTRY *glLightf) (GLenum light, GLenum pname, GLfloat param);
 void (APIENTRY	*glLightfv) (GLenum light, GLenum pname, const GLfloat *params);
 void (APIENTRY *glLighti) (GLenum light, GLenum pname, GLint param);
 void (APIENTRY *glLightiv) (GLenum light, GLenum pname, const GLint *params);
 void (APIENTRY *glLineStipple) (GLint factor, GLushort pattern);
 void (APIENTRY *glLineWidth) (GLfloat width);
 void (APIENTRY *glListBase) (GLuint base);
 void (APIENTRY *glLoadIdentity) (void);
 void (APIENTRY *glLoadMatrixd)(const GLdouble *m);
 void (APIENTRY *glLoadMatrixf) (const GLfloat *m);
 void (APIENTRY *glLoadName) (GLuint name);
 void (APIENTRY *glLogicOp) (GLenum opcode);
 void (APIENTRY *glMap1d) (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
 void (APIENTRY *glMap1f) (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
 void (APIENTRY *glMap2d) (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
 void (APIENTRY *glMap2f) (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
 void (APIENTRY *glMapGrid1d) (GLint un, GLdouble u1, GLdouble u2);
 void (APIENTRY *glMapGrid1f) (GLint un, GLfloat u1, GLfloat u2);
 void (APIENTRY *glMapGrid2d) (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
 void (APIENTRY *glMapGrid2f) (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
 void (APIENTRY *glMaterialf) (GLenum face, GLenum pname, GLfloat param);
 void (APIENTRY *glMaterialfv) (GLenum face, GLenum pname, const GLfloat *params);
 void (APIENTRY *glMateriali) (GLenum face, GLenum pname, GLint param);
 void (APIENTRY *glMaterialiv) (GLenum face, GLenum pname, const GLint *params);
 void (APIENTRY *glMatrixMode) (GLenum mode);
 void (APIENTRY *glMultMatrixd) (const GLdouble *m);
 void (APIENTRY *glMultMatrixf) (const GLfloat *m);
 void (APIENTRY *glNewList) (GLuint list, GLenum mode);
 void (APIENTRY *glNormal3b) (GLbyte nx, GLbyte ny, GLbyte nz);
 void (APIENTRY *glNormal3bv) (const GLbyte *v);
 void (APIENTRY *glNormal3d) (GLdouble nx, GLdouble ny, GLdouble nz);
 void (APIENTRY *glNormal3dv) (const GLdouble *v);
 void (APIENTRY *glNormal3f) (GLfloat nx, GLfloat ny, GLfloat nz);
 void (APIENTRY *glNormal3fv) (const GLfloat *v);
 void (APIENTRY *glNormal3i )(GLint nx, GLint ny, GLint nz);
 void (APIENTRY *glNormal3iv) (const GLint *v);
 void (APIENTRY *glNormal3s) (GLshort nx, GLshort ny, GLshort nz);
 void (APIENTRY *glNormal3sv) (const GLshort *v);
 void (APIENTRY *glNormalPointer) (GLenum type, GLsizei stride, const GLvoid *pointer);
 void (APIENTRY *glOrtho) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
 void (APIENTRY *glPassThrough )(GLfloat token);
 void (APIENTRY *glPixelMapfv) (GLenum map, GLsizei mapsize, const GLfloat *values);
 void (APIENTRY *glPixelMapuiv) (GLenum map, GLsizei mapsize, const GLuint *values);
 void (APIENTRY *glPixelMapusv) (GLenum map, GLsizei mapsize, const GLushort *values);
 void (APIENTRY *glPixelStoref) (GLenum pname, GLfloat param);
 void (APIENTRY *glPixelStorei) (GLenum pname, GLint param);
 void (APIENTRY *glPixelTransferf) (GLenum pname, GLfloat param);
 void (APIENTRY *glPixelTransferi) (GLenum pname, GLint param);
 void (APIENTRY *glPixelZoom) (GLfloat xfactor, GLfloat yfactor);
 void (APIENTRY *glPointSize) (GLfloat size);
 void (APIENTRY *glPolygonMode) (GLenum face, GLenum mode);
 void (APIENTRY *glPolygonOffset) (GLfloat factor, GLfloat units);
 void (APIENTRY *glPolygonStipple) (const GLubyte *mask);
 void (APIENTRY *glPopAttrib) (void);
 void (APIENTRY *glPopClientAttrib) (void);
 void (APIENTRY *glPopMatrix) (void);
 void (APIENTRY *glPopName) (void);
 void (APIENTRY *glPrioritizeTextures) (GLsizei n, const GLuint *textures, const GLclampf *priorities);
 void (APIENTRY *glPushAttrib) (GLbitfield mask);
 void (APIENTRY *glPushClientAttrib) (GLbitfield mask);
 void (APIENTRY *glPushMatrix) (void);
 void (APIENTRY *glPushName) (GLuint name);
 void (APIENTRY *glRasterPos2d) (GLdouble x, GLdouble y);
 void (APIENTRY *glRasterPos2dv) (const GLdouble *v);
 void (APIENTRY *glRasterPos2f) (GLfloat x, GLfloat y);
 void (APIENTRY *glRasterPos2fv) (const GLfloat *v);
 void (APIENTRY *glRasterPos2i) (GLint x, GLint y);
 void (APIENTRY *glRasterPos2iv) (const GLint *v);
 void (APIENTRY *glRasterPos2s) (GLshort x, GLshort y);
 void (APIENTRY *glRasterPos2sv) (const GLshort *v);
 void (APIENTRY *glRasterPos3d) (GLdouble x, GLdouble y, GLdouble z);
 void (APIENTRY *glRasterPos3dv) (const GLdouble *v);
 void (APIENTRY *glRasterPos3f) (GLfloat x, GLfloat y, GLfloat z);
 void (APIENTRY *glRasterPos3fv) (const GLfloat *v);
 void (APIENTRY *glRasterPos3i) (GLint x, GLint y, GLint z);
 void (APIENTRY *glRasterPos3iv) (const GLint *v);
 void (APIENTRY *glRasterPos3s) (GLshort x, GLshort y, GLshort z);
 void (APIENTRY *glRasterPos3sv) (const GLshort *v);
 void (APIENTRY *glRasterPos4d) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
 void (APIENTRY *glRasterPos4dv) (const GLdouble *v);
 void (APIENTRY *glRasterPos4f) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
 void (APIENTRY *glRasterPos4fv) (const GLfloat *v);
 void (APIENTRY *glRasterPos4i) (GLint x, GLint y, GLint z, GLint w);
 void (APIENTRY *glRasterPos4iv) (const GLint *v);
 void (APIENTRY *glRasterPos4s) (GLshort x, GLshort y, GLshort z, GLshort w);
 void (APIENTRY *glRasterPos4sv) (const GLshort *v);
 void (APIENTRY *glReadBuffer) (GLenum mode);
 void (APIENTRY *glReadPixels) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
 void (APIENTRY *glRectd) (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
 void (APIENTRY *glRectdv) (const GLdouble *v1, const GLdouble *v2);
 void (APIENTRY *glRectf) (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
 void (APIENTRY *glRectfv) (const GLfloat *v1, const GLfloat *v2);
 void (APIENTRY *glRecti) (GLint x1, GLint y1, GLint x2, GLint y2);
 void (APIENTRY *glRectiv) (const GLint *v1, const GLint *v2);
 void (APIENTRY *glRects) (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
 void (APIENTRY *glRectsv) (const GLshort *v1, const GLshort *v2);
 GLint (APIENTRY *glRenderMode) (GLenum mode);
 void (APIENTRY *glRotated) (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
 void (APIENTRY *glRotatef) (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
 void (APIENTRY *glScaled) (GLdouble x, GLdouble y, GLdouble z);
 void (APIENTRY *glScalef) (GLfloat x, GLfloat y, GLfloat z);
 void (APIENTRY *glScissor) (GLint x, GLint y, GLsizei width, GLsizei height);
 void (APIENTRY *glSelectBuffer) (GLsizei size, GLuint *buffer);
 void (APIENTRY *glShadeModel) (GLenum mode);
 void (APIENTRY *glStencilFunc) (GLenum func, GLint ref, GLuint mask);
 void (APIENTRY *glStencilMask) (GLuint mask);
 void (APIENTRY *glStencilOp) (GLenum fail, GLenum zfail, GLenum zpass);
 void (APIENTRY *glTexCoord1d) (GLdouble s);
 void (APIENTRY *glTexCoord1dv) (const GLdouble *v);
 void (APIENTRY *glTexCoord1f) (GLfloat s);
 void (APIENTRY *glTexCoord1fv) (const GLfloat *v);
 void (APIENTRY *glTexCoord1i) (GLint s);
 void (APIENTRY *glTexCoord1iv) (const GLint *v);
 void (APIENTRY *glTexCoord1s) (GLshort s);
 void (APIENTRY *glTexCoord1sv) (const GLshort *v);
 void (APIENTRY *glTexCoord2d) (GLdouble s, GLdouble t);
 void (APIENTRY *glTexCoord2dv) (const GLdouble *v);
 void (APIENTRY *glTexCoord2f) (GLfloat s, GLfloat t);
 void (APIENTRY *glTexCoord2fv) (const GLfloat *v);
 void (APIENTRY *glTexCoord2i) (GLint s, GLint t);
 void (APIENTRY *glTexCoord2iv) (const GLint *v);
 void (APIENTRY *glTexCoord2s) (GLshort s, GLshort t);
 void (APIENTRY *glTexCoord2sv) (const GLshort *v);
 void (APIENTRY *glTexCoord3d)(GLdouble s, GLdouble t, GLdouble r);
 void (APIENTRY *glTexCoord3dv) (const GLdouble *v);
 void (APIENTRY *glTexCoord3f) (GLfloat s, GLfloat t, GLfloat r);
 void (APIENTRY *glTexCoord3fv) (const GLfloat *v);
 void (APIENTRY *glTexCoord3i) (GLint s, GLint t, GLint r);
 void (APIENTRY *glTexCoord3iv) (const GLint *v);
 void (APIENTRY *glTexCoord3s) (GLshort s, GLshort t, GLshort r);
 void (APIENTRY *glTexCoord3sv) (const GLshort *v);
 void (APIENTRY *glTexCoord4d) (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
 void (APIENTRY *glTexCoord4dv) (const GLdouble *v);
 void (APIENTRY *glTexCoord4f) (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
 void (APIENTRY *glTexCoord4fv) (const GLfloat *v);
 void (APIENTRY *glTexCoord4i) (GLint s, GLint t, GLint r, GLint q);
 void (APIENTRY *glTexCoord4iv) (const GLint *v);
 void (APIENTRY *glTexCoord4s) (GLshort s, GLshort t, GLshort r, GLshort q);
 void (APIENTRY *glTexCoord4sv) (const GLshort *v);
 void (APIENTRY *glTexCoordPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static  void (APIENTRY *glTexEnvf) (GLenum target, GLenum pname, GLfloat param);
static  void (APIENTRY *glTexEnvfv) (GLenum target, GLenum pname, const GLfloat *params);
static  void (APIENTRY *glTexEnvi )(GLenum target, GLenum pname, GLint param);
static  void (APIENTRY *glTexEnviv) (GLenum target, GLenum pname, const GLint *params);
 void (APIENTRY *glTexGend )(GLenum coord, GLenum pname, GLdouble param);
 void (APIENTRY *glTexGendv) (GLenum coord, GLenum pname, const GLdouble *params);
 void (APIENTRY *glTexGenf )(GLenum coord, GLenum pname, GLfloat param);
 void (APIENTRY *glTexGenfv) (GLenum coord, GLenum pname, const GLfloat *params);
 void (APIENTRY *glTexGeni )(GLenum coord, GLenum pname, GLint param);
 void (APIENTRY *glTexGeniv) (GLenum coord, GLenum pname, const GLint *params);
 void (APIENTRY *glTexImage1D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
 void (APIENTRY *glTexImage2D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
 void (APIENTRY *glTexParameterf) (GLenum target, GLenum pname, GLfloat param);
 void (APIENTRY *glTexParameterfv) (GLenum target, GLenum pname, const GLfloat *params);
 void (APIENTRY *glTexParameteri) (GLenum target, GLenum pname, GLint param);
 void (APIENTRY *glTexParameteriv) (GLenum target, GLenum pname, const GLint *params);
 void (APIENTRY *glTexSubImage1D) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
 void (APIENTRY *glTexSubImage2D) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
 void (APIENTRY *glTranslated )(GLdouble x, GLdouble y, GLdouble z);
 void (APIENTRY *glTranslatef) (GLfloat x, GLfloat y, GLfloat z);
 void (APIENTRY *glVertex2d) (GLdouble x, GLdouble y);
 void (APIENTRY *glVertex2dv) (const GLdouble *v);
 void (APIENTRY *glVertex2f )(GLfloat x, GLfloat y);
 void (APIENTRY *glVertex2fv) (const GLfloat *v);
 void (APIENTRY *glVertex2i )(GLint x, GLint y);
 void (APIENTRY *glVertex2iv) (const GLint *v);
 void (APIENTRY *glVertex2s )(GLshort x, GLshort y);
 void (APIENTRY *glVertex2sv) (const GLshort *v);
 void (APIENTRY *glVertex3d) (GLdouble x, GLdouble y, GLdouble z);
 void (APIENTRY *glVertex3dv) (const GLdouble *v);
 void (APIENTRY *glVertex3f) (GLfloat x, GLfloat y, GLfloat z);
 void (APIENTRY *glVertex3fv) (const GLfloat *v);
 void (APIENTRY *glVertex3i) (GLint x, GLint y, GLint z);
 void (APIENTRY *glVertex3iv) (const GLint *v);
 void (APIENTRY *glVertex3s) (GLshort x, GLshort y, GLshort z);
 void (APIENTRY *glVertex3sv) (const GLshort *v);
 void (APIENTRY *glVertex4d) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
 void (APIENTRY *glVertex4dv) (const GLdouble *v);
 void (APIENTRY *glVertex4f) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
 void (APIENTRY *glVertex4fv) (const GLfloat *v);
 void (APIENTRY *glVertex4i) (GLint x, GLint y, GLint z, GLint w);
 void (APIENTRY *glVertex4iv) (const GLint *v);
 void (APIENTRY *glVertex4s )(GLshort x, GLshort y, GLshort z, GLshort w);
 void (APIENTRY *glVertex4sv) (const GLshort *v);
 void (APIENTRY *glVertexPointer) (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
 void (APIENTRY *glViewport) (GLint x, GLint y, GLsizei width, GLsizei height);


// WGL_SWAP_CONTROL
WGLSETSWAPINTERVALPROC wglSwapIntervalEXT=NULL;
WGLGETSWAPINTERVALPROC wglGetSwapIntervalEXT=NULL;


// ARB_MULTITEXTURE 
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB=NULL ;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB=NULL ;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB=NULL;
PFNGLMULTITEXCOORD2FVARBPROC glMultiTexCoord2fvARB=NULL;

// COMPILED VERTEX ARRAY
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT=NULL;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT=NULL;


// WGL_3DFX_GAMMA

SETDEVICEGAMMARAMP3DFXPROC  wglSetDeviceGammaRamp3DFX=NULL;
GETDEVICEGAMMARAMP3DFXPROC  wglGetDeviceGammaRamp3DFX=NULL;



// WGL_ARB_extensions_string 

WGLGETEXTENSIONSSTRING_ARB_PROC wglGetExtensionsStringARB =NULL;

// ARB_texture_compression 
void (APIENTRY * glCompressedTexImage3DARB)(enum target, int level,
                                 int internalformat, sizei width,
                                 sizei height, sizei depth,
                                 int border, sizei imageSize,
                                 const void *data)=0;
void (APIENTRY *glCompressedTexImage2DARB)(enum target, int level,
                                 int internalformat, sizei width,
                                 sizei height, int border, 
                                 sizei imageSize, const void *data)=0;
void (APIENTRY *glCompressedTexImage1DARB)(enum target, int level,
                                 int internalformat, sizei width,
                                 int border, sizei imageSize,
                                 const void *data)=0;
void (APIENTRY *glCompressedTexSubImage3DARB)(enum target, int level, 
                                    int xoffset, int yoffset,
                                    int zoffset, sizei width,
                                    sizei height, sizei depth,
                                    enum format, sizei imageSize,
                                    const void *data)=0;
void (APIENTRY *glCompressedTexSubImage2DARB)(enum target, int level, 
                                    int xoffset, int yoffset,
                                    sizei width, sizei height,
                                    enum format, sizei imageSize,
                                    const void *data)=0;
void (APIENTRY *glCompressedTexSubImage1DARB)(enum target, int level, 
                                    int xoffset, sizei width,
                                    enum format, sizei imageSize,
                                    const void *data)=0;
void (APIENTRY *glGetCompressedTexImageARB)(enum target, int lod,
                                  const void *img)=0;


// NV_register_combiners

void (APIENTRY *glCombinerParameterfvNV)(GLenum pname,
                          const GLfloat *params);

void (APIENTRY *glCombinerParameterivNV)(GLenum pname,
                          const GLint *params);

void (APIENTRY *glCombinerParameterfNV)(GLenum pname,
                         GLfloat param);

void (APIENTRY *glCombinerParameteriNV)(GLenum pname,
                         GLint param);

void (APIENTRY *glCombinerInputNV)(GLenum stage,
                    GLenum portion,
                    GLenum variable,
                    GLenum input,
                    GLenum mapping,
                    GLenum componentUsage);

void (APIENTRY *glCombinerOutputNV)(GLenum stage,
                     GLenum portion, 
                     GLenum abOutput,
                     GLenum cdOutput,
                     GLenum sumOutput,
                     GLenum scale,
                     GLenum bias,
                     GLboolean abDotProduct,
                     GLboolean cdDotProduct,
                     GLboolean muxSum);

void (APIENTRY *glFinalCombinerInputNV)(GLenum variable,
                         GLenum input,
                         GLenum mapping,
                         GLenum componentUsage);

void (APIENTRY *glGetCombinerInputParameterfvNV)(GLenum stage,
                                  GLenum portion,
                                  GLenum variable,
                                  GLenum pname,
                                  GLfloat *params)=0;

void (APIENTRY *glGetCombinerInputParameterivNV)(GLenum stage,
                                  GLenum portion,
                                  GLenum variable,
                                  GLenum pname,
                                  GLint *params)=0;

void (APIENTRY *glGetCombinerOutputParameterfvNV)(GLenum stage,
                                   GLenum portion, 
                                   GLenum pname,
                                   GLfloat *params)=0;

void (APIENTRY *glGetCombinerOutputParameterivNV)(GLenum stage,
                                   GLenum portion, 
                                   GLenum pname,
                                   GLint *params)=0;

void (APIENTRY *glGetFinalCombinerInputParameterfvNV)(GLenum variable,
                                       GLenum pname,
                                       GLfloat *params)=0;

void (APIENTRY *glGetFinalCombinerInputParameterivNV)(GLenum variable,
                                       GLenum pname,
                                       GLfloat *params)=0;






int GL_LoadDll ( char * name )
{


	HINSTANCE dll ;


	if (GL_dll )  // Error 
		return 0; 

	GL_dll= LoadLibrary(name);


	if (!GL_dll )
	{
		return 0;

	}

	dll=GL_dll;





	glAccum =(void *) GetProcAddress(dll,"glAccum");
	glAlphaFunc =(void *) GetProcAddress(dll,"glAlphaFunc");
	glAreTexturesResident =(void *) GetProcAddress(dll,"glAreTexturesResident");
	glArrayElement =(void *) GetProcAddress(dll,"glArrayElement");
	glBegin =(void *) GetProcAddress(dll,"glBegin");
	glBindTexture =(void *) GetProcAddress(dll,"glBindTexture");
	glBitmap =(void *) GetProcAddress(dll,"glBitmap");
	glBlendFunc =(void *) GetProcAddress(dll,"glBlendFunc");
	glCallList =(void *) GetProcAddress(dll,"glCallList");
	glCallLists =(void *) GetProcAddress(dll,"glCallLists");
	glClear =(void *) GetProcAddress(dll,"glClear");
	glClearAccum =(void *) GetProcAddress(dll,"glClearAccum");
	glClearColor =(void *) GetProcAddress(dll,"glClearColor");
	glClearDepth =(void *) GetProcAddress(dll,"glClearDepth");
	glClearIndex =(void *) GetProcAddress(dll,"glClearIndex");
	glClearStencil =(void *) GetProcAddress(dll,"glClearStencil");
	glClipPlane =(void *) GetProcAddress(dll,"glClipPlane");
	glColor3b =(void *) GetProcAddress(dll,"glColor3b");
	glColor3bv =(void *) GetProcAddress(dll,"glColor3bv");
	glColor3d =(void *) GetProcAddress(dll,"glColor3d");
	glColor3dv =(void *) GetProcAddress(dll,"glColor3dv");
	glColor3f =(void *) GetProcAddress(dll,"glColor3f");
	glColor3fv =(void *) GetProcAddress(dll,"glColor3fv");
	glColor3i  =(void *) GetProcAddress(dll,"glColor3i");
	glColor3iv =(void *) GetProcAddress(dll,"glColor3iv");
	glColor3s =(void *) GetProcAddress(dll,"glColor3s");
	glColor3sv =(void *) GetProcAddress(dll,"glColor3sv");
	glColor3ub =(void *) GetProcAddress(dll,"glColor3ub");
	glColor3ubv =(void *) GetProcAddress(dll,"glColor3ubv");
	glColor3ui =(void *) GetProcAddress(dll,"glColor3ui");
	glColor3uiv =(void *) GetProcAddress(dll,"glColor3uiv");
	glColor3us =(void *) GetProcAddress(dll,"glColor3us"); //
	glColor3usv =(void *) GetProcAddress(dll,"glColor3usv");
	glColor4b =(void *) GetProcAddress(dll,"glColor4b");
	glColor4bv =(void *) GetProcAddress(dll,"glColor4bv");
	glColor4d =(void *) GetProcAddress(dll,"glColor4d");
	glColor4dv =(void *) GetProcAddress(dll,"glColor4dv");
	glColor4f =(void *) GetProcAddress(dll,"glColor4f");
	glColor4fv =(void *) GetProcAddress(dll,"glColor4fv");
	glColor4i =(void *) GetProcAddress(dll,"glColor4i");
	glColor4iv =(void *) GetProcAddress(dll,"glColor4iv");
	glColor4s =(void *) GetProcAddress(dll,"glColor4s");
	glColor4sv =(void *) GetProcAddress(dll,"glColor4sv");
	glColor4ub =(void *) GetProcAddress(dll,"glColor4ub");
	glColor4ubv =(void *) GetProcAddress(dll,"glColor4ubv");
	glColor4ui =(void *) GetProcAddress(dll,"glColor4ui");
	glColor4uiv =(void *) GetProcAddress(dll,"glColor4uiv");
	glColor4us =(void *) GetProcAddress(dll,"glColor4us");
	glColor4usv =(void *) GetProcAddress(dll,"glColor4usv");
	glColorMask =(void *) GetProcAddress(dll,"glColorMask");
	glColorMaterial =(void *) GetProcAddress(dll,"glColorMaterial");
	glColorPointer =(void *) GetProcAddress(dll,"glColorPointer");
	glCopyPixels =(void *) GetProcAddress(dll,"glCopyPixels");
	glCopyTexImage1D =(void *) GetProcAddress(dll,"glCopyTexImage1D");
	glCopyTexImage2D =(void *) GetProcAddress(dll,"glCopyTexImage2D");
	glCopyTexSubImage1D =(void *) GetProcAddress(dll,"glCopyTexSubImage1D");
	glCopyTexSubImage2D =(void *) GetProcAddress(dll,"glCopyTexSubImage2D");
	glCullFace =(void *) GetProcAddress(dll,"glCullFace");
	glDeleteLists =(void *) GetProcAddress(dll,"glDeleteLists");
	glDeleteTextures =(void *) GetProcAddress(dll,"glDeleteTextures");
	glDepthFunc =(void *) GetProcAddress(dll,"glDepthFunc");
	glDepthMask=(void *) GetProcAddress(dll,"glDepthMask");
	glDepthRange =(void *) GetProcAddress(dll,"glDepthRange");
	glDisable =(void *) GetProcAddress(dll,"glDisable");
	glDisableClientState =(void *) GetProcAddress(dll,"glDisableClientState");
	glDrawArrays=(void *) GetProcAddress(dll,"glDrawArrays");
	glDrawBuffer=(void *) GetProcAddress(dll,"glDrawBuffer");
	glDrawElements =(void *) GetProcAddress(dll,"glDrawElements");
	glDrawPixels =(void *) GetProcAddress(dll,"glDrawPixels");
	glEdgeFlag =(void *) GetProcAddress(dll,"glEdgeFlag");
	glEdgeFlagPointer =(void *) GetProcAddress(dll,"glEdgeFlagPointer");
	glEdgeFlagv =(void *) GetProcAddress(dll,"glEdgeFlagv");
	glEnable =(void *) GetProcAddress(dll,"glEnable");
	glEnableClientState =(void *) GetProcAddress(dll,"glEnableClientState");
	glEnd =(void *) GetProcAddress(dll,"glEnd");
	glEndList =(void *) GetProcAddress(dll,"glEndList");
	glEvalCoord1d =(void *) GetProcAddress(dll,"glEvalCoord1d");
	glEvalCoord1dv =(void *) GetProcAddress(dll,"glEvalCoord1dv");
	glEvalCoord1f=(void *) GetProcAddress(dll,"glEvalCoord1f");
	glEvalCoord1fv =(void *) GetProcAddress(dll,"glEvalCoord1fv");
	glEvalCoord2d =(void *) GetProcAddress(dll,"glEvalCoord2d");
	glEvalCoord2dv =(void *) GetProcAddress(dll,"glEvalCoord2dv");
	glEvalCoord2fv=(void *) GetProcAddress(dll,"glEvalCoord2fv");
	glEvalMesh1=(void *) GetProcAddress(dll,"glEvalMesh1");
	glEvalMesh2=(void *) GetProcAddress(dll,"glEvalMesh2");
	glEvalPoint1 =(void *) GetProcAddress(dll,"glEvalPoint1");
	glEvalPoint2 =(void *) GetProcAddress(dll,"glEvalPoint2");
	glFeedbackBuffer =(void *) GetProcAddress(dll,"glFeedbackBuffer");
	glFinish=(void *) GetProcAddress(dll,"glFinish");
	glFlush =(void *) GetProcAddress(dll,"glFlush");
	glFogf =(void *) GetProcAddress(dll,"glFogf");
	glFogfv =(void *) GetProcAddress(dll,"glFogfv");
	glFogi =(void *) GetProcAddress(dll,"glFogi");
	glFogiv =(void *) GetProcAddress(dll,"glFogiv");
	glFrontFace =(void *) GetProcAddress(dll,"glFrontFace");
	glFrustum =(void *) GetProcAddress(dll,"glFrustum");
	glGenLists =(void *) GetProcAddress(dll,"glGenLists");
	glGenTextures=(void *) GetProcAddress(dll,"glGenTextures");
	glGetBooleanv =(void *) GetProcAddress(dll,"glGetBooleanv");
	glGetClipPlane=(void *) GetProcAddress(dll,"glGetClipPlane");
	glGetDoublev=(void *) GetProcAddress(dll,"glGetDoublev");
	glGetError=(void *) GetProcAddress(dll,"glGetError");
	glGetFloatv =(void *) GetProcAddress(dll,"glGetFloatv");
	glGetIntegerv=(void *) GetProcAddress(dll,"glGetIntegerv");
	glGetLightfv=(void *) GetProcAddress(dll,"glGetLightfv");
	glGetLightiv =(void *) GetProcAddress(dll,"glGetLightiv");
	glGetMapdv=(void *) GetProcAddress(dll,"glGetMapdv");
	glGetMapfv=(void *) GetProcAddress(dll,"glGetMapfv");
	glGetMapiv =(void *) GetProcAddress(dll,"glGetMapiv");
	glGetMaterialfv=(void *) GetProcAddress(dll,"glGetMaterialfv");
	glGetMaterialiv=(void *) GetProcAddress(dll,"glGetMaterialiv");
	glGetPixelMapfv=(void *) GetProcAddress(dll,"glGetPixelMapfv");
	glGetPixelMapuiv=(void *) GetProcAddress(dll,"glGetPixelMapuiv");
	glGetPixelMapusv=(void *) GetProcAddress(dll,"glGetPixelMapusv");
	glGetPointerv=(void *) GetProcAddress(dll,"glGetPointerv");
	glGetPolygonStipple=(void *) GetProcAddress(dll,"glGetPolygonStipple");
	glGetString =(void *) GetProcAddress(dll,"glGetString");
	glGetTexEnvfv =(void *) GetProcAddress(dll,"glGetTexEnvfv");
	glGetTexEnviv =(void *) GetProcAddress(dll,"glGetTexEnviv");
	glGetTexGendv=(void *) GetProcAddress(dll,"glGetTexGendv");
	glGetTexGenfv=(void *) GetProcAddress(dll,"glGetTexGenfv");
	glGetTexGeniv=(void *) GetProcAddress(dll,"glGetTexGeniv");
	glGetTexImage =(void *) GetProcAddress(dll,"glGetTexImage");
	glGetTexLevelParameterfv =(void *) GetProcAddress(dll,"glGetTexLevelParameterfv");
	glGetTexLevelParameteriv =(void *) GetProcAddress(dll,"glGetTexLevelParameteriv");
	glGetTexParameterfv =(void *) GetProcAddress(dll,"glGetTexParameterfv");
	glGetTexParameteriv =(void *) GetProcAddress(dll,"glGetTexParameteriv");
	glHint =(void *) GetProcAddress(dll,"glHint");
	glIndexMask =(void *) GetProcAddress(dll,"glIndexMask");
	glIndexPointer =(void *) GetProcAddress(dll,"glIndexPointer");
	glIndexd =(void *) GetProcAddress(dll,"glIndexd");
	glIndexdv =(void *) GetProcAddress(dll,"glIndexdv");
	glIndexf=(void *) GetProcAddress(dll,"glIndexf");
	glIndexfv=(void *) GetProcAddress(dll,"glIndexfv");
	glIndexi =(void *) GetProcAddress(dll,"glIndexfv");
	glIndexiv =(void *) GetProcAddress(dll,"glIndexiv");
	glIndexs =(void *) GetProcAddress(dll,"glIndexs");
	glIndexsv =(void *) GetProcAddress(dll,"glIndexsv");
	glIndexub =(void *) GetProcAddress(dll,"glIndexub");
	glIndexubv=(void *) GetProcAddress(dll,"glIndexubv");
	glInitNames=(void *) GetProcAddress(dll,"glInitNames");
	glInterleavedArrays =(void *) GetProcAddress(dll,"glInterleavedArrays");
	glIsEnabled =(void *) GetProcAddress(dll,"glIsEnabled");
	glIsList =(void *) GetProcAddress(dll,"glIsList");
	glIsTexture=(void *) GetProcAddress(dll,"glIsTexture");
	glLightModelf =(void *) GetProcAddress(dll,"glLightModelf");
	glLightModelfv=(void *) GetProcAddress(dll,"glLightModelfv");
	glLightModeli=(void *) GetProcAddress(dll,"glLightModeli");
	glLightModeliv =(void *) GetProcAddress(dll,"glLightModeliv");
	glLightf =(void *) GetProcAddress(dll,"glLightf");
	glLightfv =(void *) GetProcAddress(dll,"glLightfv");
	glLighti =(void *) GetProcAddress(dll,"glLighti");
	glLightiv=(void *) GetProcAddress(dll,"glLightiv");
	glLineStipple=(void *) GetProcAddress(dll,"glLineStipple");
	glLineWidth =(void *) GetProcAddress(dll,"glLineWidth");
	glListBase =(void *) GetProcAddress(dll,"glListBase");
	glLoadIdentity =(void *) GetProcAddress(dll,"glLoadIdentity");
	glLoadMatrixd =(void *) GetProcAddress(dll,"glLoadMatrixd");
	glLoadMatrixf =(void *) GetProcAddress(dll,"glLoadMatrixf");
	glLoadName =(void *) GetProcAddress(dll,"glLoadName");
	glLogicOp =(void *) GetProcAddress(dll,"glLogicOp");
	glMap1d =(void *) GetProcAddress(dll,"glMap1d");
	glMap1f =(void *) GetProcAddress(dll,"glMap1d");
	glMap2d =(void *) GetProcAddress(dll,"glMap2d");
	glMap2f =(void *) GetProcAddress(dll,"glMap2f");
	glMapGrid1d=(void *) GetProcAddress(dll,"glMapGrid1d");
	glMapGrid1f =(void *) GetProcAddress(dll,"glMapGrid1f");
	glMapGrid2d =(void *) GetProcAddress(dll,"glMapGrid1f");
	glMapGrid2f =(void *) GetProcAddress(dll,"glMapGrid2f");
	glMaterialf =(void *) GetProcAddress(dll,"glMaterialf");
	glMaterialfv=(void *) GetProcAddress(dll,"glMaterialfv");
	glMateriali =(void *) GetProcAddress(dll,"glMateriali");
	glMaterialiv =(void *) GetProcAddress(dll,"glMaterialiv");
	glMatrixMode =(void *) GetProcAddress(dll,"glMatrixMode");
	glMultMatrixd =(void *) GetProcAddress(dll,"glMultMatrixd");
	glMultMatrixf =(void *) GetProcAddress(dll,"glMultMatrixf");
	glNewList =(void *) GetProcAddress(dll,"glNewList");
	glNormal3b =(void *) GetProcAddress(dll,"glNormal3b");
	glNormal3bv =(void *) GetProcAddress(dll,"glNormal3bv");
	glNormal3d =(void *) GetProcAddress(dll,"glNormal3d");
	glNormal3dv =(void *) GetProcAddress(dll,"glNormal3dv");
	glNormal3f =(void *) GetProcAddress(dll,"glNormal3f");
	glNormal3fv =(void *) GetProcAddress(dll,"glNormal3fv");
	glNormal3i =(void *) GetProcAddress(dll,"glNormal3i");
	glNormal3iv =(void *) GetProcAddress(dll,"glNormal3iv");
	glNormal3s =(void *) GetProcAddress(dll,"glNormal3s");
	glNormal3sv =(void *) GetProcAddress(dll,"glNormal3sv");
	glNormalPointer =(void *) GetProcAddress(dll,"glNormalPointer");
	glOrtho =(void *) GetProcAddress(dll,"glOrtho");
	glPassThrough =(void *) GetProcAddress(dll,"glPassThrough");
	glPixelMapfv =(void *) GetProcAddress(dll,"glPixelMapfv");
	glPixelMapuiv =(void *) GetProcAddress(dll,"glPixelMapuiv");
	glPixelMapusv =(void *) GetProcAddress(dll,"glPixelMapusv");
	glPixelStoref =(void *) GetProcAddress(dll,"glPixelStoref");
	glPixelStorei=(void *) GetProcAddress(dll,"glPixelStorei");
	glPixelTransferf =(void *) GetProcAddress(dll,"glPixelTransferf");
	glPixelTransferi=(void *) GetProcAddress(dll,"glPixelTransferi");
	glPixelZoom =(void *) GetProcAddress(dll,"glPixelZoom");
	glPointSize =(void *) GetProcAddress(dll,"glPointSize");
	glPolygonMode =(void *) GetProcAddress(dll,"glPolygonMode");
	glPolygonOffset =(void *) GetProcAddress(dll,"glPolygonOffset");
	glPolygonStipple =(void *) GetProcAddress(dll,"glPolygonStipple");
	glPopAttrib =(void *) GetProcAddress(dll,"glPopAttrib");
	glPopClientAttrib =(void *) GetProcAddress(dll,"glPopClientAttrib");
	glPopMatrix =(void *) GetProcAddress(dll,"glPopMatrix");
	glPopName =(void *) GetProcAddress(dll,"glPopName");
	glPrioritizeTextures =(void *) GetProcAddress(dll,"glPrioritizeTextures");
	glPushAttrib =(void *) GetProcAddress(dll,"glPushAttrib");
	glPushClientAttrib =(void *) GetProcAddress(dll,"glPushClientAttrib");
	glPushMatrix =(void *) GetProcAddress(dll,"glPushMatrix");
	glPushName =(void *) GetProcAddress(dll,"glPushName");
	glRasterPos2d =(void *) GetProcAddress(dll,"glRasterPos2d");
	glRasterPos2dv =(void *) GetProcAddress(dll,"glRasterPos2dv");
	glRasterPos2f =(void *) GetProcAddress(dll,"glRasterPos2f");
	glRasterPos2fv =(void *) GetProcAddress(dll,"glRasterPos2fv");
	glRasterPos2i =(void *) GetProcAddress(dll,"glRasterPos2i");
	glRasterPos2iv =(void *) GetProcAddress(dll,"glRasterPos2iv");
	glRasterPos2s =(void *) GetProcAddress(dll,"glRasterPos2s");
	glRasterPos2sv=(void *) GetProcAddress(dll,"glRasterPos2sv");
	glRasterPos3d =(void *) GetProcAddress(dll,"glRasterPos3d");
	glRasterPos3dv =(void *) GetProcAddress(dll,"glRasterPos3dv");
	glRasterPos3f =(void *) GetProcAddress(dll,"glRasterPos3f");
	glRasterPos3fv =(void *) GetProcAddress(dll,"glRasterPos3fv");
	glRasterPos3i =(void *) GetProcAddress(dll,"glRasterPos3i");
	glRasterPos3iv =(void *) GetProcAddress(dll,"glRasterPos3iv");
	glRasterPos3s =(void *) GetProcAddress(dll,"glRasterPos3s");
	glRasterPos3sv =(void *) GetProcAddress(dll,"glRasterPos3sv");
	glRasterPos4d =(void *) GetProcAddress(dll,"glRasterPos4d");
	glRasterPos4dv=(void *) GetProcAddress(dll,"glRasterPos4dv");
	glRasterPos4f =(void *) GetProcAddress(dll,"glRasterPos4f");
	glRasterPos4fv =(void *) GetProcAddress(dll,"glRasterPos4fv");
	glRasterPos4i =(void *) GetProcAddress(dll,"glRasterPos4i");
	glRasterPos4iv =(void *) GetProcAddress(dll,"glRasterPos4iv");
	glRasterPos4s =(void *) GetProcAddress(dll,"glRasterPos4s");
	glRasterPos4sv =(void *) GetProcAddress(dll,"glRasterPos4sv");
	glReadBuffer =(void *) GetProcAddress(dll,"glReadBuffer");
	glReadPixels =(void *) GetProcAddress(dll,"glReadPixels");
	glRectd =(void *) GetProcAddress(dll,"glRectd");
	glRectdv =(void *) GetProcAddress(dll,"glRectdv");
	glRectf =(void *) GetProcAddress(dll,"glRectf");
	glRectfv=(void *) GetProcAddress(dll,"glRectfv");
	glRecti =(void *) GetProcAddress(dll,"glRecti");
	glRectiv =(void *) GetProcAddress(dll,"glRectiv");
	glRects =(void *) GetProcAddress(dll,"glRects");
	glRectsv =(void *) GetProcAddress(dll,"glRectsv");
	glRenderMode =(void *) GetProcAddress(dll,"glRenderMode");
	glRotated =(void *) GetProcAddress(dll,"glRotated");
	glRotatef =(void *) GetProcAddress(dll,"glRotatef");
	glScaled =(void *) GetProcAddress(dll,"glScaled");
	glScalef =(void *) GetProcAddress(dll,"glScalef");
	glScissor =(void *) GetProcAddress(dll,"glScissor");
	glSelectBuffer =(void *) GetProcAddress(dll,"glSelectBuffer");
	glShadeModel=(void *) GetProcAddress(dll,"glShadeModel");
	glStencilFunc =(void *) GetProcAddress(dll,"glStencilFunc");
	glStencilMask=(void *) GetProcAddress(dll,"glStencilMask");
	glStencilOp =(void *) GetProcAddress(dll,"glStencilOp");
	glTexCoord1d =(void *) GetProcAddress(dll,"glTexCoord1d");
	glTexCoord1dv =(void *) GetProcAddress(dll,"glTexCoord1dv");
	glTexCoord1f =(void *) GetProcAddress(dll,"glTexCoord1f");
	glTexCoord1fv=(void *) GetProcAddress(dll,"glTexCoord1fv");
	glTexCoord1i =(void *) GetProcAddress(dll,"glTexCoord1i");
	glTexCoord1iv =(void *) GetProcAddress(dll,"glTexCoord1iv");
	glTexCoord1s =(void *) GetProcAddress(dll,"glTexCoord1s");
	glTexCoord1sv =(void *) GetProcAddress(dll,"glTexCoord1sv");
	glTexCoord2d =(void *) GetProcAddress(dll,"glTexCoord2d");
	glTexCoord2dv =(void *) GetProcAddress(dll,"glTexCoord2dv");
	glTexCoord2f =(void *) GetProcAddress(dll,"glTexCoord2f");
	glTexCoord2fv =(void *) GetProcAddress(dll,"glTexCoord2fv");
	glTexCoord2i =(void *) GetProcAddress(dll,"glTexCoord2i");
	glTexCoord2iv=(void *) GetProcAddress(dll,"glTexCoord2iv");
	glTexCoord2s=(void *) GetProcAddress(dll,"glTexCoord2s");
	glTexCoord2sv =(void *) GetProcAddress(dll,"glTexCoord2sv");
	glTexCoord3d =(void *) GetProcAddress(dll,"glTexCoord3d");
	glTexCoord3dv=(void *) GetProcAddress(dll,"glTexCoord3dv");
	glTexCoord3f =(void *) GetProcAddress(dll,"glTexCoord3f");
	glTexCoord3fv =(void *) GetProcAddress(dll,"glTexCoord3fv");
	glTexCoord3i =(void *) GetProcAddress(dll,"glTexCoord3i");
	glTexCoord3iv =(void *) GetProcAddress(dll,"glTexCoord3iv");
	glTexCoord3s =(void *) GetProcAddress(dll,"glTexCoord3s");
	glTexCoord3sv =(void *) GetProcAddress(dll,"glTexCoord3sv");
	glTexCoord4d =(void *) GetProcAddress(dll,"glTexCoord4d");
	glTexCoord4dv=(void *) GetProcAddress(dll,"glTexCoord4dv");
	glTexCoord4f =(void *) GetProcAddress(dll,"glTexCoord4f");
	glTexCoord4fv =(void *) GetProcAddress(dll,"glTexCoord4fv");
	glTexCoord4i =(void *) GetProcAddress(dll,"glTexCoord4i");
	glTexCoord4iv =(void *) GetProcAddress(dll,"glTexCoord4iv");
	glTexCoord4s=(void *) GetProcAddress(dll,"glTexCoord4s");
	glTexCoord4sv =(void *) GetProcAddress(dll,"glTexCoord4sv");
	glTexCoordPointer =(void *) GetProcAddress(dll,"glTexCoordPointer");
	glTexEnvf =(void *) GetProcAddress(dll,"glTexEnvf");
	glTexEnvfv =(void *) GetProcAddress(dll,"glTexEnvfv");
	glTexEnvi =(void *) GetProcAddress(dll,"glTexEnvi");
	glTexEnviv=(void *) GetProcAddress(dll,"glTexEnviv");
	glTexGend =(void *) GetProcAddress(dll,"glTexGend");
	glTexGendv =(void *) GetProcAddress(dll,"glTexGendv");
	glTexGenf =(void *) GetProcAddress(dll,"glTexGenf");
	glTexGenfv =(void *) GetProcAddress(dll,"glTexGenfv");
	glTexGeni =(void *) GetProcAddress(dll,"glTexGeni");
	glTexGeniv =(void *) GetProcAddress(dll,"glTexGeniv");
	glTexImage1D =(void *) GetProcAddress(dll,"glTexImage1D");
	glTexImage2D =(void *) GetProcAddress(dll,"glTexImage2D");
	glTexParameterf =(void *) GetProcAddress(dll,"glTexParameterf");
	glTexParameterfv=(void *) GetProcAddress(dll,"glTexParameterfv");
	glTexParameteri =(void *) GetProcAddress(dll,"glTexParameteri");
	glTexParameteriv =(void *) GetProcAddress(dll,"glTexParameteriv");
	glTexSubImage1D=(void *) GetProcAddress(dll,"glTexSubImage1D");
	glTexSubImage2D =(void *) GetProcAddress(dll,"glTexSubImage2D");
	glTranslated =(void *) GetProcAddress(dll,"glTranslated");
	glTranslatef =(void *) GetProcAddress(dll,"glTranslatef");
	glVertex2d=(void *) GetProcAddress(dll,"glVertex2d");
	glVertex2dv =(void *) GetProcAddress(dll,"glVertex2dv");
	glVertex2f =(void *) GetProcAddress(dll,"glVertex2f");
	glVertex2fv =(void *) GetProcAddress(dll,"glVertex2fv");
	glVertex2i =(void *) GetProcAddress(dll,"glVertex2i");
	glVertex2iv =(void *) GetProcAddress(dll,"glVertex2iv");
	glVertex2s =(void *) GetProcAddress(dll,"glVertex2s");
	glVertex2sv =(void *) GetProcAddress(dll,"glVertex2sv");
	glVertex3d =(void *) GetProcAddress(dll,"glVertex3d");
	glVertex3dv =(void *) GetProcAddress(dll,"glVertex3dv");
	glVertex3f =(void *) GetProcAddress(dll,"glVertex3f");
	glVertex3fv=(void *) GetProcAddress(dll,"glVertex3fv");
	glVertex3i =(void *) GetProcAddress(dll,"glVertex3i");
	glVertex3iv =(void *) GetProcAddress(dll,"glVertex3iv");
	glVertex3s =(void *) GetProcAddress(dll,"glVertex3s");
	glVertex3sv =(void *) GetProcAddress(dll,"glVertex3sv");
	glVertex4d =(void *) GetProcAddress(dll,"glVertex4d");
	glVertex4dv =(void *) GetProcAddress(dll,"glVertex4dv");
	glVertex4f =(void *) GetProcAddress(dll,"glVertex4f");
	glVertex4fv =(void *) GetProcAddress(dll,"glVertex4fv");
	glVertex4i =(void *) GetProcAddress(dll,"glVertex4i");
	glVertex4iv =(void *) GetProcAddress(dll,"glVertex4iv");
	glVertex4s =(void *) GetProcAddress(dll,"glVertex4s");
	glVertex4sv =(void *) GetProcAddress(dll,"glVertex4sv");
	glVertexPointer =(void *) GetProcAddress(dll,"glVertexPointer");
	glViewport =(void *) GetProcAddress(dll,"glViewport");





// OpenGL wgl prototypes

	awglCopyContext=(void *) GetProcAddress(dll,"wglCopyContext");
	awglCreateContext=(void *) GetProcAddress(dll,"wglCreateContext");
	awglCreateLayerContext=(void *) GetProcAddress(dll,"wglCreateLayerContext");
	awglDeleteContext=(void *) GetProcAddress(dll,"wglDeleteContext");
	awglGetCurrentContext=(void *) GetProcAddress(dll,"wglGetCurrentContext");
	awglGetCurrentDC=(void *) GetProcAddress(dll,"wglGetCurrentDC");
	awglGetProcAddress=(void *) GetProcAddress(dll,"wglGetProcAddress");
	awglMakeCurrent=(void *) GetProcAddress(dll,"wglMakeCurrent");
	awglShareLists=(void *) GetProcAddress(dll,"wglShareLists");
	awglUseFontBitmapsA=(void *) GetProcAddress(dll,"wglUseFontBitmapsA");
	awglUseFontBitmapsW=(void *) GetProcAddress(dll,"wglUseFontBitmapsW");




//awglUseFontBitmaps  = (void *) GetProcAddress(dll,"wglUseFontBitmaps");

	awglSwapLayerBuffers=(void *) GetProcAddress(dll,"wglSwapLayerBuffers");

/*
wglUseFontOutlines  ;


wglDescribeLayerPlane(HDC, int, int, UINT,
                                             LPLAYERPLANEDESCRIPTOR);
wglSetLayerPaletteEntries(HDC, int, int, int,
                                                 CONST COLORREF *);
wglGetLayerPaletteEntries(HDC, int, int, int,
                                                 COLORREF *);
wglRealizeLayerPalette(HDC, int, BOOL);
wglSwapLayerBuffers(HDC, UINT);


*/

return 1;

}


int GL_UnloadDll (void )
{

	if (GL_dll )
	{
		if (FreeLibrary ( GL_dll ))
		{
			GL_dll=NULL;
			return 1;
		}
		return 0;



	}
	else 
		return 0;


}

// GLU- Replacement-functions : taken from MESA 


static GLint bytes_per_pixel( GLenum format, GLenum type )
{
   GLint n, m;

   switch (format) {
      case GL_COLOR_INDEX:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
	 n = 1;
	 break;
      case GL_LUMINANCE_ALPHA:
	 n = 2;
	 break;
      case GL_RGB:
	 n = 3;
	 break;
      case GL_RGBA:
#ifdef GL_EXT_abgr
      case GL_ABGR_EXT:
#endif
	 n = 4;
	 break;
      default:
	 n = 0;
   }

   switch (type) {
      case GL_UNSIGNED_BYTE:	m = sizeof(GLubyte);	break;
      case GL_BYTE:		m = sizeof(GLbyte);	break;
      case GL_BITMAP:		m = 1;			break;
      case GL_UNSIGNED_SHORT:	m = sizeof(GLushort);	break;
      case GL_SHORT:		m = sizeof(GLshort);	break;
      case GL_UNSIGNED_INT:	m = sizeof(GLuint);	break;
      case GL_INT:		m = sizeof(GLint);	break;
      case GL_FLOAT:		m = sizeof(GLfloat);	break;
      default:			m = 0;
   }

   return n * m;
}



#define CEILING( A, B )  ( (A) % (B) == 0 ? (A)/(B) : (A)/(B)+1 )

#define dummy(J, K)


GLint APIENTRY GL_ScaleImage( GLenum format,
                              GLsizei widthin, GLsizei heightin,
                              GLenum typein, const void *datain,
                              GLsizei widthout, GLsizei heightout,
                              GLenum typeout, void *dataout )
{
   GLint components, i, j, k;
   GLfloat *tempin, *tempout;
   GLfloat sx, sy;
   GLint unpackrowlength, unpackalignment, unpackskiprows, unpackskippixels;
   GLint packrowlength, packalignment, packskiprows, packskippixels;
   GLint sizein, sizeout;
   GLint rowstride, rowlen;


   /* Determine number of components per pixel */
   switch (format) {
      case GL_COLOR_INDEX:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         components = 1;
	 break;
      case GL_LUMINANCE_ALPHA:
	 components = 2;
	 break;
      case GL_RGB:
      //case GL_BGR:
	 components = 3;
	 break;
      case GL_RGBA:
   //   case GL_BGRA:
#ifdef GL_EXT_abgr
      case GL_ABGR_EXT:
#endif
	 components = 4;
	 break;
      default:
	 return GL_INVALID_ENUM;
   }

   /* Determine bytes per input datum */
   switch (typein) {
      case GL_UNSIGNED_BYTE:	sizein = sizeof(GLubyte);	break;
      case GL_BYTE:		sizein = sizeof(GLbyte);	break;
      case GL_UNSIGNED_SHORT:	sizein = sizeof(GLushort);	break;
      case GL_SHORT:		sizein = sizeof(GLshort);	break;
      case GL_UNSIGNED_INT:	sizein = sizeof(GLuint);	break;
      case GL_INT:		sizein = sizeof(GLint);		break;
      case GL_FLOAT:		sizein = sizeof(GLfloat);	break;
      case GL_BITMAP:
	 /* not implemented yet */
      default:
	 return GL_INVALID_ENUM;
   }

   /* Determine bytes per output datum */
   switch (typeout) {
      case GL_UNSIGNED_BYTE:	sizeout = sizeof(GLubyte);	break;
      case GL_BYTE:		sizeout = sizeof(GLbyte);	break;
      case GL_UNSIGNED_SHORT:	sizeout = sizeof(GLushort);	break;
      case GL_SHORT:		sizeout = sizeof(GLshort);	break;
      case GL_UNSIGNED_INT:	sizeout = sizeof(GLuint);	break;
      case GL_INT:		sizeout = sizeof(GLint);	break;
      case GL_FLOAT:		sizeout = sizeof(GLfloat);	break;
      case GL_BITMAP:
	 /* not implemented yet */
      default:
	 return GL_INVALID_ENUM;
   }

   /* Get glPixelStore state */
   glGetIntegerv( GL_UNPACK_ROW_LENGTH, &unpackrowlength );
   glGetIntegerv( GL_UNPACK_ALIGNMENT, &unpackalignment );
   glGetIntegerv( GL_UNPACK_SKIP_ROWS, &unpackskiprows );
   glGetIntegerv( GL_UNPACK_SKIP_PIXELS, &unpackskippixels );
   glGetIntegerv( GL_PACK_ROW_LENGTH, &packrowlength );
   glGetIntegerv( GL_PACK_ALIGNMENT, &packalignment );
   glGetIntegerv( GL_PACK_SKIP_ROWS, &packskiprows );
   glGetIntegerv( GL_PACK_SKIP_PIXELS, &packskippixels );

   /* Allocate storage for intermediate images */
   tempin = (GLfloat *) malloc( widthin * heightin
			        * components * sizeof(GLfloat) );
   if (!tempin) {
      return GL_OUT_OF_MEMORY;
   }
   tempout = (GLfloat *) malloc( widthout * heightout
		 	         * components * sizeof(GLfloat) );
   if (!tempout) {
      free( tempin );
      return GL_OUT_OF_MEMORY;
   }


   /*
    * Unpack the pixel data and convert to floating point
    */

   if (unpackrowlength>0) {
      rowlen = unpackrowlength;
   }
   else {
      rowlen = widthin;
   }
   if (sizein >= unpackalignment) {
      rowstride = components * rowlen;
   }
   else {
      rowstride = unpackalignment/sizein
	        * CEILING( components * rowlen * sizein, unpackalignment );
   }

   switch (typein) {
      case GL_UNSIGNED_BYTE:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLubyte *ubptr = (GLubyte *) datain
	                   + i * rowstride
			   + unpackskiprows * rowstride
			   + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *ubptr++;
	    }
	 }
	 break;
      case GL_BYTE:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLbyte *bptr = (GLbyte *) datain
	                 + i * rowstride
			 + unpackskiprows * rowstride
			 + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *bptr++;
	    }
	 }
	 break;
      case GL_UNSIGNED_SHORT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLushort *usptr = (GLushort *) datain
	                    + i * rowstride
			    + unpackskiprows * rowstride
			    + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *usptr++;
	    }
	 }
	 break;
      case GL_SHORT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLshort *sptr = (GLshort *) datain
	                  + i * rowstride
			  + unpackskiprows * rowstride
			  + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *sptr++;
	    }
	 }
	 break;
      case GL_UNSIGNED_INT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLuint *uiptr = (GLuint *) datain
	                  + i * rowstride
			  + unpackskiprows * rowstride
			  + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *uiptr++;
	    }
	 }
	 break;
      case GL_INT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLint *iptr = (GLint *) datain
	                + i * rowstride
			+ unpackskiprows * rowstride
			+ unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = (GLfloat) *iptr++;
	    }
	 }
	 break;
      case GL_FLOAT:
	 k = 0;
	 for (i=0;i<heightin;i++) {
	    GLfloat *fptr = (GLfloat *) datain
	                  + i * rowstride
			  + unpackskiprows * rowstride
			  + unpackskippixels * components;
	    for (j=0;j<widthin*components;j++) {
               dummy(j, k);
	       tempin[k++] = *fptr++;
	    }
	 }
	 break;
      default:
	 return GL_INVALID_ENUM;
   }


   /*
    * Scale the image!
    */

   if (widthout > 1)
      sx = (GLfloat) (widthin-1) / (GLfloat) (widthout-1);
   else
      sx = (GLfloat) (widthin-1);
   if (heightout > 1)
      sy = (GLfloat) (heightin-1) / (GLfloat) (heightout-1);
   else
      sy = (GLfloat) (heightin-1);

/*#define POINT_SAMPLE*/
#ifdef POINT_SAMPLE
   for (i=0;i<heightout;i++) {
      GLint ii = i * sy;
      for (j=0;j<widthout;j++) {
	 GLint jj = j * sx;

	 GLfloat *src = tempin + (ii * widthin + jj) * components;
	 GLfloat *dst = tempout + (i * widthout + j) * components;

	 for (k=0;k<components;k++) {
	    *dst++ = *src++;
	 }
      }
   }
#else
   if (sx<1.0 && sy<1.0) {
      /* magnify both width and height:  use weighted sample of 4 pixels */
      GLint i0, i1, j0, j1;
      GLfloat alpha, beta;
      GLfloat *src00, *src01, *src10, *src11;
      GLfloat s1, s2;
      GLfloat *dst;

      for (i=0;i<heightout;i++) {
	 i0 = i * sy;
	 i1 = i0 + 1;
	 if (i1 >= heightin) i1 = heightin-1;
/*	 i1 = (i+1) * sy - EPSILON;*/
	 alpha = i*sy - i0;
	 for (j=0;j<widthout;j++) {
	    j0 = j * sx;
	    j1 = j0 + 1;
	    if (j1 >= widthin) j1 = widthin-1;
/*	    j1 = (j+1) * sx - EPSILON; */
	    beta = j*sx - j0;

	    /* compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
	    src00 = tempin + (i0 * widthin + j0) * components;
	    src01 = tempin + (i0 * widthin + j1) * components;
	    src10 = tempin + (i1 * widthin + j0) * components;
	    src11 = tempin + (i1 * widthin + j1) * components;

	    dst = tempout + (i * widthout + j) * components;

	    for (k=0;k<components;k++) {
	       s1 = *src00++ * (1.0-beta) + *src01++ * beta;
	       s2 = *src10++ * (1.0-beta) + *src11++ * beta;
	       *dst++ = s1 * (1.0-alpha) + s2 * alpha;
	    }
	 }
      }
   }
   else {
      /* shrink width and/or height:  use an unweighted box filter */
      GLint i0, i1;
      GLint j0, j1;
      GLint ii, jj;
      GLfloat sum, *dst;

      for (i=0;i<heightout;i++) {
	 i0 = i * sy;
	 i1 = i0 + 1;
	 if (i1 >= heightin) i1 = heightin-1; 
/*	 i1 = (i+1) * sy - EPSILON; */
	 for (j=0;j<widthout;j++) {
	    j0 = j * sx;
	    j1 = j0 + 1;
	    if (j1 >= widthin) j1 = widthin-1;
/*	    j1 = (j+1) * sx - EPSILON; */

	    dst = tempout + (i * widthout + j) * components;

	    /* compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
	    for (k=0;k<components;k++) {
	       sum = 0.0;
	       for (ii=i0;ii<=i1;ii++) {
		  for (jj=j0;jj<=j1;jj++) {
		     sum += *(tempin + (ii * widthin + jj) * components + k);
		  }
	       }
	       sum /= (j1-j0+1) * (i1-i0+1);
	       *dst++ = sum;
	    }
	 }
      }
   }
#endif


   /*
    * Return output image
    */

   if (packrowlength>0) {
      rowlen = packrowlength;
   }
   else {
      rowlen = widthout;
   }
   if (sizeout >= packalignment) {
      rowstride = components * rowlen;
   }
   else {
      rowstride = packalignment/sizeout
	        * CEILING( components * rowlen * sizeout, packalignment );
   }

   switch (typeout) {
      case GL_UNSIGNED_BYTE:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLubyte *ubptr = (GLubyte *) dataout
	                   + i * rowstride
			   + packskiprows * rowstride
			   + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *ubptr++ = (GLubyte) tempout[k++];
	    }
	 }
	 break;
      case GL_BYTE:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLbyte *bptr = (GLbyte *) dataout
	                 + i * rowstride
			 + packskiprows * rowstride
			 + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *bptr++ = (GLbyte) tempout[k++];
	    }
	 }
	 break;
      case GL_UNSIGNED_SHORT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLushort *usptr = (GLushort *) dataout
	                    + i * rowstride
			    + packskiprows * rowstride
			    + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *usptr++ = (GLushort) tempout[k++];
	    }
	 }
	 break;
      case GL_SHORT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLshort *sptr = (GLshort *) dataout
	                  + i * rowstride
			  + packskiprows * rowstride
			  + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *sptr++ = (GLshort) tempout[k++];
	    }
	 }
	 break;
      case GL_UNSIGNED_INT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLuint *uiptr = (GLuint *) dataout
	                  + i * rowstride
			  + packskiprows * rowstride
			  + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *uiptr++ = (GLuint) tempout[k++];
	    }
	 }
	 break;
      case GL_INT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLint *iptr = (GLint *) dataout
	                + i * rowstride
			+ packskiprows * rowstride
			+ packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *iptr++ = (GLint) tempout[k++];
	    }
	 }
	 break;
      case GL_FLOAT:
	 k = 0;
	 for (i=0;i<heightout;i++) {
	    GLfloat *fptr = (GLfloat *) dataout
	                  + i * rowstride
			  + packskiprows * rowstride
			  + packskippixels * components;
	    for (j=0;j<widthout*components;j++) {
               dummy(j, k+i);
	       *fptr++ = tempout[k++];
	    }
	 }
	 break;
      default:
	 return GL_INVALID_ENUM;
   }


   /* free temporary image storage */
   free( tempin );
   free( tempout );

   return 0;
}

GLint APIENTRY GL_Build2DMipmaps( GLenum target, GLint components,
                                  GLsizei width, GLsizei height, GLenum format,
                                  GLenum type, const void *data )
{
   GLint w, h, maxsize;
   void *image, *newimage;
   GLint neww, newh, level, bpp;
   int error;
   GLboolean done;
   GLint retval = 0;
   GLint unpackrowlength, unpackalignment, unpackskiprows, unpackskippixels;
   GLint packrowlength, packalignment, packskiprows, packskippixels;

   if (width < 1 || height < 1)
      return GL_INVALID_VALUE;

   //glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxsize );

   maxsize = glconfig.maxTextureSize;

   w = round_2( width );
   if (w>maxsize) {
      w = maxsize;
   }
   h = round_2( height );
   if (h>maxsize) {
      h = maxsize;
   }

   bpp = bytes_per_pixel( format, type );
   if (bpp==0) {
      /* probably a bad format or type enum */
      return GL_INVALID_ENUM;
   }

   /* Get current glPixelStore values */
   glGetIntegerv( GL_UNPACK_ROW_LENGTH, &unpackrowlength );
   glGetIntegerv( GL_UNPACK_ALIGNMENT, &unpackalignment );
   glGetIntegerv( GL_UNPACK_SKIP_ROWS, &unpackskiprows );
   glGetIntegerv( GL_UNPACK_SKIP_PIXELS, &unpackskippixels );
   glGetIntegerv( GL_PACK_ROW_LENGTH, &packrowlength );
   glGetIntegerv( GL_PACK_ALIGNMENT, &packalignment );
   glGetIntegerv( GL_PACK_SKIP_ROWS, &packskiprows );
   glGetIntegerv( GL_PACK_SKIP_PIXELS, &packskippixels );

   /* set pixel packing */
   glPixelStorei( GL_PACK_ROW_LENGTH, 0 );
   glPixelStorei( GL_PACK_ALIGNMENT, 1 );
   glPixelStorei( GL_PACK_SKIP_ROWS, 0 );
   glPixelStorei( GL_PACK_SKIP_PIXELS, 0 );

   done = GL_FALSE;

   if (w!=width || h!=height) {
      /* must rescale image to get "top" mipmap texture image */
      image = malloc( (w+4) * h * bpp );
      if (!image) {
	 return GL_OUT_OF_MEMORY;
      }
      error = GL_ScaleImage( format, width, height, type, data,
			     w, h, type, image );
      if (error) {
         retval = error;
         done = GL_TRUE;
      }
   }
   else {
      image = (void *) data;
   }

   level = 0;
   while (!done) {
      if (image != data) {
         /* set pixel unpacking */
         glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
         glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
         glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
         glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
      }

      glTexImage2D( target, level, components, w, h, 0, format, type, image );

      if (w==1 && h==1)  break;

      neww = (w<2) ? 1 : w/2;
      newh = (h<2) ? 1 : h/2;
      newimage = malloc( (neww+4) * newh * bpp );
      if (!newimage) {
	 return GL_OUT_OF_MEMORY;
      }

      error =  GL_ScaleImage( format, w, h, type, image,
			      neww, newh, type, newimage );
      if (error) {
         retval = error;
         done = GL_TRUE;
      }

      if (image!=data) {
	 free( image );
      }
      image = newimage;

      w = neww;
      h = newh;
      level++;
   }

   if (image!=data) {
      free( image );
   }

   /* Restore original glPixelStore state */
   glPixelStorei( GL_UNPACK_ROW_LENGTH, unpackrowlength );
   glPixelStorei( GL_UNPACK_ALIGNMENT, unpackalignment );
   glPixelStorei( GL_UNPACK_SKIP_ROWS, unpackskiprows );
   glPixelStorei( GL_UNPACK_SKIP_PIXELS, unpackskippixels );
   glPixelStorei( GL_PACK_ROW_LENGTH, packrowlength );
   glPixelStorei( GL_PACK_ALIGNMENT, packalignment );
   glPixelStorei( GL_PACK_SKIP_ROWS, packskiprows );
   glPixelStorei( GL_PACK_SKIP_PIXELS, packskippixels );

   return retval;
}



void APIENTRY GL_Perspective( GLdouble fovy, GLdouble aspect,
                              GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}





// some wrapping -functions whitch track the state :
// i`m tracking only  the important states 
#define TRACK_GL_STATE 1


#define MAX_TEX_UNITS 32

#if TRACK_GL_STATE
static int Cull_Face_Enabled =1;
static char Tex_Unit_Enabled [MAX_TEX_UNITS];
static int Tex_IDs [MAX_TEX_UNITS];
static int Active_Tex_Unit=0;
static int Blending_Enabled =0;
static int Alpha_Test_Enabled =0;
static int Polygon_Offset_Enabled=0;

static int 	DepthMask_State=GL_TRUE;
#endif 

void GL_Enable (int param )
{
	
#if TRACK_GL_STATE
	switch (param )
	{

	case GL_CULL_FACE:
		if (!Cull_Face_Enabled)
		{
			glEnable (param);
			Cull_Face_Enabled=1;
		}
		break;

	case GL_TEXTURE_2D :
		if (!Tex_Unit_Enabled [Active_Tex_Unit])
		{
			glEnable(param );
			Tex_Unit_Enabled [Active_Tex_Unit]=1;
		}
		break;

	case GL_BLEND :
		if (!Blending_Enabled )
		{
			glEnable (param);
			Blending_Enabled=1;
		}
		break;

	case GL_ALPHA_TEST:
		if (!Alpha_Test_Enabled)
		{
			glEnable (param );
			Alpha_Test_Enabled=1;
		}
		break;

	case GL_POLYGON_OFFSET:
		if (!Polygon_Offset_Enabled)
		{
			glEnable (param);
			Polygon_Offset_Enabled=1;
		}

	default :
		glEnable( param );
		break;

	}
#else 
	glEnable (param);

#endif 


}


void GL_Disable (int param )
{

#if TRACK_GL_STATE
	switch (param )
	{

	case GL_CULL_FACE:
		if (Cull_Face_Enabled)
		{
			glDisable (param);
			Cull_Face_Enabled=0;
		}
		break;

	case GL_TEXTURE_2D :
		if (Tex_Unit_Enabled [Active_Tex_Unit])
		{
			glDisable(param );
			Tex_Unit_Enabled [Active_Tex_Unit]=0;
		}
		break;

	case GL_BLEND :
		if (Blending_Enabled )
		{
			glDisable (param);
			Blending_Enabled=0;
		}
		break;

	case GL_ALPHA_TEST:
		if (Alpha_Test_Enabled)
		{
			glDisable (param );
			Alpha_Test_Enabled=0;
		}
		break;

	case GL_POLYGON_OFFSET:
		if (Polygon_Offset_Enabled)
		{
			glDisable (param);
			Polygon_Offset_Enabled=0;
		}

	default :
		glDisable( param );
		break;

	}
#else 
	glDisable (param);
#endif 



}

static int CullMode =0; 


void GL_CullFace (int mode )
{

#if TRACK_GL_STATE
	if (mode != CullMode )
	{
		glCullFace (mode );
		CullMode = mode ;

	}

#else
	glCullFace (mode );
#endif 



}

void GL_ActiveTextureARB (int param )
{
#if TRACK_GL_STATE
	int num = param - GL_TEXTURE0_ARB ;

	if (num != Active_Tex_Unit )
	{
		glActiveTextureARB ( param );
		Active_Tex_Unit = num ;
		
	}
#else 
	glActiveTextureARB (param );
#endif 

}

#if TRACK_GL_STATE
static float  Tex_Env_Mode[MAX_TEX_UNITS ];

static float Combine_Rgb_Ext[MAX_TEX_UNITS ] ;
static float Combine_Alpha_Ext[MAX_TEX_UNITS ] ;
static float Source0_Rgb_Ext[MAX_TEX_UNITS ];
static float Source1_Rgb_Ext[MAX_TEX_UNITS ];
static float Source2_Rgb_Ext[MAX_TEX_UNITS ];
static float Source0_Alpha_Ext[MAX_TEX_UNITS ];
static float Source1_Alpha_Ext[MAX_TEX_UNITS ];
static float Source2_Alpha_Ext[MAX_TEX_UNITS ];
static float Operand0_Rgb_Ext[MAX_TEX_UNITS ];
static float Operand1_Rgb_Ext[MAX_TEX_UNITS ];
static float Operand2_Rgb_Ext[MAX_TEX_UNITS ];
static float Operand0_Alpha_Ext[MAX_TEX_UNITS ];
static float Operand1_Alpha_Ext[MAX_TEX_UNITS ];
static float Operand2_Alpha_Ext[MAX_TEX_UNITS ];
static float Rgb_Scale_Ext[MAX_TEX_UNITS ];
static float Alpha_Scale [MAX_TEX_UNITS ]; 
// I think this will get important when using TEX_ENV_COMBINE 
#endif 

void GL_TexEnvf (int target ,int pname ,float param )
{
	
#if TRACK_GL_STATE
	switch ( target )
	{
		
	case GL_TEXTURE_ENV:

		switch (pname )
		{
		case GL_TEXTURE_ENV_MODE:
			if (param!= Tex_Env_Mode[Active_Tex_Unit])
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,param );	
				Tex_Env_Mode[Active_Tex_Unit] = param ;
			}
			break;
		case GL_COMBINE_RGB_EXT:
			if (param !=Combine_Rgb_Ext[Active_Tex_Unit])
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,param );
				Combine_Rgb_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_COMBINE_ALPHA_EXT:	
			if (param !=Combine_Alpha_Ext[Active_Tex_Unit])
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_COMBINE_ALPHA_EXT,param );
				Combine_Alpha_Ext[Active_Tex_Unit]=param ;
			}
			break;
        case GL_SOURCE0_RGB_EXT:
			if (param !=Source0_Rgb_Ext[Active_Tex_Unit])
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,param);
				Source0_Rgb_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_SOURCE1_RGB_EXT:
			if (param !=Source1_Rgb_Ext[Active_Tex_Unit])
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT,param);
				Source1_Rgb_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_SOURCE2_RGB_EXT:
			if (param !=Source2_Rgb_Ext[Active_Tex_Unit])
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_SOURCE2_RGB_EXT,param);
				Source2_Rgb_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_SOURCE0_ALPHA_EXT:
			if (param != Source0_Alpha_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_SOURCE0_ALPHA_EXT,param);
				Source0_Alpha_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_SOURCE1_ALPHA_EXT:
			if (param != Source1_Alpha_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_SOURCE1_ALPHA_EXT,param);
				Source1_Alpha_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_SOURCE2_ALPHA_EXT:
			if (param != Source2_Alpha_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_SOURCE2_ALPHA_EXT,param);
				Source2_Alpha_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_OPERAND0_RGB_EXT:
			if (param != Operand0_Rgb_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,param);
				Operand0_Rgb_Ext[Active_Tex_Unit] = param;
			}
			break;
        case GL_OPERAND1_RGB_EXT:	
			if (param != Operand1_Rgb_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,param);
				Operand1_Rgb_Ext[Active_Tex_Unit] = param;
			}
			break;
        case GL_OPERAND2_RGB_EXT:
			if (param != Operand2_Rgb_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_OPERAND2_RGB_EXT,param);
				Operand2_Rgb_Ext[Active_Tex_Unit] = param;
			}
			break;
        case GL_OPERAND0_ALPHA_EXT:
			if (param != Operand0_Alpha_Ext[Active_Tex_Unit] )
			{
				glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA_EXT,param );
				Operand0_Alpha_Ext[Active_Tex_Unit] = param ;
			}
			break;
        case GL_OPERAND1_ALPHA_EXT:
			if (param != Operand1_Alpha_Ext[Active_Tex_Unit] )
			{
				glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA_EXT,param );
				Operand1_Alpha_Ext[Active_Tex_Unit] = param ;
			}
			break;
        case GL_OPERAND2_ALPHA_EXT:
			if (param != Operand2_Alpha_Ext[Active_Tex_Unit] )
			{
				glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND2_ALPHA_EXT,param );
				Operand2_Alpha_Ext[Active_Tex_Unit] = param ;
			}
			break;
        case GL_RGB_SCALE_EXT:
			if (param != Rgb_Scale_Ext[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_RGB_SCALE_EXT,param );
				Rgb_Scale_Ext[Active_Tex_Unit]=param;
			}
			break;
        case GL_ALPHA_SCALE:
			if (param != Alpha_Scale[Active_Tex_Unit] )
			{
				glTexEnvf (GL_TEXTURE_ENV,GL_ALPHA_SCALE,param );
				Alpha_Scale[Active_Tex_Unit] =param;
			}
			break;

		default :
			glTexEnvf (target,pname,param);
			break;
		}

		break;


	default :
		break;

	}
#else 
	glTexEnvf (target,pname ,param );
#endif 


}

#if TRACK_GL_STATE
static char Color_Array_Enabled ;
static char Vertex_Array_Enabled ;
static char Tex_Coord_Array_Enabled [MAX_TEX_UNITS];

static int Active_Client_Tex_Unit =0;

static int Blendsrc=0;
static int Blenddst=0;
static int DepthFunc =0;

static int AlphaFunc=0;
static float Alpharef =0.0;

#endif 
void GL_BlendFunc (int src, int dst )
{
#if TRACK_GL_STATE
	if (Blendsrc!=src || Blenddst != dst )
	{
		glBlendFunc (src,dst);
		Blendsrc=src;
		Blenddst=dst;
	}
#else 
	glBlendFunc (src,dst);
#endif 

}

void GL_DepthFunc (int func )
{
#if TRACK_GL_STATE
	if (DepthFunc !=func)
	{
		glDepthFunc (func);
		DepthFunc=func;
	
	}
#else 
	glDepthFunc (func );
#endif 
}

void GL_AlphaFunc(int func, float ref )
{
#if TRACK_GL_STATE
	if (AlphaFunc!=func || Alpharef != ref )
	{
		glAlphaFunc ( func,ref);
		AlphaFunc=func;
		Alpharef=ref;
	}
#else 
	glAlphaFunc(func,ref);
#endif 
}

void GL_ClientActiveTextureARB (int par )
{

#if 	TRACK_GL_STATE
	int num = par - GL_TEXTURE0_ARB ;


	if (num!= Active_Client_Tex_Unit )
	{
		glClientActiveTextureARB(par);
		Active_Client_Tex_Unit = num;
	}
#else 
	glClientActiveTextureARB (par);
#endif

}


void GL_DisableClientState (int par )
{

#if TRACK_GL_STATE
	switch (par )
	{

	case GL_COLOR_ARRAY:

		if (Color_Array_Enabled)
		{
			glDisableClientState (GL_COLOR_ARRAY);
			Color_Array_Enabled=0;
		}
		break;

	case GL_VERTEX_ARRAY:
		if (Vertex_Array_Enabled)
		{
			glDisableClientState (GL_VERTEX_ARRAY);
			Vertex_Array_Enabled=0;
		}
		break;
	case GL_TEXTURE_COORD_ARRAY:
		if (Tex_Coord_Array_Enabled [Active_Client_Tex_Unit])
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			Tex_Coord_Array_Enabled[Active_Client_Tex_Unit]=0;
		}

		break;


	default :
		glDisableClientState (par);
		break;

	}

#else 
	glDisableClientState (par );

#endif 
}


void GL_EnableClientState (int par )
{
#if TRACK_GL_STATE

	switch (par )
	{

	case GL_COLOR_ARRAY:

		if (!Color_Array_Enabled)
		{
			glEnableClientState (GL_COLOR_ARRAY);
			Color_Array_Enabled=1;
		}
		break;

	case GL_VERTEX_ARRAY:
		if (!Vertex_Array_Enabled)
		{
			glEnableClientState (GL_VERTEX_ARRAY);
			Vertex_Array_Enabled=1;
		}
		break;
	case GL_TEXTURE_COORD_ARRAY:
		if (!Tex_Coord_Array_Enabled [Active_Client_Tex_Unit])
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			Tex_Coord_Array_Enabled[Active_Client_Tex_Unit]=1;
		}

		break;


	default :
		glEnableClientState (par);
		break;

	}
#else 
	glEnableClientState (par );
#endif 

}
void GL_DepthMask ( GLboolean state )
{

#if TRACK_GL_STATE
	if (state==DepthMask_State)
		return ;

	glDepthMask (state );


	DepthMask_State =state ;

#else 
	glDepthMask (state );
#endif 
}



void GL_BindTexture (int par1,int id )
{
#if TRACK_GL_STATE
		if (id != Tex_IDs [Active_Tex_Unit ])
		{
			
			glBindTexture (GL_TEXTURE_2D,id);
			Tex_IDs [Active_Tex_Unit ] = id;
		}
#else 
	glBindTexture (GL_TEXTURE_2D,id);
#endif
}



void * GL_GetProcAddress (const char * str )
{
	if (awglGetProcAddress)
	{
		return awglGetProcAddress(str);
	}
	else
	{
		return GetProcAddress(GL_dll,str);
	}
	
}

// TODO !!!
void GL_Set_Gamma (byte red[256],byte green[256],byte blue[256])
{





}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);








static const vid_mode_t vid_modes[12] = {
	{320, 240},
	{400, 300},
	{512, 384},
	{640, 480},
	{800, 600},
	{960, 720},
	{1024, 768},
	{1152, 864},
	{1280, 1024},
	{1600, 1200},
	{2048, 1536},
	{856, 380}
};


// works quite fine :

int WIN_ChangeResolution(int w, int  h, int  bitdepth ) {
	  DEVMODE mode;
	  int flags=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	  int res;
	
	mode.dmSize=sizeof (mode);
	mode.dmBitsPerPel=bitdepth;
	mode.dmPelsWidth=w;
	mode.dmPelsHeight=h;
	mode.dmDisplayFlags=0;
	mode.dmFields=flags;




	res=ChangeDisplaySettingsEx(
	  NULL,
	  &mode,
	  NULL,
	  CDS_FULLSCREEN,   
	  NULL
	);
 

	if (res==DISP_CHANGE_SUCCESSFUL)
	{
		winX=w;
		winY=h;
		return 1;
	}

	return 0;



}


int WIN_Reset_DisplaySettings (void )
{
	int res;

	res=ChangeDisplaySettingsEx(
	  NULL,
	  NULL,
	  NULL,
	  CDS_FULLSCREEN,   
	  NULL
	);

	return (res==DISP_CHANGE_SUCCESSFUL);






}



// put somewhere else 

int WIN_CreateWindow ( HINSTANCE inst ,int nCmdShow)
{

	WNDCLASSEX wcex;
	vid_mode_t mode;

	memset (&wcex,0,sizeof (WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_OWNDC;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= inst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "AFTERSHOCK";
	wcex.hIconSm		= NULL;

	if (! RegisterClassEx(&wcex))
	{
		Con_Printf ("Could not register Window Class \n");
		return 0;
	}
	else 
		Con_Printf ("... registered window class\n");



	hInst = inst; 

	mode= vid_modes[r_mode->integer];

// Fullscreen support works ,
// it`s just disabled cause it makes Problems when debugging 
   if (1)//!r_fullscreen->integer)
   {
   hWnd = CreateWindow("AFTERSHOCK", "Aftershock", WS_CLIPSIBLINGS | WS_CAPTION,
      CW_USEDEFAULT, 0, mode.width,mode.height , NULL, NULL, hInst, NULL);

		glconfig.isFullscreen=0;
		winX=mode.width;
		winY=mode.height;
   }
   else 
   {
	   WIN_Reset_DisplaySettings ();
		glconfig.isFullscreen=1;
		hWnd = CreateWindowEx( WS_EX_TOPMOST , 
                        "Aftershock", "Aftershock", 
                        WS_POPUP | WS_CLIPSIBLINGS,
                        0, 0, mode.width, mode.height,
                        NULL, NULL, hInst, NULL);

		
		Con_Printf ("...calling CDS :");
		if (!WIN_ChangeResolution (mode.width,mode.height,r_colorbits->integer ? r_colorbits->integer : 16 ))
		{
			Con_Printf ("failed\n");
			return 0;
		}
		else
			Con_Printf ("ok\n");

   }
   if( !hWnd ) 
   {
		Con_Printf ("Could not Create Window \n");
		return 0;
   }

   ShowWindow( hWnd, nCmdShow );
   UpdateWindow( hWnd );

   return 1;



}


int WIN_Destroy_Window (void )
{

	if (hWnd)
	{

		if (hRC )
		{
			awglMakeCurrent( NULL, NULL );
			awglDeleteContext( hRC );
		
			hRC=NULL;
		}


		if (dc )
		{
			ReleaseDC(hWnd,dc);

			dc= NULL;


		}

		
		DestroyWindow ( hWnd);

		hWnd=NULL;



		if (!UnregisterClass("AFTERSHOCK",hInst))
		return 0;


		return 1;

	}
	else 
	{
		Error ("WIN_Destroy_Window : no Window present !");


	}

	return 0;

}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x,y;
	static int oldx=-1,oldy=-1;


	switch( message ) 
	{
		
				
		case WM_PAINT:
			
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			Sys_Keyboard_Event (wParam,1);
			break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
			Sys_Keyboard_Event (wParam,0);
			break;


			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			Key_MouseDown(wParam);
			break;

			case WM_RBUTTONUP:
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			Key_MouseUp(wParam);
			break;

			case WM_MOUSEMOVE:

			x=LOWORD(lParam);
			y=HIWORD(lParam);

			Key_Update_MousePosition (x,y );		
			break;
		
		case WM_COMMAND:	
				   return DefWindowProc( hWnd, message, wParam, lParam );	
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
   }
   return 0;
}



#define MAX_PFDS 128
static int GL_ChoosePFD( int colorbits, int depthbits, int stencilbits )
{
	PIXELFORMATDESCRIPTOR pfdlist[MAX_PFDS];
	PIXELFORMATDESCRIPTOR *pfd;
	PIXELFORMATDESCRIPTOR *bestpfd;
	int numPFDs;
	int i;
	unsigned int flags;
	int best=-1;

	flags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;
	if( r_stereo->integer ) {
		Con_Printf( "...attempting to use stereo\n" );
		flags |= PFD_STEREO;
	}

	Con_Printf( "...GL_ChoosePFD( %d, %d, %d )\n", colorbits, depthbits, stencilbits );

	/* get number of PFDs */
	numPFDs = DescribePixelFormat( dc, 0, 0, NULL );

	if(numPFDs > MAX_PFDS) {
		Con_Printf( "...numPFDs > MAX_PFDS (%d > %d)\n", numPFDs, MAX_PFDS );
		numPFDs = MAX_PFDS;
	}
	Con_Printf( "...%d PFDs found\n", numPFDs );

	/* fill PFD list */
	for( i=0; i<numPFDs; i++ ) {
		DescribePixelFormat( dc, i+1, sizeof(PIXELFORMATDESCRIPTOR), &pfdlist[i] );
	}

	/* choose PFD */
	best = 0;
	for( i=0; i<numPFDs; i++ ) {
		pfd = &pfdlist[i];
		if( pfd->dwFlags & PFD_GENERIC_FORMAT && !(pfd->dwFlags & PFD_GENERIC_ACCELERATED) && !r_allowSoftwareGL->integer ) {
			continue;
		}

		if( pfd->iPixelType != PFD_TYPE_RGBA ) {
			
			continue;
		}

		if( (pfd->dwFlags & flags) != flags ) {
			continue;
		}

		if( pfd->cColorBits >= colorbits && pfd->cDepthBits >= depthbits && pfd->cStencilBits >= stencilbits )
			if(!best) {
				best = i+1;
			} else {
				bestpfd = &pfdlist[best-1];
				if(pfd->cColorBits < bestpfd->cColorBits && pfd->cDepthBits < bestpfd->cDepthBits && pfd->cStencilBits < bestpfd->cStencilBits)
					best = i+1;
			}
	}

	bestpfd = & pfdlist[best];

	if (!(bestpfd->dwFlags & PFD_GENERIC_FORMAT) )
	{
		Con_Printf("...hardware acceleration found\n");
	}
	else
	{
		Con_Printf("...hardware acceleration not found\n");
	}

	return( best );
}



static int IsExtensionSupported(char *ext)
{

	char *start=glconfig.extensions_string;
	char *where,*terminator;

	for(;;)
	{

		where=strstr(start,ext);
		if (!where)
			break;
		terminator=where+strlen(ext);
		if (where==start || *(where-1)==' ')
			if (*terminator==' ' || *terminator=='\0')
				return 1;
			start=terminator;
	}

	return 0;
}


void  GetGlConfig(glconfig_t * config)
{
	memcpy (config,&glconfig,sizeof (glconfig_t ));
}


int Init_OpenGL ( void )
{

	PIXELFORMATDESCRIPTOR pfd;
	int iFormat=-1;
	HINSTANCE glide ;
	HINSTANCE dll ;
	char * dllname ;


	if (opengl_initialized)
		return 1;

	Con_Printf("Initializing OpenGl subsystem\n");



	Con_Printf("...initializing AGL\n");
	glide = LoadLibrary( "glide2x.dll" );
	if( !glide )
		glide = LoadLibrary( "glide3x.dll" );
	if( glide ) {
		FreeLibrary( glide );

		dll =LoadLibrary (_3DFX_DRIVER_NAME);
		if (dll)
		{
			dllname = _3DFX_DRIVER_NAME;
			glconfig.driverType=GLDRV_VOODOO;
			FreeLibrary ( dll );
		}
		else 
		{
			glconfig.driverType=GLDRV_ICD;
			dllname =OPENGL_DRIVER_NAME;
		}

	}
	else
	{
		glconfig.driverType=GLDRV_ICD;
		dllname =OPENGL_DRIVER_NAME;

	}
	
	Con_Printf("...calling Load Library ('%s');",dllname);
	if (!GL_LoadDll ( dllname))
	{
		Con_Printf ("failed\n");
		return 0;
	}
	else 
		Con_Printf ("succeded\n");


	if (! WIN_CreateWindow ( hInst ,nCmdShow))
		return 0;


	Con_Printf ("\n");
	Con_Printf ("Initializing OpenGL driver\n");


	Con_Printf ("...getting DC:");
	
	dc = GetDC( hWnd );

	if (!dc)
	{
		Con_Printf("failed\n");
		return 0;
	}
	else
		Con_Printf("succeded\n");


	iFormat=GL_ChoosePFD( 16, 16, 8 );



	if (iFormat<0)
	{
		return 0;
	}
	
	if (!SetPixelFormat( dc, iFormat, &pfd ))
	{
		Con_Printf ("Init_OpenGL : Could not Set PixelFormat \n");
		return 0;
	}
	else 
	{
		Con_Printf("...PIXELFORMAT %i selected\n",iFormat);
	}

		
	Con_Printf("...creating GL context :");
	hRC = awglCreateContext( dc );

	if (!hRC)
	{
		Con_Printf ("failed\n");
		return 0;
	}
	else
		Con_Printf("succeded\n");

	Con_Printf("...making context current:");
	if (!awglMakeCurrent( dc, hRC ))
	{
		Con_Printf ("failed\n");
		return 0;
	}
	else
		Con_Printf ("succeded\n");

	if (1)  // TODO : Make function !!!
	{
		const char *vendor=glGetString(GL_VENDOR);
		const char *ext=glGetString(GL_EXTENSIONS);
		const char *renderer=glGetString(GL_RENDERER);
		const char *version=glGetString(GL_VERSION);

		A_strncpyz(glconfig.vendor_string,vendor,MAX_STRING_CHARS);
		A_strncpyz(glconfig.extensions_string,ext,MAX_STRING_CHARS);
		A_strncpyz(glconfig.renderer_string,renderer,MAX_STRING_CHARS);
		A_strncpyz(glconfig.version_string,version,MAX_STRING_CHARS);



		if (r_allowExtensions->integer)
		{
			Con_Printf("Initializing OpenGl extensions\n");

			// this is internal
			if (IsExtensionSupported("GL_ARB_texture_compression"))
			{
				gl_ext_info._GL_ARB_texture_compression=1;

				glCompressedTexImage3DARB=GL_GetProcAddress("glCompressedTexImage3DARB");
                glCompressedTexImage2DARB=GL_GetProcAddress("glCompressedTexImage2DARB");
				glCompressedTexImage1DARB=GL_GetProcAddress("glCompressedTexImage1DARB");
				glCompressedTexSubImage3DARB=GL_GetProcAddress("glCompressedTexSubImage3DARB");
				glCompressedTexSubImage2DARB=GL_GetProcAddress("glCompressedTexSubImage2DARB");
				glCompressedTexSubImage1DARB=GL_GetProcAddress("glCompressedTexSubImage1DARB");
				glGetCompressedTexImageARB=GL_GetProcAddress("glGetCompressedTexImageARB");
                                 
			}
			else
			{
				gl_ext_info._GL_ARB_texture_compression=0;
				
				glCompressedTexImage3DARB=0;
                glCompressedTexImage2DARB=0;
				glCompressedTexImage1DARB=0;
				glCompressedTexSubImage3DARB=0;
				glCompressedTexSubImage2DARB=0;
				glCompressedTexSubImage1DARB=0;
				glGetCompressedTexImageARB=0;
			}

			if (IsExtensionSupported("GL_S3_s3tc"))
			{
				Con_Printf("...using GL_S3_s3tc\n");
				gl_ext_info._GL_S3_s3tc=1;
			}
			else
			{
				Con_Printf("...GL_S3_s3tc not found\n");
				gl_ext_info._GL_S3_s3tc=0;

			}
			if (IsExtensionSupported("GL_3DFX_texture_compression_FXT1"))
			{

				Con_Printf("...using GL_3DFX_texture_compression_FXT1\n");
				gl_ext_info._GL_3DFX_texture_compression_FXT1=1;

			}
			else
			{
				Con_Printf("...GL_3DFX_texture_compression_FXT1 not found\n");
				gl_ext_info._GL_3DFX_texture_compression_FXT1=0;
			}

			if (IsExtensionSupported("GL_EXT_texture_env_add"))
			{
				Con_Printf ("...using GL_EXT_texture_env_add\n");
				gl_ext_info._TexEnv_Add=1;
				glconfig.textureEnvAddAvailable=1;
			}
			else
			{
				Con_Printf ("... GL_EXT_texture_env_add not found\n");
				gl_ext_info._TexEnv_Add=0;
				glconfig.textureEnvAddAvailable=0;
			}
			if (IsExtensionSupported ("GL_EXT_texture_env_combine"))
			{
				Con_Printf("...ignoring GL_EXT_texture_env_combine\n");
				gl_ext_info._TexEnv_Combine=1;
			}
			else
			{
				Con_Printf("...GL_EXT_texture_env_combine not found\n");
				gl_ext_info._TexEnv_Combine=0;
			}
			if (IsExtensionSupported ("GL_NV_texture_env_combine4"))
			{
				Con_Printf("...ignoring GL_NV_texture_env_combine4\n");
				gl_ext_info._TexEnv_Combine4=1;
			}
			else
			{
				Con_Printf("...GL_NV_texture_env_combine4 not found\n");
				gl_ext_info._TexEnv_Combine4=0;
			}
			if (IsExtensionSupported ("GL_NV_register_combiners"))
			{
				Con_Printf("...ignoring GL_NV_register_combiners\n");
				gl_ext_info._GL_NV_register_combiners=1;

				
				glCombinerParameterfvNV=GL_GetProcAddress("glCombinerParameterfvNV");
				glCombinerParameterivNV=GL_GetProcAddress("glCombinerParameterivNV");

				glCombinerParameterfNV=GL_GetProcAddress("glCombinerParameterfNV");

				glCombinerParameteriNV=GL_GetProcAddress("glCombinerParameteriNV");

				glCombinerInputNV=GL_GetProcAddress("glCombinerInputNV");
	
				glCombinerOutputNV=GL_GetProcAddress("glCombinerOutputNV");
				glFinalCombinerInputNV=GL_GetProcAddress("glFinalCombinerInputNV");

				glGetCombinerInputParameterfvNV=GL_GetProcAddress("glGetCombinerInputParameterfvNV");

				glGetCombinerInputParameterivNV=GL_GetProcAddress("glGetCombinerInputParameterivNV");

				glGetCombinerOutputParameterfvNV=GL_GetProcAddress("glGetCombinerOutputParameterfvNV");

				glGetCombinerOutputParameterivNV=GL_GetProcAddress("glGetCombinerOutputParameterivNV");

				glGetFinalCombinerInputParameterfvNV=GL_GetProcAddress("glGetFinalCombinerInputParameterfvNV");

				glGetFinalCombinerInputParameterivNV=GL_GetProcAddress("glGetFinalCombinerInputParameterivNV");

			}
			else
			{
				Con_Printf("... GL_NV_register_combiners not found\n");
				gl_ext_info._GL_NV_register_combiners=0;
			}

			if (IsExtensionSupported ("WGL_EXT_swap_control"))
			{
				Con_Printf ("...using WGL_EXT_swap_control\n");
				gl_ext_info._WGL_swap_control=1;

				wglSwapIntervalEXT= (WGLSETSWAPINTERVALPROC) GL_GetProcAddress ("wglSwapIntervalEXT");
				wglGetSwapIntervalEXT = (WGLGETSWAPINTERVALPROC) GL_GetProcAddress("wglGetSwapIntervalEXT");


				if (!wglSwapIntervalEXT || !wglGetSwapIntervalEXT)
					return 0;
			}
			else 
			{
				Con_Printf ("...WGL_EXT_swap_control not found\n");
				gl_ext_info._WGL_swap_control=0;

				wglSwapIntervalEXT=NULL;
				wglGetSwapIntervalEXT=NULL;
			}


			if (IsExtensionSupported("GL_ARB_multitexture"))
			{
				Con_Printf ("...using GL_ARB_multitexture\n");
				gl_ext_info._ARB_Multitexture=1;
				glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&glconfig.maxActiveTextures);

				// Load :
				glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)GL_GetProcAddress("glMultiTexCoord2fARB");
				glMultiTexCoord2fvARB =(PFNGLMULTITEXCOORD2FVARBPROC)GL_GetProcAddress("glMultiTexCoord2fvARB");  
				glActiveTextureARB=(PFNGLACTIVETEXTUREARBPROC) GL_GetProcAddress("glActiveTextureARB");
				glClientActiveTextureARB =(PFNGLCLIENTACTIVETEXTUREARBPROC) GL_GetProcAddress("glClientActiveTextureARB");

				if (!glMultiTexCoord2fARB || !glMultiTexCoord2fARB || !glMultiTexCoord2fvARB || !glActiveTextureARB || !glClientActiveTextureARB)
					return 0;


			}
			else
			{
				Con_Printf ("...GL_ARB_multitexture not found\n");
				gl_ext_info._ARB_Multitexture=0;
				glconfig.maxActiveTextures=1;

				glMultiTexCoord2fARB=NULL;
				glMultiTexCoord2fvARB=NULL;
				glActiveTextureARB=NULL;
				glClientActiveTextureARB=NULL;
			}


			if (IsExtensionSupported("GL_EXT_compiled_vertex_array"))
			{
				Con_Printf ("...using GL_EXT_compiled_vertex_array\n");
				gl_ext_info._CompiledVertex_Arrays=1;
				
				// Load:

				glLockArraysEXT=(PFNGLLOCKARRAYSEXTPROC) GL_GetProcAddress("glLockArraysEXT");
				glUnlockArraysEXT=(PFNGLUNLOCKARRAYSEXTPROC) GL_GetProcAddress("glUnlockArraysEXT");
		
				if(!glLockArraysEXT || !glUnlockArraysEXT)
					return 0;

			}
			else
			{
				Con_Printf ("...GL_EXT_compiled_vertex_array not found\n");
				gl_ext_info._CompiledVertex_Arrays=0;

				glLockArraysEXT=NULL;
				glUnlockArraysEXT=NULL;
			}	



			if (IsExtensionSupported("WGL_3DFX_gamma_control"))
			{
				Con_Printf ("...using WGL_3DFX_gamma_control\n");
				gl_ext_info._WGL_3DFX_gamma=1;

				// TODO !
			}
			else 
			{
				Con_Printf ("...WGL_3DFX_gamma_control not found\n");
				gl_ext_info._WGL_3DFX_gamma=0;
			}


			Con_Printf("\n");

		}



	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&glconfig.maxTextureSize);
	glconfig.vidHeight=winY;
	glconfig.vidWidth=winX;
	glconfig.displayFrequency=GetDeviceCaps( dc, VREFRESH );

	Con_Printf ("GL_VENDOR: %s\n",glconfig.vendor_string);
	Con_Printf ("GL_RENDERER: %s\n",glconfig.renderer_string);
	Con_Printf ("GL_VERSION: %s\n",glconfig.version_string);
	Con_Printf ("GL_EXTENSIONS: %s\n",glconfig.extensions_string);
	Con_Printf ("GL_MAX_TEXTURE_SIZE: %i\n",glconfig.maxTextureSize);
	Con_Printf ("GL_MAX_ACTIVE_TEXTURES_ARB: %i\n",glconfig.maxTextureSize);


	Con_Printf ("\n");
	Con_Printf ("PIXELFORMAT:color(%i -bits) Z (%i bit) stencil (%i Bits)\n",(int)pfd.cColorBits,(int)pfd.cDepthBits,(int)pfd.cStencilBits);
	Con_Printf ("MODE: %i, %i * %i",r_mode->integer,glconfig.vidWidth,glconfig.vidHeight);
	if (r_fullscreen->integer)
	{
		Con_Printf(" fullscreen");
	}
	else
	{
		Con_Printf(" windowed");
	}
	if (glconfig.displayFrequency)
	{
		Con_Printf(" hz:%i\n",glconfig.displayFrequency);
	}
	else
	{
		Con_Printf(" hz:N/A\n");
	}

	Con_Printf("CPU: TODO !!!\n");
	Con_Printf("rendering primitives: single glDrawElements\n"); // TODO !!!
	Con_Printf("texturemode: %s\n",r_textureMode->string);
	Con_Printf("picmip:%i\n",r_picmip->integer);
	Con_Printf("texture bits:%i\n",r_texturebits->integer);


	Con_Printf("multitexture: ");
	if (!r_ext_multitexture->integer)
	{
		Con_Printf("disabled\n");
	}
	else
	{
		Con_Printf("enabled\n");
	}

	Con_Printf ("compiled vertex arrays: ");
	if (r_ext_compiled_vertex_array->integer)
	{
		Con_Printf("enabled\n");
	}
	else
	{
		Con_Printf("disabled\n");
	}

	Con_Printf("texenv add: ");
	if (r_ext_texture_env_add->integer)
	{
		Con_Printf("enabled\n");
	}
	else
	{
		Con_Printf("disabled\n");
	}

	Con_Printf("compressed textures: ");
	if (r_ext_compress_textures->integer)
	{
		Con_Printf("enabled\n");
	}
	else
	{
		Con_Printf("disabled\n");
	}



	// Reset the States :
#if TRACK_GL_STATE
	Cull_Face_Enabled =1;
	Active_Tex_Unit=0;
	Blending_Enabled =0;
	Alpha_Test_Enabled =0;
	memset (Tex_Unit_Enabled,0,MAX_TEX_UNITS);
	memset (Tex_IDs,0,MAX_TEX_UNITS*sizeof (int ));

	Color_Array_Enabled=0;
	Vertex_Array_Enabled=0;

	memset (Tex_Coord_Array_Enabled,0,MAX_TEX_UNITS);

	Active_Client_Tex_Unit =0;

	Blendsrc=0;
	Blenddst =0;

	DepthFunc =0;
	AlphaFunc=0;
	Alpharef =0;

	memset ( Tex_Env_Mode,0,sizeof (float ) *MAX_TEX_UNITS );

	memset ( Combine_Rgb_Ext,0,sizeof (float ) * MAX_TEX_UNITS ) ;
	memset ( Combine_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS ) ;
 	memset (Source0_Rgb_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Source1_Rgb_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Source2_Rgb_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Source0_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Source1_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Source2_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Operand0_Rgb_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Operand1_Rgb_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Operand2_Rgb_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Operand0_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Operand1_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Operand2_Alpha_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Rgb_Scale_Ext,0,sizeof (float ) *MAX_TEX_UNITS );
 	memset (Alpha_Scale ,0,sizeof (float ) *MAX_TEX_UNITS ); 


#endif 

	opengl_initialized=1;

	return 1;



}

int Restart_Opengl (void )
{

	if (Shutdown_OpenGL () && Init_OpenGL())
		return 1;

	Error ("Could not restart OpenGL ");

	return 0;


}

int Shutdown_OpenGL (void )
{

	
	if (!opengl_initialized) 
		return 1;

	if (!WIN_Destroy_Window ())
		return 0;
		
	if (!GL_UnloadDll())
		return 0;
	
	if (!WIN_Reset_DisplaySettings ())
		return 0;

	opengl_initialized=0;

	return 1;


}