#ifndef _DISPLAY_OBJECT_SET_H
#define _DISPLAY_OBJECT_SET_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include "DisplayObject.h" 
#include "MVPHandler.h" 
#include <vector>
#include <set>
#include "CurvePoint.h"
#include "CurveDisplay.h"
#include "SurfaceDisplay.h"
#include "VolumeDisplay.h"

class DisplayObjectSet : public MouseListener, ActiveObject {

	public:
		DisplayObjectSet();
		void tesselateAll();
		void tesselateAll(int *n);
		void paintMetaInfoBox(int x, int y);
		void paintSelectionBox();
		void paintAll();
		void paintAllMouseAreas(std::vector<MVPHandler*> viewpanels);
		void addObject(DisplayObject* obj);
		void clear();
		bool removeObject(DisplayObject* obj);
		void removeSelected();
		void hideObjects(DISPLAY_CLASS_TYPE type);
		void unHideObjects(DISPLAY_CLASS_TYPE type);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void setOnlySelectable(DISPLAY_CLASS_TYPE classType);
		void enableControlMesh(bool enabled);
		void changeControlMesh();
		void colorSelectedByParameterValues(bool show_color);
		void changeParametric();
		void setActionListener(void (*actionPerformed)(ActiveObject*, int )) ;
		void setColor(DISPLAY_CLASS_TYPE type, float r, float g, float b);
		void setLineWidth(int width);
		void setPointSize(int size);

		std::vector<DisplayObject*> getSelectedObjects();
		std::vector<DisplayObject*> getSelectedObjects(DISPLAY_CLASS_TYPE classType);
		std::vector<DisplayObject*>::iterator objects_begin();
		std::vector<DisplayObject*>::iterator objects_end();

		PointDisplay*   getDisplayObject(Go::Point *p);
		CurveDisplay*   getDisplayObject(Go::SplineCurve *c);
		SurfaceDisplay* getDisplayObject(Go::SplineSurface *s);
		VolumeDisplay*  getDisplayObject(Go::SplineVolume *v);

		DisplayObject*  getObjectAt(int x, int y);

		bool hasNewObjects;

	private:
		int objectAtPosition(int x, int y);

		std::vector<DisplayObject*> objects;
		std::set<DisplayObject*> selected;
		std::vector<DisplayObject*> hidden;
		DISPLAY_CLASS_TYPE classType;
		int default_resolution[3];
		bool no_control_mesh;
		bool control_mesh;
		bool left_mouse_held;
		int startX, startY;
		int drawX, drawY;
		GLfloat *wallbuffer;
};

#endif

