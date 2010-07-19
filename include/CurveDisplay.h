
#ifndef _CURVE_DISPLAY_H
#define _CURVE_DISPLAY_H

#include "DisplayObject.h"
#include <GoTools/geometry/SplineCurve.h>
#include "ActiveObject.h"
#include <GL/glut.h>
#include <vector>
class CurvePoint;

using namespace std;
using namespace Go;

static const int CURVE_SELECTED = 0;

class CurveDisplay : public DisplayObject, ActiveObject {
	
	public:
		SplineCurve *curve;

		// CurveDisplay(boost::shared_ptr<SplineCurve> curve);
		CurveDisplay(SplineCurve *curve);
		~CurveDisplay();
		void reTesselate();
		void tesselate(int *n);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		bool isAtPosition(int x, int y);
		double parValueAtPosition(int x, int y);
		bool splitPeriodicCurveInFour(vector<CurvePoint*> splits, vector<SplineCurve*> &ret_val);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void setActionListener(void (*actionPerformed)(ActiveObject*, int));
		DISPLAY_CLASS_TYPE classType() { return CURVE; }
		void printDebugInfo();
		void print(ostream *out);
			
	
	private:

		int resolution;
		int width;
		int height;
		double *positions;
		double *param_values;
		GLfloat *xi_buffer;
		bool selected;
};

#endif
