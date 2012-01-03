#ifndef _CURVEPOINT_H
#define _CURVEPOINT_H

#include <GL/glut.h>
#include "DisplayObject.h"
class CurveDisplay;

class CurvePoint : public DisplayObject {

	public:
		CurvePoint(CurveDisplay *curve, double u);
		~CurvePoint();
		void tesselate(int *n);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		DISPLAY_CLASS_TYPE classType() { return CURVE_POINT; }
		bool isAtPosition(int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void makeDirectionalPoint();
		void setParValue(double u);
		double getParValue();
		void printDebugInfo();
		void print(std::ostream *out);

	private:
		void paintDirectionArrow();
		double u;
		double pos[3];
		double direction[3];
		int width;
		int height;
		CurveDisplay *curve;
		GLfloat *xi_buffer;
		bool directionalPoint;
};

#endif
