#ifndef _DISPLAY_OBJECT_H
#define _DISPLAY_OBJECT_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <iostream>

enum DISPLAY_CLASS_TYPE {
	NONE,
	ALL,
	POINT,
	CURVE_POINT,
	CURVE,
	SURFACE,
	VOLUME
};
	

class DisplayObject : public MouseListener, ActiveObject {

	private:
		bool selected;
		bool selected_color_specified;
		DisplayObject *origin;

	protected:
		double color[3];
		double selected_color[3];

	public:
		DisplayObject() {
			selected = false;
			selected_color_specified = false;
			origin   = NULL;
		}
		~DisplayObject() { }
		// all below methods SHOULD be overwritten in all implementing classes (java interface-like)
		virtual void tesselate(int *n=NULL) { }
		virtual void paint() { }
		virtual void paintSelected() { }
		virtual void paintMouseAreas() { }
		virtual void readMouseAreas() { }
		virtual DISPLAY_CLASS_TYPE classType() { return NONE; }
		virtual bool isAtPosition(int x, int y) { return false; }
		virtual void processMouse(int button, int state, int x, int y) { }
		virtual void processMouseActiveMotion(int x, int y) { }
		virtual void processMousePassiveMotion(int x, int y) { }
		virtual void printDebugInfo() { }
		virtual void print(std::ostream *out) { }
		virtual void setDrawControlMesh(bool draw) { }
		virtual void setColorByParameterValues(bool show_color) { }
		virtual void setActionListener(void (*actionPerformed)(ActiveObject*, int )) { ActiveObject::setActionListener(actionPerformed); }

		// trying diffent things with the mouse-masks
		virtual void initMouseMasks() { }
		virtual void setMaskPos(int x, int y, bool value) { }
		virtual void paintMouseAreas(float r, float g, float b) { }

		// no need to overwrite these
		DisplayObject* getOrigin() {
			return origin;
		}
		void setOrigin(DisplayObject *o) {
			origin = o;
		}
		void setSelected(bool selected) {
			this->selected = selected;
		}
		bool isSelected() {
			return selected;
		}
		virtual void setSelectedColor(double r, double g, double b) {
			selected_color[0] = r;
			selected_color[1] = g;
			selected_color[2] = b;
			selected_color_specified = true;
			fireActionEvent(ACTION_REQUEST_REPAINT);
		}
		virtual void setColor(double r, double g, double b) {
			color[0] = r;
			color[1] = g;
			color[2] = b;
			if(!selected_color_specified)
				for(int i=0; i<3; i++)
					selected_color[i] = (color[i]+.4 > 1) ? 1 : color[i]+.4;
			fireActionEvent(ACTION_REQUEST_REPAINT);
		}

};

#endif
