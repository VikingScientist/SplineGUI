
#include <GoTools/geometry/SplineCurve.h>
#include "CurvePoint.h"
#include "CurveDisplay.h"
#include <GL/glu.h>
#include <cmath>

CurvePoint::CurvePoint(CurveDisplay *curve, double u) {
	this->curve = curve;
	this->u = u;
	xi_buffer = NULL;
	start = false;
}

CurvePoint::~CurvePoint() {
	if(xi_buffer) delete[] xi_buffer;
}

void CurvePoint::tesselate(int *n) {
	Go::Point pt(3);
	Go::Point pt_dir(3);
	double ev_u = curve->curve->startparam() + u*(curve->curve->endparam() - curve->curve->startparam());
	curve->curve->point(pt, ev_u);
	double u_front = u + .03;
	if(u_front > 1.0) 
		u_front -= 1.0;
	double ev_u_direction = curve->curve->startparam() + u_front*(curve->curve->endparam() - curve->curve->startparam());
	curve->curve->point(pt_dir, ev_u_direction);
	pos[0] = pt[0];
	pos[1] = pt[1];
	pos[2] = pt[2];
	direction[0] = pt_dir[0] - pt[0];
	direction[1] = pt_dir[1] - pt[1];
	direction[2] = pt_dir[2] - pt[2];
}

void CurvePoint::paint() {
	if(start)
		glColor3f(.2,.4,.2);
	else
		glColor3f(.2,.2,.2);
	glPointSize(10);
	glBegin(GL_POINTS);
		glVertex3f(pos[0], pos[1], pos[2]);
	glEnd();
	if(start)
		paintDirectionArrow();
}

void CurvePoint::paintSelected() {
	glColor3f(.9,.9,.9);
	glPointSize(10);
	glBegin(GL_POINTS);
		glVertex3f(pos[0], pos[1], pos[2]);
	glEnd();
	if(start)
		paintDirectionArrow();
}

void CurvePoint::paintMouseAreas() {
	glColor3f(1,1,1);
	glPointSize(16);
	glBegin(GL_POINTS);
		glVertex3f(pos[0], pos[1], pos[2]);
	glEnd();
	if(start)
		paintDirectionArrow();
}

void CurvePoint::readMouseAreas() {
	if(xi_buffer) delete[] xi_buffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	xi_buffer = new GLfloat[width*height];
	glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, xi_buffer);
}

void CurvePoint::paintDirectionArrow() {
	glEnable(GL_LIGHTING);
	GLUquadricObj *quadObj = gluNewQuadric();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	double rho   = sqrt(direction[0]*direction[0] + direction[1]*direction[1] + direction[2]*direction[2]);
	double phi   = acos(direction[2]/rho) * 180/M_PI;
	double theta = atan2(direction[1], direction[0]) * 180/M_PI;
	glTranslatef(pos[0], pos[1], pos[2]);
	glRotatef(theta, 0,0,1);
	glRotatef(phi, 0,1,0);
	gluCylinder(quadObj, rho/3, 0, rho, 6, 4);
	glPopMatrix();
	glDisable(GL_LIGHTING);
}

bool CurvePoint::isAtPosition(int x, int y) {
	return (xi_buffer[y*width+x] > 0.0);
}

void CurvePoint::processMouse(int button, int state, int x, int y) {
}

void CurvePoint::processMouseActiveMotion(int x, int y) {
	if(curve->isAtPosition(x,y)) {
		u = curve->parValueAtPosition(x,y);
		tesselate(NULL);
		fireActionEvent(REQUEST_REPAINT);
	}
}

void CurvePoint::processMousePassiveMotion(int x, int y) {
}

void CurvePoint::setToStart() {
	start = true;
}

void CurvePoint::setParValue(double u) {
	this->u = u;
}

double CurvePoint::getParValue() {
	return u;
}

void CurvePoint::printDebugInfo() {
	cout << "Point at " << u << endl;
}

void CurvePoint::print(ostream *out) {
	// Curve Points are not represented in GoTools (as far as we are concerned anyway)
}

