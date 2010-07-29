#ifndef _DISPLAY_OBJECT_SET_H
#define _DISPLAY_OBJECT_SET_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include "DisplayObject.h" 
#include "MVPHandler.h" 
#include <vector>
#include <set>

using namespace std;

class DisplayObjectSet : public MouseListener, ActiveObject {

	public:
		DisplayObjectSet();
		void tesselateAll();
		void tesselateAll(int *n);
		void paintAll();
		void paintAllMouseAreas(vector<MVPHandler*> viewpanels);
		void addObject(DisplayObject* obj);
		bool removeObject(DisplayObject* obj);
		void removeSelected();
		void hideObjects(DISPLAY_CLASS_TYPE type);
		void unHideObjects(DISPLAY_CLASS_TYPE type);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void setOnlySelectable(DISPLAY_CLASS_TYPE classType);
		void changeControlMesh();
		void changeParametric();
		void setActionListener(void (*actionPerformed)(ActiveObject*, int )) ;
		vector<DisplayObject*> getSelectedObjects();
		vector<DisplayObject*> getSelectedObjects(DISPLAY_CLASS_TYPE classType);
		vector<DisplayObject*>::iterator objects_begin();
		vector<DisplayObject*>::iterator objects_end();


		bool hasNewObjects;

	private:
		int objectAtPosition(int x, int y);

		vector<DisplayObject*> objects;
		set<DisplayObject*> selected;
		vector<DisplayObject*> hidden;
		DISPLAY_CLASS_TYPE classType;
		int default_resolution[3];
		bool control_mesh;
		bool left_mouse_held;
		bool parametric;
		GLfloat *wallbuffer;
};

#endif

