#include "LRVolDisplay.h"
#include "VolumeDisplay.h"
#include "SurfaceDisplay.h"
#include "SplineGUI.h"
#include "LRSpline/Element.h"
#include <GL/glut.h>
#include <GoTools/geometry/ObjectHeader.h>
#include <float.h>
#include <algorithm>

using namespace Go;
using namespace LR;
using namespace std;

static void getBezierKnot(int p, vector<double>& knot) {
	knot.resize(2*p);
	for(int i=0; i<p; i++) knot[i] = 0;
	for(int i=0; i<p; i++) knot[i] = 1;
}

LRVolDisplay::LRVolDisplay(LRSplineVolume *volume) : DisplayObject() {
	this->volume           = volume;
	positions              = NULL;
	normals                = NULL;
	triangle_strip         = NULL;
	param_values           = NULL;
	wallbuffer             = NULL;
	cp_pos                 = NULL;
	resolution[0]          = 0;
	resolution[1]          = 0;
	resolution[2]          = 0;
	draw_contol_mesh       = false;

	int p1 = volume->order(0);
	int p2 = volume->order(1);
	int p3 = volume->order(2);
	vector<double> knot1(2*p1);
	vector<double> knot2(2*p2);
	vector<double> knot3(2*p3);
	vector<double> coefs;
	for(int i=0; i<p1; i++) knot1[ i  ] = 0.0;
	for(int i=0; i<p1; i++) knot1[p1+i] = 1.0;
	for(int i=0; i<p2; i++) knot2[ i  ] = 0.0;
	for(int i=0; i<p2; i++) knot2[p2+i] = 1.0;
	for(int i=0; i<p3; i++) knot3[ i  ] = 0.0;
	for(int i=0; i<p3; i++) knot3[p3+i] = 1.0;
	int iel = -1;
	for(Element *el : volume->getAllElements()) {
		iel++;
		if(el->getParmin(0) != volume->startparam(0) && 
		   el->getParmin(1) != volume->startparam(1) && 
		   el->getParmin(2) != volume->startparam(2) && 
		   el->getParmax(0) != volume->endparam(0)  && 
		   el->getParmax(1) != volume->endparam(1)  && 
		   el->getParmax(2) != volume->endparam(2) )
			continue;
		volume->getBezierElement(iel, coefs);
		SplineVolume *oneBez = new SplineVolume(p1, p2, p3,
		                                        p1, p2, p3,
		                                        knot1.begin(), knot2.begin(), knot3.begin(),
		                                        coefs.begin(), 3, false);
		bezierVols.push_back(new VolumeDisplay(oneBez));
	}

	setColor(.8, .4, .05); // orange

}

LRVolDisplay::~LRVolDisplay() {
	if(positions)      delete[] positions;
	if(normals)        delete[] normals;
	if(triangle_strip) delete[] triangle_strip;
	if(param_values)   delete[] param_values;
	if(wallbuffer)     delete[] wallbuffer;
	if(cp_pos)         delete[] cp_pos;
	for(VolumeDisplay *v : bezierVols) 
		delete v;
}

void LRVolDisplay::setSelectedColor(double r, double g, double b) {
	DisplayObject::setSelectedColor(r,g,b);
	for(VolumeDisplay *v : bezierVols)
		v->setSelectedColor(r,g,b);
}

void LRVolDisplay::setColor(double r, double g, double b) {
	DisplayObject::setColor(r,g,b);
	for(VolumeDisplay *v : bezierVols)
		v->setColor(r,g,b);
}

void LRVolDisplay::tesselate(int *n) {
	int xi_eta[]   = {n[0], n[1]};
	int xi_zeta[]  = {n[0], n[2]};
	int eta_zeta[] = {n[1], n[2]};
	resolution[0]  = n[0];
	resolution[1]  = n[1];
	resolution[2]  = n[2];
	
	for(VolumeDisplay *v : bezierVols)
		v->tesselate(n);

	if(cp_pos)   delete[] cp_pos;
	cp_count = volume->nBasisFunctions();
	cp_pos = new GLdouble[cp_count*3];
	int i=0;
	for(Basisfunction* b : volume->getAllBasisfunctions())
		for(int d=0; d<3; d++)
			cp_pos[i++] = b->cp(d);
	
}

void LRVolDisplay::paint() {
	if(draw_contol_mesh) {
		glColor3f(0,0,0);
		glLineWidth(2);
		glPointSize(6);
		glVertexPointer(3, GL_DOUBLE, 0, cp_pos);
		glDrawArrays(GL_POINTS, 0, cp_count);
	} else {
		for(VolumeDisplay *v : bezierVols) 
			if(! v->isSelected())
				v->paint();
	}
}

void LRVolDisplay::paintSelected() {
	for(VolumeDisplay *v : bezierVols) 
		v->paintSelected();
}

void LRVolDisplay::paintMouseAreas() {
	glDisable(GL_COLOR_ARRAY);
	for(VolumeDisplay *v : bezierVols) 
		v->paintMouseAreas(1,1,1);
	glEnable(GL_COLOR_ARRAY);
}

void LRVolDisplay::readMouseAreas() {
	if(wallbuffer) delete[] wallbuffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	wallbuffer = new GLfloat[3*width*height];
	glReadPixels(0, 0, width, height, GL_RGB,  GL_FLOAT, wallbuffer);
}

bool LRVolDisplay::isAtPosition(int x, int y) {
	int k = (y*width+x)*3;
	return wallbuffer[k]>0 || wallbuffer[k+1]>0 || wallbuffer[k+2]>0;
}

SurfaceDisplay* LRVolDisplay::getSurfaceAt(int x, int y) {
	/*
	int k = (y*width+x)*3;
	if(wallbuffer[k]==0 && wallbuffer[k+1]==0 && wallbuffer[k+2]==1)      // 001
		return walls[0];
	else if(wallbuffer[k]==0 && wallbuffer[k+1]==1 && wallbuffer[k+2]==0) // 010
		return walls[1];
	else if(wallbuffer[k]==0 && wallbuffer[k+1]==1 && wallbuffer[k+2]==1) // 011
		return walls[2];
	else if(wallbuffer[k]==1 && wallbuffer[k+1]==0 && wallbuffer[k+2]==0) // 100
		return walls[3];
	else if(wallbuffer[k]==1 && wallbuffer[k+1]==0 && wallbuffer[k+2]==1) // 101
		return walls[4];
	else if(wallbuffer[k]==1 && wallbuffer[k+1]==1 && wallbuffer[k+2]==0) // 110
		return walls[5];
	else
	*/
		return NULL;
}


void LRVolDisplay::processMouse(int button, int state, int x, int y) {
	/*
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && xi_buffer[y*width+x] > 0.0) {
		selected = !selected;
	}
	*/
}

void LRVolDisplay::processMouseActiveMotion(int x, int y) {
}

void LRVolDisplay::processMousePassiveMotion(int x, int y) {
}

void LRVolDisplay::printDebugInfo() {
	cout << *volume;
}

void LRVolDisplay::print(ostream *out) {
	(*out) << *volume;
}

void LRVolDisplay::setDrawControlMesh(bool draw) {
	draw_contol_mesh = draw;
}

void LRVolDisplay::setColorByParameterValues(bool draw) {
	colorByParametervalues = draw;
	for(VolumeDisplay *v : bezierVols)
		v->setColorByParameterValues(draw);
}

void LRVolDisplay::initMouseMasks() {
	if(wallbuffer) delete[] wallbuffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	wallbuffer = new GLfloat[3*width*height];
	// for(int i=0; i<3*width*height; i++)
		// wallbuffer[i] = 0.0;
}

void LRVolDisplay::setMaskPos(int x, int y, bool value) {
	int k = (y*width+x)*3;
	wallbuffer[k] = (value) ? 1.0 : 0.0;
}

void LRVolDisplay::paintMouseAreas(float r, float g, float b) {
	for(VolumeDisplay *v : bezierVols)
		v->paintMouseAreas(r,g,b);
}

void LRVolDisplay::getBoundingBox(Go::BoundingBox &box) const { 
	double umin = DBL_MAX;
	double umax = DBL_MIN;
	double vmin = DBL_MAX;
	double vmax = DBL_MIN;
	double wmin = DBL_MAX;
	double wmax = DBL_MIN;
	for(Basisfunction *b : volume->getAllBasisfunctions()) {
		umin = min(umin, b->cp(0));
		umax = max(umax, b->cp(0));
		vmin = min(vmin, b->cp(1));
		vmax = max(vmax, b->cp(1));
		wmin = min(wmin, b->cp(2));
		wmax = max(wmax, b->cp(2));
	}
	box.setFromPoints(Point(umin,vmin,wmin), Point(umax,vmax,wmax));
}

