# Choose glut or glx interface:
# GLUT
MAINSRC = uiglut.c
MAINLIB = -lglut
# GLX
#MAINSRC = uiglx.c
#MAINLIB =

SRC = util.c bsp.c globals.c vec.c render.c tex.c lightmap.c mesh.c \
      unzip.c pak.c shader.c renderback.c skybox.c entity.c md3.c mapent.c \
      uicommon.c $(MAINSRC)

OBJ = $(SRC:.c=.o)

# Solaris
INC = -I/usr/openwin/include -I/cs/include/glut -I/cs/include/jpeg -I/cs/include
LIB = -L/usr/openwin/lib -L/cs/lib -R/usr/openwin/lib:/cs/lib
OPT = -g
DEFS = -DASHOCK_BIG_ENDIAN -DSUN_OGL_NO_VERTEX_MACROS
CC = cc

# AIX
INC = -I/cs/src/glut/include -I/cs/include/jpeg -I/cs/include
LIB = -L/cs/lib -L/cs/src/glut/lib/aix
OPT = -g
DEFS = -DASHOCK_BIG_ENDIAN
CC = xlc

# SGI
INC = -I/cs/include
LIB = -L/cs/src/glut/lib/sgi -L/cs/src/zlib/zlib-1.1.3 -L/cs/lib
OPT = -g
DEFS = -DASHOCK_BIG_ENDIAN
CC = cc

# Linux
INC = -I/usr/X11R6/include
LIB = -L/usr/X11R6/lib
OPT = -g -Wall
DEFS = 
CC = gcc

# You shouldn't have to change anything below this line

CFLAGS = $(INC) $(OPT) $(DEFS)

# You might have to change GLU to MesaGLU depending on your Mesa version
GLLIBS = $(LIB) $(MAINLIB) -lGL -lGLU -lX11 -lXi -lXmu -lXext
LIBS = $(GLLIBS) -lm -ljpeg -lz

all: aftershock

.c.o:
	$(CC) $(CFLAGS) -c $<

aftershock: $(OBJ)
	$(CC) $(OPT) -o $@ $^ $(LIBS)

clean:
	rm -f aftershock *.o
