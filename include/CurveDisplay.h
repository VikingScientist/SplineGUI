
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
		virtual ~CurveDisplay();
		virtual void reTesselate();
		virtual void tesselate(int *n=NULL);
		virtual void paint();
		virtual void paintSelected();
		virtual void paintMouseAreas();
		virtual void readMouseAreas();
		virtual bool isAtPosition(int x, int y);
		virtual double parValueAtPosition(int x, int y);
		virtual bool splitPeriodicCurveInFour(std::vector<CurvePoint*> splits, std::vector<Go::SplineCurve*> &ret_val);
		virtual void processMouse(int button, int state, int x, int y);
		virtual void processMouseActiveMotion(int x, int y);
		virtual void processMousePassiveMotion(int x, int y);
		virtual DISPLAY_CLASS_TYPE classType() { return CURVE; }
		virtual void printDebugInfo();
		virtual void print(std::ostream *out);
		virtual void initMouseMasks();
		virtual void setMaskPos(int x, int y, bool value);
		virtual void paintMouseAreas(float r, float g, float b) ;

		virtual void setLineWidth(int line_width);

		virtual void getBoundingBox(Go::BoundingBox &box) const ;
			
	
	protected:

		int resolution;
		int width;
		int height;
		int line_width;
		int dim;
		double *positions;
		double *param_values;
		double *color_buffer;
		GLfloat *xi_buffer;
		bool selected;
		PointDisplay *start_p;
		PointDisplay *stop_p;
};

#endif
