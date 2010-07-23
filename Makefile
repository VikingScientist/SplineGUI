CC = g++
PROG = bin/Fenris

GOTLIB     = -L/usr/local/lib/GoTools -lGoToolsCore -lGoTrivariate
GLLIB      = -lglut

CFLAGS = -Wall -g 

SRCS = src/main.cpp \
       src/Camera.cpp \
       src/OrthoProjection.cpp \
	   src/PointDisplay.cpp \
	   src/CurveDisplay.cpp \
	   src/SurfaceDisplay.cpp \
	   src/VolumeDisplay.cpp \
	   src/DisplayObjectSet.cpp \
	   src/Button.cpp \
	   src/Fenris.cpp \
	   src/CurvePoint.cpp

LIBS = $(GLLIB) $(GOTLIB) -Iinclude

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
