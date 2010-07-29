
#ifndef _SURFACE_DISPLAY_H
#define _SURFACE_DISPLAY_H

#include "DisplayObject.h"
#include <GoTools/geometry/SplineSurface.h>
#include <GL/glut.h>
#include <vector>
class CurveDisplay;

using namespace std;
using namespace Go;

class SurfaceDisplay : public DisplayObject {
	
	public:
		SplineSurface *surf;

		SurfaceDisplay(SplineSurface *surf, bool clean=false) ;
		~SurfaceDisplay();
		void setFaceIndex(int i);
		void tesselate(int *n);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		void paintMouseAreas(float r, float g, float b);
		bool isAtPosition(int x, int y);
		double* parValueAtPosition(int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		DISPLAY_CLASS_TYPE classType() { return SURFACE; }
		void printDebugInfo();
		void print(ostream *out);

		void initMouseMasks() ;
		void setMaskPos(int x, int y, bool value) ;
			
	
	private:

		int resolution[2];
		int triangle_count;
		int width;
		int height;
		int faceIndex;
		GLdouble *positions;
		GLdouble *normals;
		GLdouble *param_values;
		GLuint *triangle_strip;
		GLfloat *xi_buffer;
		GLfloat *eta_buffer;
		bool selected;
		vector<CurveDisplay*> knot_lines;

		void myGridEvaluator(int n1, int n2, vector<double>& pts, vector<double>& nor, vector<double>& par_u, vector<double>& par_v);
};

#endif
