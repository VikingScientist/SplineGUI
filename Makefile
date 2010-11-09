CC = g++
PROG = bin/SplineGUI

GOTLIB     = -L/usr/local/lib/GoTools -lGoToolsCore -lGoTrivariate
GLLIB      = -lglut -lGLU

CFLAGS = -Wall -g 

MAIN = src/main.cpp 

SRCS = src/Camera.cpp \
       src/OrthoProjection.cpp \
	   src/PointDisplay.cpp \
	   src/CurveDisplay.cpp \
	   src/SurfaceDisplay.cpp \
	   src/VolumeDisplay.cpp \
	   src/DisplayObjectSet.cpp \
	   src/Button.cpp \
	   src/SplineGUI.cpp \
	   src/CurvePoint.cpp

LIBS = $(GLLIB) $(GOTLIB) -Iinclude

all: $(PROG)

$(PROG):	$(SRCS) $(MAIN)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(MAIN) $(LIBS)

libs: $(SRCS)
	$(CC) $(CFLAGS) -c -fPIC $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
