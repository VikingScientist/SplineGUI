//==============================================================================
//!
//! \file Fenris.cpp
//!
//! \date July 2010
//!
//! \author Kjetil A. Johannessen / SINTEF
//!
//! \brief Public and private access functions to the Fenris GUI class
//!
//==============================================================================

/**
 * \todo Is it possible to remove the use of the Workaround_namespace and replace this by
 * static class-functions? Possible also private.
 * 
 * \todo Implement a function to allow for command-line input (ideally we would like pop-up
 * dialogs, but GLUT doesn't really make this easy).
 */

// standard c++ headers
#include <iostream>
#include <stdlib.h>
#include <fstream>

// openGL headers
#include <GL/glut.h>

// GoTools headers
#include <GoTools/geometry/ParamCurve.h>
#include <GoTools/geometry/ObjectHeader.h>

// Fenris headers
#include "Camera.h"
#include "DisplayObject.h"
#include "DisplayObjectSet.h"
#include "CurvePoint.h"
#include "CurveDisplay.h"
#include "SurfaceDisplay.h"
#include "VolumeDisplay.h"
#include "OrthoProjection.h"
#include "Button.h"
#include "Fenris.h"
typedef unsigned int uint;

using namespace std;
using namespace boost;
using namespace Go;
using namespace Workaround_namespace;

Fenris *Fenris::instance_ = NULL;

namespace Workaround_namespace {

string fileUsage = "  fenris <inputFile> \n";

int window_width  = 1000;
int window_height = 700;
int window_border = 10;

Camera cam(480, 480, 200, 200);
OrthoProjection top_view(TOP);
OrthoProjection front_view(FRONT);
OrthoProjection left_view(LEFT);
vector<MVPHandler*> view_panels;

DisplayObjectSet objectSet;
vector<MouseListener*> mouse_listeners;
vector<MouseListener*> selected_mouse_listeners;
vector<Button*> buttons;

void addMouseListener(MouseListener *ml) {
	mouse_listeners.push_back(ml);
}

void actionListener(ActiveObject *caller, int event) {
	if(event & ACTION_REQUEST_REMASK)
		updateMouseMasks();
	if(event & ACTION_REQUEST_REPAINT)
		glutPostRedisplay();
}


void updateMouseMasks() {
	objectSet.paintAllMouseAreas(view_panels);
}

void drawScene() {
	glViewport(0,0, window_width, window_height);
	glClearColor(.7, .7, .7, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
	for(uint i=0; i<view_panels.size(); i++) {
		view_panels[i]->setScissortest();
		view_panels[i]->setViewport();
		view_panels[i]->paintBackground();
		view_panels[i]->setProjection();
		view_panels[i]->setModelView();
		objectSet.paintAll();
		view_panels[i]->paintMeta();
	}

	glDisable(GL_SCISSOR_TEST);
	glViewport(0,0, window_width, window_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	
	for(vector<Button*>::size_type i=0; i<buttons.size(); i++)
		buttons[i]->paint();

	glutSwapBuffers();
	if(objectSet.hasNewObjects) {
		updateMouseMasks();
		objectSet.hasNewObjects = false;
	}
}

void handleResize(int w, int h) {
	window_width  = w;
	window_height = h;
	int b = window_border;
	if(view_panels.size() == 4) {
		cam.handleResize     (    (w+b)/2, (h-3*b)*3/5+2*b, (w-3*b)/2, (h-3*b)/5*2 );
		top_view.handleResize(          b, (h-3*b)*3/5+2*b, (w-3*b)/2, (h-3*b)/5*2 );
		left_view.handleResize(   (w+b)/2,       (h+2*b)/5, (w-3*b)/2, (h-3*b)/5*2 );
		front_view.handleResize(        b,       (h+2*b)/5, (w-3*b)/2, (h-3*b)/5*2 );
	} else { // view_panels.size() == 1
		cam.handleResize     (   0,0,0,0);
		top_view.handleResize(   0,0,0,0);
		left_view.handleResize(  0,0,0,0);
		front_view.handleResize( 0,0,0,0);
		view_panels[0]->handleResize( b, (h+2*b)/5, w-2*b, (h-2*b)*4/5 );
	}
		
	glutPostRedisplay();
	updateMouseMasks();
}

void readFile(const char *filename) {
	ifstream inFile;
	ObjectHeader head;
	inFile.open(filename);
	if(!inFile.good()) {
		cerr << "Error reading input file \"" << filename << "\"" << endl;
		exit(1);
	}
	while(!inFile.eof()) {
		head.read(inFile);
		switch(head.classType()) {
			case Class_SplineCurve: {
				SplineCurve *c = new SplineCurve();
				c->read(inFile);
				CurveDisplay *obj = new CurveDisplay(c);
				objectSet.addObject((DisplayObject*) obj);
				cout << "SplineCurve succesfully read" << endl;
				break;
			}
			case Class_SplineSurface: {
				SplineSurface *s = new SplineSurface();
				s->read(inFile);
				SurfaceDisplay *obj = new SurfaceDisplay(s);
				objectSet.addObject((DisplayObject*) obj);
				cout << "SplineSurface succesfully read" << endl;
				break;
			}
			case Class_SplineVolume: {
				SplineVolume *v = new SplineVolume();
				v->read(inFile);
				VolumeDisplay *obj = new VolumeDisplay(v);
				objectSet.addObject((DisplayObject*) obj);
				cout << "SplineVolume succesfully read" << endl;
				break;
			}
			case Class_CurveOnSurface:
			case Class_Line:
			case Class_Circle:
			case Class_BoundedSurface:
			case Class_GoBaryPolSurface:
			case Class_GoHBSplineParamSurface:
			case Class_CompositeSurface:
			case Class_Plane:
			case Class_Cylinder:
			case Class_Sphere:
			case Class_Cone:
			case Class_Torus:
			case Class_Go3dsObject:
			case Class_GoHeTriang:
			case Class_GoSdTriang:
			case Class_GoQuadMesh:
			case Class_GoHybridMesh:
			case Class_ParamTriang:
			case Class_GoVrmlGeometry:
			case Class_PointCloud:
			case Class_LineCloud:
			case Class_GoTriangleSets:
			case Class_RectGrid:
			case Class_Unknown:
			default:
				fprintf(stderr, "File \"%s\" contains unknown or unsupported class object\n", filename);
				exit(1);
		}
		ws(inFile); // eat whitespaces
	}
	inFile.close();
}

void handleKeypress(unsigned char key, int x, int y) {
	if(key == 'c') {
		objectSet.changeControlMesh();
		objectSet.removeSelected();
		glutPostRedisplay();
	} else if(key == 32) { // space-key
		if(view_panels.size() == 1) {
			view_panels.clear();
			view_panels.push_back(&cam);
			view_panels.push_back(&front_view);
			view_panels.push_back(&left_view);
			view_panels.push_back(&top_view);
			handleResize(window_width, window_height);
		} else {
			for(uint i=0; i<view_panels.size(); i++) {
				if(((MouseListener*)view_panels[i])->isHover()) {
					MVPHandler *big_panel = view_panels[i];
					view_panels.clear();
					view_panels.push_back(big_panel);
					handleResize(window_width, window_height);
					break;
				}
			}
		}
	} else if(key == 'q') {
		exit(0);
	}
}

void processMouse(int button, int state, int x, int y) {
	y = window_height - y; // define (x,y) to be in the lower left. Much easier since this is how GL works with these things
	for(vector<Button*>::size_type i=0; i<buttons.size(); i++) {
		MouseListener *l = buttons[i];
		if(l->isCatchingAll() || l->isContained(x,y)) {
			l->processMouse(button, state, x-l->getX(), y-l->getY());
			if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) { // button-presses are stopping the call to proceed to below layers
				return;
			}
		}
	}
	for(vector<MouseListener*>::size_type i=mouse_listeners.size(); i-->0; ) {
		MouseListener* l = mouse_listeners[i];
		if(l->isCatchingAll() || l->isContained(x,y)) {
			l->processMouse(button, state, x-l->getX(), y-l->getY());
			mouse_listeners.erase(mouse_listeners.begin()+i);
			selected_mouse_listeners.push_back(l);
		}
	}
	if(state == GLUT_UP) { // clear selected mouse listeners
		while(selected_mouse_listeners.size() > 0) {
			MouseListener* l = selected_mouse_listeners.back();
			l->processMouse(button, state, x-l->getX(), y-l->getY());
			mouse_listeners.push_back(l);
			selected_mouse_listeners.pop_back();
		}
	}
}

void processMouseActiveMotion(int x, int y) {
	y = window_height - y; // define (x,y) origin to be in the lower left. Much easier since this is how GL works with these things
	for(vector<MouseListener*>::size_type i=0; i<mouse_listeners.size(); i++) {
		MouseListener* l = mouse_listeners[i];
		if(l->isCatchingAll() || l->isContained(x,y)) {
			if( !l->isHover()) 
				l->onEnter();
			l->processMouseActiveMotion(x-l->getX(), y-l->getY());
		} else if( l->isHover() ) {
			l->onExit();
		}
	}
	for(uint i=0; i<selected_mouse_listeners.size(); i++) {
		MouseListener* l = selected_mouse_listeners[i];
		if(l->isContained(x,y) && !l->isHover() )
			l->onEnter();
		else if(!l->isContained(x,y) && l->isHover() )
			l->onExit();
		l->processMouseActiveMotion(x-l->getX(), y-l->getY());
	}
}

void processMousePassiveMotion(int x, int y) {
	y = window_height - y; // define (x,y) to be in the lower left. Much easier since this is how GL works with these things
	for(vector<MouseListener*>::size_type i=0; i<mouse_listeners.size(); i++) {
		MouseListener* l = mouse_listeners[i];
		if(l->isCatchingAll() || l->isContained(x,y)) {
			if( !l->isHover()) 
				l->onEnter();
			l->processMousePassiveMotion(x-l->getX(), y-l->getY());
		} else if( l->isHover() ) {
			l->onExit();
		}
	}
	for(uint i=0; i<selected_mouse_listeners.size(); i++) {
		MouseListener* l = selected_mouse_listeners[i];
		if(l->isContained(x,y) && !l->isHover() )
			l->onEnter();
		else if(!l->isContained(x,y) && l->isHover() )
			l->onExit();
		l->processMousePassiveMotion(x-l->getX(), y-l->getY());
	}
}

void initRendering() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glDepthFunc(GL_LEQUAL);
	glReadBuffer(GL_BACK); // readPixels used to create a mask for mouse inputs
	// glEnable(GL_POINT_SMOOTH);
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
	glEnable ( GL_COLOR_MATERIAL ) ;

	objectSet.tesselateAll(); // using default resolution
}

/*
void *start(void *arg) {
	glutMainLoop();
	return NULL;
}
*/

} // namespace Workaround_namespace


Fenris::Fenris() {
	// initalize Fenris private attributes
	next_button_x = 10;
	next_button_y = 20;

	// initalize GLUT
	int argc = 0;
	glutInit(&argc, NULL);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	
	glutCreateWindow("Fenris");
	initRendering();
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutReshapeFunc(handleResize);
	
	// setup view_panels interaction
	view_panels.push_back(&front_view);
	view_panels.push_back(&left_view);
	view_panels.push_back(&top_view);
	view_panels.push_back(&cam);
	for(uint i=0; i<view_panels.size(); i++) {
		addMouseListener(view_panels[i]);
		((ActiveObject*)view_panels[i])->setActionListener(actionListener);
	}

	// setup DisplayObject interaction (curves, surfaces etc)
	addMouseListener(&objectSet);
	((ActiveObject*) &objectSet)->setActionListener(actionListener);

}

/**************************************************************//**
 \brief Displays the GUI and halts program execution

 see the description on getInstance()
 *****************************************************************/
void Fenris::show() {
	glutMainLoop();
}

/**************************************************************//**
 \brief Sets the window size 
 \param width Width in pixels
 \param height Height in pixels
 *****************************************************************/
void Fenris::setSize(int width, int height) {
	window_width  = width;
	window_height = height;
}

/**************************************************************//**
 \brief Adds .g2-file to display
 \param filename name of .g2-file to be read and added to the display list
 *****************************************************************/
void Fenris::addFile(const char *filename) {
	readFile(filename);
}

//! \brief adds a single SplineCurve to be displayed
void Fenris::addObject(Go::SplineCurve *c) {
	CurveDisplay *obj = new CurveDisplay(c);
	objectSet.addObject((DisplayObject*) obj);
}

//! \brief adds a single SplineSurface to be displayed
void Fenris::addObject(Go::SplineSurface *s) {
	SurfaceDisplay *obj = new SurfaceDisplay(s);
	objectSet.addObject((DisplayObject*) obj);
}

//! \brief adds a single SplineVolume to be displayed
void Fenris::addObject(Go::SplineVolume *v) {
	VolumeDisplay *obj = new VolumeDisplay(v);
	objectSet.addObject((DisplayObject*) obj);
}

//! \brief get all selected volumes, ignoring selected surfaces
vector<SplineVolume*> getSelectedVolumes() {
	vector<SplineVolume*> results;
	vector<DisplayObject*> selected = objectSet.getSelectedObjects(VOLUME);
	for(uint i=0; i<selected.size(); i++) {
		SplineVolume *v = ((VolumeDisplay*)selected[i])->volume;
		results.push_back(v);
	}
	return results;
}

//! \brief get all selected surfaces, ignoring selected volumes
vector<SplineSurface*> getSelectedSurfaces() {
	vector<SplineSurface*> results;
	vector<DisplayObject*> selected = objectSet.getSelectedObjects(SURFACE);
	for(uint i=0; i<selected.size(); i++) {
		SplineSurface *s = ((SurfaceDisplay*)selected[i])->surf;
		results.push_back(s);
	}
	return results;
}

void Fenris::addButton(Button *b) {
	string text = b->getText();
	int width   = text.length() * 6 + 20;
	int height  = 30;
	int x       = next_button_x;
	int y       = next_button_y;

	next_button_x += width + 10;
	b->setSizeAndPos(x, y, width, height);
	addMouseListener((MouseListener*)b);
	((ActiveObject*) b)->setActionListener(actionListener);
	buttons.push_back(b);
}

