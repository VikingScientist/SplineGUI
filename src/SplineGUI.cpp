//==============================================================================
//!
//! \file SplineGUI.cpp
//!
//! \date July 2010
//!
//! \author Kjetil A. Johannessen / SINTEF
//!
//! \brief Public and private access functions to the SplineGUI GUI class
//!
//==============================================================================


// standard c++ headers
#include <iostream>
#include <stdlib.h>
#include <fstream>

// openGL headers
#include <GL/glut.h>

// GoTools headers
#include <GoTools/geometry/ParamCurve.h>
#include <GoTools/geometry/ObjectHeader.h>
#include <GoTools/utils/BoundingBox.h>

// SplineGUI headers
#include "Camera.h"
#include "DisplayObjectSet.h"
#include "CurvePoint.h"
#include "CurveDisplay.h"
#include "SurfaceDisplay.h"
#include "VolumeDisplay.h"
#include "OrthoProjection.h"
#include "Button.h"
#include "TextField.h"
#include "SplineGUI.h"

#include <sys/time.h>

typedef unsigned int uint;

using namespace std;
// using namespace boost;
using namespace Go;
using namespace Workaround_namespace;

SplineGUI *SplineGUI::instance_ = NULL;

namespace Workaround_namespace {

string fileUsage = "  splineGUI <inputFile> \n";

int window_width  = 1000;
int window_height = 700;
int window_border = 10;

// to detect mouse hover events
int last_mouse_x;
int last_mouse_y;
struct timeval last_mouse_move;
int hover_threadshold = 1000; // in milliseconds
bool does_hover       = false;
bool doIgnoreRemaskEvents = false;


Camera cam(480, 480, 200, 200);
OrthoProjection top_view(TOP);
OrthoProjection front_view(FRONT);
OrthoProjection left_view(LEFT);
vector<MVPHandler*> view_panels;

DisplayObjectSet objectSet;
vector<MouseListener*> mouse_listeners;
vector<KeyListener*> key_class_listeners;
vector<MouseListener*> selected_mouse_listeners;
vector<Button*> buttons;
vector<TextField*> textFields;

void addMouseListener(MouseListener *ml) {
	mouse_listeners.push_back(ml);
}
void addKeyboardClassListener(KeyListener* kl) {
	key_class_listeners.push_back(kl);
}

void removeMouseListener(MouseListener *ml) {
	for(uint i=0; i<mouse_listeners.size(); i++) {
		if(mouse_listeners[i] == ml) {
			mouse_listeners.erase(mouse_listeners.begin() + i);
			break;
		}
	}
}
void removeKeyboardClassListener(KeyListener* kl) {
	for(uint i=0; i<key_class_listeners.size(); i++) {
		if(key_class_listeners[i] == kl) {
			key_class_listeners.erase(key_class_listeners.begin() + i);
			break;
		}
	}
}


void actionListener(ActiveObject *caller, int event) {
	if(event & ACTION_REQUEST_REMASK && !doIgnoreRemaskEvents)
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
	
	objectSet.paintSelectionBox();
	for(vector<Button*>::size_type i=0; i<buttons.size(); i++)
		buttons[i]->paint();
	for(vector<TextField*>::size_type i=0; i<textFields.size(); i++)
		textFields[i]->paint();
	if(does_hover)
		objectSet.paintMetaInfoBox(last_mouse_x, last_mouse_y);

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
	int patchCount = 0;
	while(!inFile.eof()) {
		char *patchName = new char[256];
		sprintf(patchName, "Patch %d", patchCount++);
		head.read(inFile);
		switch(head.classType()) {
			case Class_SplineCurve: {
				SplineCurve *c = new SplineCurve();
				c->read(inFile);
				CurveDisplay *obj = new CurveDisplay(c);
				obj->setMeta(patchName);
				objectSet.addObject((DisplayObject*) obj);
				cout << "SplineCurve succesfully read" << endl;
				break;
			}
			case Class_SplineSurface: {
				SplineSurface *s = new SplineSurface();
				s->read(inFile);
				SurfaceDisplay *obj = new SurfaceDisplay(s);
				obj->setMeta(patchName);
				objectSet.addObject((DisplayObject*) obj);
				cout << "SplineSurface succesfully read" << endl;
				break;
			}
			case Class_SplineVolume: {
				SplineVolume *v = new SplineVolume();
				v->read(inFile);
				VolumeDisplay *obj = new VolumeDisplay(v);
				obj->setMeta(patchName);
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

	SplineGUI *gui = SplineGUI::getInstance();
	gui->computeBoundingBox();
}

void handleKeypress(unsigned char key, int x, int y) {
	SplineGUI *gui = SplineGUI::getInstance();
	for(uint i=0; i<gui->keyListeners_.size(); i++)
		gui->keyListeners_[i](key);
	for(uint i=0; i<key_class_listeners.size(); i++)
		key_class_listeners[i]->handleKeypress(key,x,y);
	
	if(!gui->isControlKeysEnabled() )
		return;

	if(key == 'c') {
		objectSet.changeControlMesh();
		objectSet.removeSelected();
		glutPostRedisplay();
	} else if(key == 'h') {
		does_hover = true;
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
	gettimeofday(&last_mouse_move, NULL);
	last_mouse_x = x;
	last_mouse_y = y;
	if(does_hover) {
		does_hover   = false;
		glutPostRedisplay();
	}
	for(vector<MouseListener*>::size_type i=0; i<mouse_listeners.size(); i++) {
		MouseListener* l = mouse_listeners[i];
		if(l->isCatchingAll() || l->isContained(x,y)) {
			if( !l->isHover()) 
				l->onEnter(x-l->getX(), y-l->getY());
			l->processMouseActiveMotion(x-l->getX(), y-l->getY());
		} else if( l->isHover() ) {
			l->onExit(x-l->getX(), y-l->getY());
		}
	}
	for(uint i=0; i<selected_mouse_listeners.size(); i++) {
		MouseListener* l = selected_mouse_listeners[i];
		if(l->isContained(x,y) && !l->isHover() )
			l->onEnter(x-l->getX(), y-l->getY());
		else if(!l->isContained(x,y) && l->isHover() )
			l->onExit(x-l->getX(), y-l->getY());
		l->processMouseActiveMotion(x-l->getX(), y-l->getY());
	}
}

void processMousePassiveMotion(int x, int y) {
	y = window_height - y; // define (x,y) to be in the lower left. Much easier since this is how GL works with these things
	gettimeofday(&last_mouse_move, NULL);
	last_mouse_x = x;
	last_mouse_y = y;
	if(does_hover) {
		does_hover   = false;
		glutPostRedisplay();
	}
	for(vector<MouseListener*>::size_type i=0; i<mouse_listeners.size(); i++) {
		MouseListener* l = mouse_listeners[i];
		if(l->isCatchingAll() || l->isContained(x,y)) {
			if( !l->isHover()) 
				l->onEnter(x-l->getX(), y-l->getY());
			l->processMousePassiveMotion(x-l->getX(), y-l->getY());
		} else if( l->isHover() ) {
			l->onExit(x-l->getX(), y-l->getY());
		}
	}
	for(uint i=0; i<selected_mouse_listeners.size(); i++) {
		MouseListener* l = selected_mouse_listeners[i];
		if(l->isContained(x,y) && !l->isHover() )
			l->onEnter(x-l->getX(), y-l->getY());
		else if(!l->isContained(x,y) && l->isHover() )
			l->onExit(x-l->getX(), y-l->getY());
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

void hoverTest() {
	timeval time_now;
	long mtime, seconds, useconds;

	gettimeofday(&time_now, NULL);
	seconds  = time_now.tv_sec  - last_mouse_move.tv_sec;
	useconds = time_now.tv_usec - last_mouse_move.tv_usec;
	mtime = ((seconds)*1000 + useconds/1000.0);

	if(mtime > hover_threadshold) {
		does_hover = true;
		glutPostRedisplay();
	}
}

} // namespace Workaround_namespace


SplineGUI::SplineGUI() {
	// initalize SplineGUI private attributes
	next_button_x = 10;
	next_button_y = 20;
	controlKeysEnabled = true;
	doIgnoreRemaskEvents = false;

	// initalize GLUT
	int argc = 0;
	glutInit(&argc, NULL);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	
	glutCreateWindow("SplineGUI");
	initRendering();
	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutPassiveMotionFunc(processMousePassiveMotion);
	glutReshapeFunc(handleResize);

	glutIdleFunc(hoverTest);
	
	// setup view_panels interaction
	view_panels.push_back(&front_view);
	view_panels.push_back(&left_view);
	view_panels.push_back(&top_view);
	view_panels.push_back(&cam);
	for(uint i=0; i<view_panels.size(); i++) {
		addMouseListener(view_panels[i]);
		((ActiveObject*)view_panels[i])->addActionListener(actionListener);
	}

	// setup DisplayObject interaction (curves, surfaces etc)
	addMouseListener(&objectSet);
	((ActiveObject*) &objectSet)->addActionListener(actionListener);

	// set hover start timing 
	gettimeofday(&last_mouse_move, NULL);

}

/**************************************************************//**
 \brief Displays the GUI and halts program execution

 see the description on getInstance()
 *****************************************************************/
void SplineGUI::show() {
	glutMainLoop();
}

/**************************************************************//**
 \brief Sets the window size 
 \param width Width in pixels
 \param height Height in pixels
 *****************************************************************/
void SplineGUI::setSize(int width, int height) {
	window_width  = width;
	window_height = height;
}

/**************************************************************//**
 \brief Adds .g2-file to display
 \param filename name of .g2-file to be read and added to the display list
 *****************************************************************/
void SplineGUI::addFile(const char *filename) {
	readFile(filename);
}

//! \brief adds a single Point to be displayed
void SplineGUI::addObject(Go::Point *p) {
	PointDisplay *obj = new PointDisplay(*p);
	objectSet.addObject((DisplayObject*) obj);
}

//! \brief adds a single SplineCurve to be displayed
void SplineGUI::addObject(Go::SplineCurve *c, bool clean) {
	CurveDisplay *obj = new CurveDisplay(c, clean);
	objectSet.addObject((DisplayObject*) obj);
}

//! \brief adds a single SplineSurface to be displayed
void SplineGUI::addObject(Go::SplineSurface *s, bool clean) {
	SurfaceDisplay *obj = new SurfaceDisplay(s, clean);
	objectSet.addObject((DisplayObject*) obj);
}

//! \brief adds a single SplineVolume to be displayed
void SplineGUI::addObject(Go::SplineVolume *v) {
	VolumeDisplay *obj = new VolumeDisplay(v);
	objectSet.addObject((DisplayObject*) obj);
}

void SplineGUI::setSplineColor(float r, float g, float b) {
	objectSet.setColor(VOLUME, r,g,b);
	objectSet.setColor(SURFACE, r,g,b);
}

PointDisplay* SplineGUI::getDisplayObject(Go::Point *p) {
	return objectSet.getDisplayObject(p);
}

CurveDisplay* SplineGUI::getDisplayObject(Go::SplineCurve *c) {
	return objectSet.getDisplayObject(c);
}

SurfaceDisplay* SplineGUI::getDisplayObject(Go::SplineSurface *s) {
	return objectSet.getDisplayObject(s);
}

VolumeDisplay* SplineGUI::getDisplayObject(Go::SplineVolume *v) {
	return objectSet.getDisplayObject(v);
}


vector<DisplayObject*> SplineGUI::getSelectedObjects() {
	return objectSet.getSelectedObjects();
}

void SplineGUI::hideObjects(DISPLAY_CLASS_TYPE type) {
	objectSet.hideObjects(type);
}

void SplineGUI::unHideObjects(DISPLAY_CLASS_TYPE type) {
	objectSet.unHideObjects(type);
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

void SplineGUI::addButton(Button *b) {
	string text = b->getText();
	int width   = text.length() * 6 + 28;
	int height  = 30;
	int x       = next_button_x;
	int y       = next_button_y;

	next_button_x += width + 10;
	b->setSizeAndPos(x, y, width, height);
	addMouseListener((MouseListener*)b);
	((ActiveObject*) b)->addActionListener(actionListener);
	buttons.push_back(b);
}

void SplineGUI::addTextField(TextField *tf) {
	int width   = tf->getWidth();
	int height  = tf->getHeight();
	int x       = next_button_x;
	int y       = next_button_y;

	next_button_x += width + 10;
	tf->setSizeAndPos(x, y, width, height);
	addMouseListener(tf);
	addKeyboardClassListener((KeyListener*) tf);
	((ActiveObject*) tf)->addActionListener(actionListener);
	textFields.push_back(tf);
}

void SplineGUI::removeTextField(TextField *tf) {
	next_button_x -= tf->getWidth() - 10;
	removeMouseListener(tf);
	removeKeyboardClassListener((KeyListener*)tf);
	for(uint i=0; i<textFields.size(); i++) {
		if(textFields[i] == tf) {
			textFields.erase(textFields.begin() + i);
			break;
		}
	}
	delete tf;
}

void SplineGUI::computeBoundingBox() {
	BoundingBox box;
	objectSet.getBoundingBox(box);
	cam.viewBoundingBox(box);
	top_view.viewBoundingBox(box);
	front_view.viewBoundingBox(box);
	left_view.viewBoundingBox(box);
}

//! \brief true if GUI should surpress (costly) remask events used to detect object selections.
//!        Remask may still be applied by manually calling updateMouseMasks() when needed
void SplineGUI::ignoreRemaskEvents(bool ignore) {
	doIgnoreRemaskEvents = ignore;
}

//! \brief true if GUI should interpet special key-characters (c,q,space etc) as view control
//!        false if these are to be ignored (i.e. only passed to key listeners)
void SplineGUI::enableControlKeys(bool controlKeysEnabled) {
	this->controlKeysEnabled = controlKeysEnabled;
}

bool SplineGUI::isControlKeysEnabled() {
	return controlKeysEnabled;
}

void SplineGUI::addKeyboardListener(void (*keyListener)(unsigned char)) {
	keyListeners_.push_back(keyListener);
}

DisplayObjectSet* SplineGUI::getObjectSet() {
	return &objectSet;
}

