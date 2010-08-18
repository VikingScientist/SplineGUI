
#include "PointDisplay.h"
#include <GL/glu.h>
#include <cmath>

PointDisplay::PointDisplay(Point p) {
	this->point = p;
	size = 10;
	bool_buffer = NULL;
	setColor(.1,.1,.1);
	setSelectedColor(.9,.9,.9);
}

PointDisplay::~PointDisplay() {
	if(bool_buffer) delete[] bool_buffer;
}

void PointDisplay::tesselate(int *n) {
}

void PointDisplay::paint() {
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glColor3f(color[0], color[1], color[2]);
	glPointSize(size);
	glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
	glEnd();

	glDisable(GL_POINT_SMOOTH);
	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);
}

void PointDisplay::paintSelected() {
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glColor3f(selected_color[0], selected_color[1], selected_color[2]);
	glPointSize(size);
	glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
	glEnd();

	glDisable(GL_POINT_SMOOTH);
	glBlendFunc(GL_NONE, GL_NONE);
	glDisable(GL_BLEND);
}

void PointDisplay::paintMouseAreas() {
	glColor3f(1,1,1);
	glPointSize(size+8);
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

void PointDisplay::initMouseMasks() {
	if(bool_buffer) delete[] bool_buffer;
	width  = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	bool_buffer = new GLfloat[width*height];
	// for(int i=0; i<width*height; i++)
		// bool_buffer[i] = 0.0;
}

void PointDisplay::setMaskPos(int x, int y, bool value) {
	bool_buffer[y*width+x] = (value) ? 1.0 : 0.0;
}

void PointDisplay::paintMouseAreas(float r, float g, float b) {
	glColor3f(r,g,b);
	glPointSize(size+8);
	glBegin(GL_POINTS);
		glVertex3f(point[0], point[1], point[2]);
	glEnd();
}

void PointDisplay::setPointSize(int size) {
	this->size = size;
}


