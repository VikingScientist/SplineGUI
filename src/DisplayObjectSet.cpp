
#include "DisplayObjectSet.h"
#include "DisplayObject.h"
#include "VolumeDisplay.h"
#include <vector>
#include <GL/glut.h>

using namespace std;

DisplayObjectSet::DisplayObjectSet() {
	classType             = ALL;
	left_mouse_held       = false;
	hasNewObjects         = false;
	control_mesh          = false;
	parametric            = false;
	startX                = -1;
	startY                = -1;
	drawX                 = -1;
	drawY                 = -1;
	default_resolution[0] = 40;
	default_resolution[1] = 40;
	default_resolution[2] = 40;
	wallbuffer            = NULL;
}

/**
 * \todo Make sure that non-uniform tesselation resolution is possible by extensive
 *       testing and more than likely followed by some bugfixing.
 */
void DisplayObjectSet::tesselateAll() {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->tesselate(default_resolution);
}
void DisplayObjectSet::tesselateAll(int *n) {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->tesselate(n);
}

void DisplayObjectSet::setActionListener(void (*actionPerformed)(ActiveObject*, int )) {
	this->actionPerformed = actionPerformed;
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->setActionListener(actionPerformed);
}

void DisplayObjectSet::paintSelectionBox() {
	if(!left_mouse_held)
		return;
	int vp[4]; // vp = viewport(x,y,widht,height)
	glGetIntegerv(GL_VIEWPORT, vp);
	double x1 = ((double) startX )/vp[2]*2 - 1;
	double y1 = ((double) startY )/vp[3]*2 - 1;
	double x2 = ((double) drawX  )/vp[2]*2 - 1;
	double y2 = ((double) drawY  )/vp[3]*2 - 1;
	glLineWidth(1);
	glColor3f(0,0,0);
	glBegin(GL_LINE_STRIP);
		glVertex2f(x1,y1);
		glVertex2f(x1,y2);
		glVertex2f(x2,y2);
		glVertex2f(x2,y1);
		glVertex2f(x1,y1);
	glEnd();
}

/*! \todo Figure out why paintAll is called 4 times on startup and on each mousebutton-press */
void DisplayObjectSet::paintAll() {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		if( !(*obj)->isSelected() )
			(*obj)->paint();
	for(set<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		(*obj)->paintSelected();
}

void DisplayObjectSet::paintAllMouseAreas(vector<MVPHandler*> viewpanels) {
	glClearColor(0,0,0,0);
	int width  = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

#if 0  
# ===== THIS IS THE OLD READPIXEL-WAY FOR EVERY OBJECT - TOO SLOW!!!! ======
	
	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// draw every object to get the depth-buffer correct
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj) {
		glViewport(0,0, window_width, window_height);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for(uint i=0; i<viewpanels.size(); i++) {
			viewpanels[i]->setScissortest();
			viewpanels[i]->setViewport();
			viewpanels[i]->setProjection();
			viewpanels[i]->setModelView();
			(*obj)->paintMouseAreas();
		}
		glDisable(GL_SCISSOR_TEST);
		// (*obj)->readMouseAreas();
	}
	// clear color only (depth kept), and repaint masks, and read them to memory
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj) {
		glViewport(0,0, window_width, window_height);
		glClear(GL_COLOR_BUFFER_BIT);
		for(uint i=0; i<viewpanels.size(); i++) {
			viewpanels[i]->setScissortest();
			viewpanels[i]->setViewport();
			viewpanels[i]->setProjection();
			viewpanels[i]->setModelView();
			(*obj)->paintMouseAreas();
		}
		(*obj)->readMouseAreas();
		glDisable(GL_SCISSOR_TEST);
	}
	glDisableClientState(GL_COLOR_ARRAY);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#endif

/*
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->initMouseMasks();
*/

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
	for(uint i=0; i<viewpanels.size(); i++) {
		viewpanels[i]->setScissortest();
		viewpanels[i]->setViewport();
		viewpanels[i]->setProjection();
		viewpanels[i]->setModelView();
		float r, g, b;
		int n = objects.size();
		int obj_i = 1;
		if(n > 32*32*32) {
			cerr << "Too many objects in the scene - rewrite MouseMask-code\n";
			exit(1);
		}
		for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj) {
			r =  (obj_i%32)     / 32.;
			g = ((obj_i/32)%32) / 32.;
			b =  (obj_i/32/32)  / 32.;
			(*obj)->paintMouseAreas(r, g, b);
			obj_i++;
		}
	}
	glDisable(GL_SCISSOR_TEST);
	if(wallbuffer) delete[] wallbuffer;
	wallbuffer = new GLfloat[3*width*height];
	glReadPixels(0, 0, width, height, GL_RGB,  GL_FLOAT, wallbuffer);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

int DisplayObjectSet::objectAtPosition(int x, int y) {
	int width  = glutGet(GLUT_WINDOW_WIDTH);
	int k = (width*y+x)*3;
	int i;
	GLfloat r,g,b;
	r = wallbuffer[k++];
	g = wallbuffer[k++];
	b = wallbuffer[k++];
	i = floor(b*32+.5)*32*32 +
	    floor(g*32+.5)*32 +
		floor(r*32+.5);
	return i-1;
}

void DisplayObjectSet::addObject(DisplayObject* obj) {
	objects.push_back((DisplayObject*) obj);
	obj->tesselate(default_resolution);
	if(actionPerformed) obj->setActionListener(actionPerformed);
	hasNewObjects = true;
}

void DisplayObjectSet::clear() {
	objects.clear();
	selected.clear();
	hidden.clear();
	fireActionEvent(ACTION_REQUEST_REPAINT | ACTION_REQUEST_REMASK);
}

bool DisplayObjectSet::removeObject(DisplayObject* del_obj) {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj < objects.end(); ++obj) {
		if(del_obj == *obj) {
			objects.erase(obj);
			for(set<DisplayObject*>::iterator sel=selected.begin(); sel != selected.end(); ++sel) {
				if(del_obj == *sel) {
					selected.erase(sel);
					break;
				}
			}
			return true;
		}
	}

	for(vector<DisplayObject*>::iterator obj=hidden.begin(); obj != hidden.end(); ++obj) {
		if(del_obj == *obj) {
			hidden.erase(obj);
			return true;
		}
	}
	return false;
}

void DisplayObjectSet::removeSelected() {
	for(set<DisplayObject*>::iterator sel=selected.begin(); sel != selected.end(); ++sel)
		(*sel)->setSelected(false);
	selected.clear();
}

/** \todo make it possible that items appear in BOTH the hidden vector and the objects vector
          in the same way that the selected vector works */

void DisplayObjectSet::hideObjects(DISPLAY_CLASS_TYPE type) {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj < objects.end(); ++obj) {
		if((*obj)->classType() == type || type==ALL) {
			hidden.push_back(*obj);
			objects.erase(obj);
			obj--;
		}
	}
	bool anything_erased ;
	do {
		anything_erased = false;
		for(set<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj) {
			if((*obj)->classType() == type || type==ALL) {
				(*obj)->setSelected(false);
				selected.erase(obj);
				anything_erased = true;
				break;
			}
		}
	} while(anything_erased);
	fireActionEvent(ACTION_REQUEST_REPAINT | ACTION_REQUEST_REMASK);
}

void DisplayObjectSet::unHideObjects(DISPLAY_CLASS_TYPE type) {
	for(vector<DisplayObject*>::iterator obj=hidden.begin(); obj < hidden.end(); ++obj) {
		if((*obj)->classType() == type || type==ALL) {
			objects.push_back(*obj);
			hidden.erase(obj);
			obj--;
		}
	}
	fireActionEvent(ACTION_REQUEST_REPAINT | ACTION_REQUEST_REMASK);
}

void DisplayObjectSet::processMouse(int button, int state, int x, int y) {
	if(control_mesh) return;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		left_mouse_held = true;
		startX = x;
		startY = y;
		drawX  = x;
		drawY  = y;
	} else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if(! (glutGetModifiers() & GLUT_ACTIVE_CTRL) ) {
			for(set<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
				(*obj)->setSelected(false);
			selected.clear();
		}
		if(abs(startX-drawX)+abs(startY-drawY) < 3) {
			int obj_i = objectAtPosition(x,y);
			if(obj_i>=0 && (classType == ALL || objects[obj_i]->classType()==classType)) {
				selected.insert(objects[obj_i]);
			}
		} else {
			int lowx  = (startX<drawX) ? startX : drawX;
			int highx = (startX<drawX) ? drawX  : startX;
			int lowy  = (startY<drawY) ? startY : drawY;
			int highy = (startY<drawY) ? drawY  : startY;
			for(int xi=lowx; xi<=highx; xi++) {
				for(int yi=lowy; yi<=highy; yi++) {
					int obj_i = objectAtPosition(xi,yi);
					if(obj_i>=0 && (classType == ALL || objects[obj_i]->classType()==classType)) {
						selected.insert(objects[obj_i]);
					}
				}
			}
		}

		left_mouse_held = false;
		fireActionEvent(ACTION_REQUEST_REPAINT);
	}
}

void DisplayObjectSet::processMouseActiveMotion(int x, int y) {
	if(!left_mouse_held)
		return;
	for(set<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		if(((*obj)->classType()==classType || classType==ALL) )
			(*obj)->processMouseActiveMotion(x,y);
	drawX = x;
	drawY = y;
	fireActionEvent(ACTION_REQUEST_REPAINT);
}

void DisplayObjectSet::processMousePassiveMotion(int x, int y) {
}

void DisplayObjectSet::setOnlySelectable(DISPLAY_CLASS_TYPE classType) {
	this->classType = classType;
}

vector<DisplayObject*> DisplayObjectSet::getSelectedObjects() {
	return getSelectedObjects(ALL);
}

vector<DisplayObject*> DisplayObjectSet::getSelectedObjects(DISPLAY_CLASS_TYPE classType) {
	vector<DisplayObject*> results;
	for(set<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		if(((*obj)->classType()==classType || classType==ALL) )
			results.push_back(*obj);
	
	return results;
}

/*
DisplayObject* DisplayObjectSet::getSelected(DISPLAY_CLASS_TYPE classType) {
	for(vector<DisplayObject*>::size_type i=0; i<selected.size(); i++)
		if((selected[i]->classType()==classType || classType==ALL) )
			return selected[i];
	
	return NULL;
}
*/
void DisplayObjectSet::changeParametric() {
	parametric = !parametric;
}
void DisplayObjectSet::changeControlMesh() {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->setDrawControlMesh(!control_mesh);
	control_mesh = !control_mesh;
}

vector<DisplayObject*>::iterator DisplayObjectSet::objects_begin() {
	return objects.begin();
}
vector<DisplayObject*>::iterator DisplayObjectSet::objects_end() {
	return objects.end();
}

void DisplayObjectSet::setColor(DISPLAY_CLASS_TYPE type, float r, float g, float b) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++)
		if((*it)->classType() == type || type == ALL)
			(*it)->setColor(r,g,b);
	for(it=hidden.begin(); it!=hidden.end(); it++)
		if((*it)->classType() == type || type == ALL)
			(*it)->setColor(r,g,b);
}

PointDisplay* DisplayObjectSet::getDisplayObject(Go::Point *p) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++) {
		if((*it)->classType() == POINT) {
			PointDisplay* pd = (PointDisplay*) *it;
			if(pd->point.dist(*p) < 1e-4)
				return pd;
		}
	}
	for(it=hidden.begin(); it!=hidden.end(); it++) {
		if((*it)->classType() == POINT) {
			PointDisplay* pd = (PointDisplay*) *it;
			if(pd->point.dist(*p) < 1e-4)
				return pd;
		}
	}
	return NULL;
}

CurveDisplay* DisplayObjectSet::getDisplayObject(Go::SplineCurve *c) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++) {
		if((*it)->classType() == CURVE) {
			CurveDisplay* cd = (CurveDisplay*) *it;
			if(cd->curve == c)
				return cd;
		}
	}
	for(it=hidden.begin(); it!=hidden.end(); it++) {
		if((*it)->classType() == CURVE) {
			CurveDisplay* cd = (CurveDisplay*) *it;
			if(cd->curve == c)
				return cd;
		}
	}
	return NULL;
}

SurfaceDisplay* DisplayObjectSet::getDisplayObject(Go::SplineSurface *s) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++) {
		if((*it)->classType() == SURFACE) {
			SurfaceDisplay* sd = (SurfaceDisplay*) *it;
			if(sd->surf == s)
				return sd;
		}
	}
	for(it=hidden.begin(); it!=hidden.end(); it++) {
		if((*it)->classType() == SURFACE) {
			SurfaceDisplay* sd = (SurfaceDisplay*) *it;
			if(sd->surf == s)
				return sd;
		}
	}
	return NULL;
}

VolumeDisplay* DisplayObjectSet::getDisplayObject(Go::SplineVolume *v) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++) {
		if((*it)->classType() == VOLUME) {
			VolumeDisplay* vd = (VolumeDisplay*) *it;
			if(vd->volume == v)
				return vd;
		}
	}
	for(it=hidden.begin(); it!=hidden.end(); it++) {
		if((*it)->classType() == VOLUME) {
			VolumeDisplay* vd = (VolumeDisplay*) *it;
			if(vd->volume == v)
				return vd;
		}
	}
	return NULL;
}

void DisplayObjectSet::setLineWidth(int width) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++)
		if((*it)->classType() == CURVE)
			((CurveDisplay*)*it)->setLineWidth(width);
}

void DisplayObjectSet::setPointSize(int size) {
	vector<DisplayObject*>::iterator it;
	for(it=objects.begin(); it!=objects.end(); it++)
		if((*it)->classType() == POINT)
			((PointDisplay*)*it)->setPointSize(size);
}
