
#ifndef _CURVE_DISPLAY_H
#define _CURVE_DISPLAY_H

#include "DisplayObject.h"
#include "PointDisplay.h"
#include <GoTools/geometry/SplineCurve.h>
#include <GoTools/utils/BoundingBox.h>
#include <GL/glut.h>
#include <vector>
class CurvePoint;

static const int CURVE_SELECTED = 0;

class CurveDisplay : public DisplayObject {
	
	public:
		Go::SplineCurve *curve;

		CurveDisplay(Go::SplineCurve *curve, bool clean=false);
		~CurveDisplay();
		void reTesselate();
		void tesselate(int *n=NULL);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		bool isAtPosition(int x, int y);
		double parValueAtPosition(int x, int y);
		bool splitPeriodicCurveInFour(std::vector<CurvePoint*> splits, std::vector<Go::SplineCurve*> &ret_val);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		DISPLAY_CLASS_TYPE classType() { return CURVE; }
		void printDebugInfo();
		void print(std::ostream *out);
		void initMouseMasks();
		void setMaskPos(int x, int y, bool value);
		void paintMouseAreas(float r, float g, float b) ;

		void setLineWidth(int line_width);

		void getBoundingBox(Go::BoundingBox &box) const ;
			
	
	private:

		int resolution;
		int width;
		int height;
		int line_width;
		int dim;
		double *positions;
		double *param_values;
		GLfloat *xi_buffer;
		bool selected;
		PointDisplay *start_p;
		PointDisplay *stop_p;
};

#endif
