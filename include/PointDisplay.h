#ifndef _POINTDISPLAY_H
#define _POINTDISPLAY_H

#include <GoTools/utils/Point.h>
#include <GoTools/utils/BoundingBox.h>
#include <GL/glut.h>
#include "DisplayObject.h"

class PointDisplay : public DisplayObject {

	public:
		Go::Point point;

		PointDisplay(Go::Point p);
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
		void print(std::ostream *out);

		void initMouseMasks();
		void setMaskPos(int x, int y, bool value);
		void paintMouseAreas(float r, float g, float b) ;

		void getBoundingBox(Go::BoundingBox &box) const ;
		void setPointSize(int size);

	private:
		int width;
		int height;
		int size;
		GLfloat *bool_buffer;
};

#endif

