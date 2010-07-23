#ifndef _POINTDISPLAY_H
#define _POINTDISPLAY_H

#include <GoTools/utils/Point.h>
#include <GL/glut.h>
#include "DisplayObject.h"
#include "ActiveObject.h"

using namespace Go;

class PointDisplay : public DisplayObject, ActiveObject {

	public:
		Point point;

		PointDisplay(Point p);
		~PointDisplay();
		void tesselate(int *n);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		DISPLAY_CLASS_TYPE classType() { return POINT; }
		bool isAtPosition(int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void printDebugInfo();
		void print(ostream *out);

	private:
		int width;
		int height;
		GLfloat *bool_buffer;
};

#endif

