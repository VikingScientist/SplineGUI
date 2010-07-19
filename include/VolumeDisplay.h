
#ifndef _VOLUME_DISPLAY_H
#define _VOLUME_DISPLAY_H

#include "DisplayObject.h"
#include <GoTools/trivariate/SplineVolume.h>
#include "ActiveObject.h"
#include <GL/glut.h>
#include <vector>
class CurveDisplay;
class SurfaceDisplay;

using namespace std;
using namespace Go;

class VolumeDisplay : public DisplayObject, ActiveObject {
	
	public:
		SplineVolume *volume;

		VolumeDisplay(SplineVolume *volume);
		~VolumeDisplay();
		void tesselate(int *n);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		bool isAtPosition(int x, int y);
		SurfaceDisplay* getSurfaceAt(int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		DISPLAY_CLASS_TYPE classType() { return VOLUME; }
		void printDebugInfo();
		void print(ostream *out);
		void setDrawControlMesh(bool draw);
			
	
	private:

		bool draw_contol_mesh;
		int resolution[3];
		int triangle_count;
		int line_count;
		int cp_count;
		int width;
		int height;
		GLdouble *cp_pos;
		GLuint *cp_lines;
		GLdouble *positions;
		GLdouble *normals;
		GLdouble *param_values;
		GLuint *triangle_strip;
		GLfloat *wallbuffer;
		vector<SurfaceDisplay*> walls;
};

#endif
