
#ifndef _SURFACE_DISPLAY_H
#define _SURFACE_DISPLAY_H

#include "DisplayObject.h"
#include <GoTools/geometry/SplineSurface.h>
#include <GoTools/geometry/SplineCurve.h>
#include <GL/glut.h>
#include <vector>
class CurveDisplay;

class SurfaceDisplay : public DisplayObject {
	
	public:
		Go::SplineSurface *surf;

		SurfaceDisplay(Go::SplineSurface *surf, bool clean=false) ;
		~SurfaceDisplay();
		void setFaceIndex(int i);
		void tesselate(int *n=NULL);
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
		void print(std::ostream *out);
		void setDrawControlMesh(bool draw);
		void setDrawMeshlines(bool draw) ;
		void setColorByParameterValues(bool draw);

		void initMouseMasks() ;
		void setMaskPos(int x, int y, bool value) ;

		void getBoundingBox(Go::BoundingBox &box) const ;
			
	
	private:

		bool draw_contol_mesh;
		bool draw_meshlines;
		bool colorByParametervalues;
		int resolution[2];
		int triangle_count;
		int width;
		int height;
		int faceIndex;
		int line_count; // for control polygon
		int cp_count;
		int dim;
		GLdouble *cp_pos;
		GLuint   *cp_lines;
		GLdouble *positions;
		GLdouble *normals;
		GLdouble *param_values;
		GLuint   *triangle_strip;
		GLfloat  *xi_buffer;
		GLfloat  *eta_buffer;
		std::vector<CurveDisplay*> knot_lines;

		void myGridEvaluator(int n1, int n2, std::vector<double>& pts, std::vector<double>& nor, std::vector<double>& par_u, std::vector<double>& par_v);
};

#endif
