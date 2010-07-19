
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
	default_resolution[0] = 40;
	default_resolution[1] = 40;
	default_resolution[2] = 40;
}

/**
 * \todo Make sure that non-uniform tesselation resolution is possible by extensive
 *       testing and more than likely followed by some bugfixing.
 */
void DisplayObjectSet::tesselateAll() {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->tesselate(default_resolution);
	/*
	for(vector<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		(*obj)->tesselate(default_resolution);
	*/
}
void DisplayObjectSet::tesselateAll(int *n) {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		(*obj)->tesselate(n);
	/*
	for(vector<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		(*obj)->tesselate(n);
	*/
}

void DisplayObjectSet::paintAll() {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj)
		if( !(*obj)->isSelected() )
			(*obj)->paint();
	for(vector<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		(*obj)->paintSelected();
}

void DisplayObjectSet::paintAllMouseAreas(vector<MVPHandler*> viewpanels) {
	glClearColor(0,0,0,0);
	int window_width  = glutGet(GLUT_WINDOW_WIDTH);
	int window_height = glutGet(GLUT_WINDOW_HEIGHT);
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
	/*
	for(vector<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj)
		(*obj)->paintMouseAreas();
	*/
}

void DisplayObjectSet::addObject(DisplayObject* obj) {
	objects.push_back((DisplayObject*) obj);
	obj->tesselate(default_resolution);
	hasNewObjects = true;
	// fireActionEvent(GEOMETRY_MOVE_STOPPED);
}

bool DisplayObjectSet::removeObject(DisplayObject* del_obj) {
	for(vector<DisplayObject*>::iterator obj=objects.begin(); obj < objects.end(); ++obj) {
		if(del_obj == *obj) {
			objects.erase(obj);
			for(vector<DisplayObject*>::iterator sel=selected.begin(); sel < selected.end(); ++sel)
				if(del_obj == *sel)
					selected.erase(sel);
			return true;
		}
	}

/*
	for(vector<DisplayObject*>::iterator obj=selected.begin(); obj != selected.end(); ++obj) {
		if(del_obj == *obj) {
			selected.erase(obj);
			return true;
		}
	}
*/
	return false;
}

void DisplayObjectSet::removeSelected() {
	for(uint i=0; i<selected.size(); i++)
		selected[i]->setSelected(false);
	selected.clear();
}

void DisplayObjectSet::processMouse(int button, int state, int x, int y) {
	if(control_mesh) return;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		left_mouse_held = true;
		// if ctrl is not pressed. Move every (previously marked selected
		// object back into the "objects" vector to empty selection
		if(! (glutGetModifiers() & GLUT_ACTIVE_CTRL) ) {
			for(uint i=selected.size(); i-->0; ) {
				if(classType == ALL || selected[i]->classType() == classType) {
					selected[i]->setSelected(false);
					selected.erase(selected.begin()+i);
				}
			}
		}
		/*
			for(vector<DisplayObject*>::size_type i=selected.size(); i-->0; )  {
				DisplayObject *obj = selected[i];
				if(classType == ALL || obj->classType() == classType) {
					objects.push_back(obj);
					selected.erase(selected.begin()+i);
				}
			}
		}
		*/
		for(vector<DisplayObject*>::iterator obj=objects.begin(); obj != objects.end(); ++obj) {
			if( (glutGetModifiers()       &  GLUT_ACTIVE_SHIFT) &&
			    (*obj)->classType()       == VOLUME             &&
			    (*obj)->isAtPosition(x,y)                       &&
				(classType==ALL           || classType==SURFACE) ) {
				DisplayObject* surf_edge = (DisplayObject*) ((VolumeDisplay*)(*obj))->getSurfaceAt(x,y);
				selected.push_back( surf_edge );
				surf_edge->setSelected(true);
			} else if((*obj)->isAtPosition(x,y) && (classType==ALL || classType==(*obj)->classType()) ) {
				selected.push_back( (*obj ));
				(*obj)->setSelected(true);
			}
		}

		/*
		vector<int> newly_selected;
		for(vector<DisplayObject*>::size_type i=0; i<objects.size(); i++) {
			if( glutGetModifiers()            == GLUT_ACTIVE_SHIFT &&
			    objects[i]->classType()       == VOLUME            &&
			    objects[i]->isAtPosition(x,y)                      &&
				(classType==ALL               || classType==SURFACE) ) {
				selected.push_back( (DisplayObject*) ((VolumeDisplay*)objects[i])->getSurfaceAt(x,y) );
			} else if(objects[i]->isAtPosition(x,y) && (classType==ALL || classType==objects[i]->classType())) {
				newly_selected.push_back(i);
			}
		}

		// newly_selected is sorted... since we are erasing things from
		// the vector "objects", we need to iterate in reverse direction
		// to not destroy the indices
		for(vector<int>::reverse_iterator i=newly_selected.rbegin(); i!=newly_selected.rend(); i++) {
			selected.push_back(objects[*i]);
			vector<DisplayObject*>::iterator delete_pos = objects.begin() + *i;
			objects.erase(delete_pos);
		}
		*/
		fireActionEvent(ACTION_REQUEST_REPAINT);
	} else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_mouse_held = false;
		fireActionEvent(ACTION_REQUEST_REPAINT);
	}
}

void DisplayObjectSet::processMouseActiveMotion(int x, int y) {
	if(!left_mouse_held)
		return;
	for(vector<DisplayObject*>::size_type i=0; i<selected.size(); i++)
		if((selected[i]->classType()==classType || classType==ALL) )
			selected[i]->processMouseActiveMotion(x,y);
}

void DisplayObjectSet::processMousePassiveMotion(int x, int y) {
}

void DisplayObjectSet::setOnlySelectable(DISPLAY_CLASS_TYPE classType) {
	this->classType = classType;
}

vector<DisplayObject*> DisplayObjectSet::getSelectedObjects() {
	return selected;
}

vector<DisplayObject*> DisplayObjectSet::getSelectedObjects(DISPLAY_CLASS_TYPE classType) {
	vector<DisplayObject*> results;
	for(vector<DisplayObject*>::size_type i=0; i<selected.size(); i++)
		if((selected[i]->classType()==classType || classType==ALL) )
			results.push_back(selected[i]);
	
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
