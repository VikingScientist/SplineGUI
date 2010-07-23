#ifndef _DISPLAY_OBJECT_H
#define _DISPLAY_OBJECT_H

#include "MouseListener.h"
#include <iostream>

using namespace std;

enum DISPLAY_CLASS_TYPE {
	NONE,
	ALL,
	POINT,
	CURVE_POINT,
	CURVE,
	SURFACE,
	VOLUME
};
	

class DisplayObject : public MouseListener {

	private:
		bool selected;
		DisplayObject *origin;

	public:
		DisplayObject() {
			selected = false;
			origin   = NULL;
		}
		~DisplayObject() { }
		// all below methods SHOULD be overwritten in all implementing classes (java interface-like)
		virtual void tesselate(int *n) { }
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
		virtual void print(ostream *out) { }
		virtual void setDrawControlMesh(bool draw) { }

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

};

#endif
