
#include "PointDisplay.h"
#include "CurveDisplay.h"
#include <GL/glu.h>
#include <cmath>

PointDisplay::PointDisplay(Point p) {
	this->point = p;
	bool_buffer = NULL;
}

PointDisplay::~PointDisplay() {
	if(bool_buffer) delete[] bool_buffer;
}

void PointDisplay::tesselate(int *n) {
}

void PointDisplay::paint() {
	glColor3f(.2,.2,.2);
	glPointSize(10);
	glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
	glEnd();
}

void PointDisplay::paintSelected() {
	glColor3f(.9,.9,.9);
	glPointSize(10);
	glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
	glEnd();
}

void PointDisplay::paintMouseAreas() {
	glColor3f(1,1,1);
	glPointSize(16);
	glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
	glEnd();
}

void PointDisplay::readMouseAreas() {
	if(bool_buffer) delete[] bool_buffer;
	width  = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	bool_buffer = new GLfloat[width*height];
	glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, bool_buffer);
}

bool PointDisplay::isAtPosition(int x, int y) {
	return (bool_buffer[y*width+x] > 0.0);
}

void PointDisplay::processMouse(int button, int state, int x, int y) {
}

void PointDisplay::processMouseActiveMotion(int x, int y) {
}

void PointDisplay::processMousePassiveMotion(int x, int y) {
}

void PointDisplay::printDebugInfo() {
	cout << "Point at " << point << endl;
}

void PointDisplay::print(ostream *out) {
}
