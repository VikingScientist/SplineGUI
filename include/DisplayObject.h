#ifndef _DISPLAY_OBJECT_H
#define _DISPLAY_OBJECT_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <iostream>
#include <GoTools/utils/BoundingBox.h>

// openGL headers
#include <GL/glut.h>

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
		const char* meta;

	public:
		DisplayObject() {
			selected = false;
			selected_color_specified = false;
			origin   = NULL;
			meta = "";
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
		virtual void addActionListener(void (*actionPerformed)(ActiveObject*, int )) { ActiveObject::addActionListener(actionPerformed); }
		virtual void getBoundingBox(Go::BoundingBox &box) const { }

		// trying diffent things with the mouse-masks
		virtual void initMouseMasks() { }
		virtual void setMaskPos(int x, int y, bool value) { }
		virtual void paintMouseAreas(float r, float g, float b) { }

		void glutPrint(float x, float y, const char* text, float r, float g, float b, float a)
		{
			bool blending = false;
			if(glIsEnabled(GL_BLEND)) blending = true;
			glEnable(GL_BLEND);
			glColor4f(r,g,b,a);
			glRasterPos2f(x,y);
			int i = 0;
			while(text[i] != 0)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i++]);
			if(!blending) glDisable(GL_BLEND);
		}  

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
		void setMeta(const char *meta_text) {
			meta = meta_text;
		}
		void printMeta() {
			std::cout  << meta << std::endl;
		}
		virtual void paintMeta(int x, int y) {
			int stringLength = -1;
			while(meta[++stringLength] != 0)  ;

			if(stringLength==0)
				return;

			int vp[4]; // vp = viewport(x,y,widht,height)
			glGetIntegerv(GL_VIEWPORT, vp);
			double xs, ys;
			double x1 = x - 5;
			double y1 = y - 5;
			double x2 = x + stringLength * 6 + 5;
			double y2 = y + 12 + 5;
			xs = ((double) x  )/vp[2]*2 - 1;
			ys = ((double) y  )/vp[3]*2 - 1;
			x1 = ((double) x1 )/vp[2]*2 - 1;
			y1 = ((double) y1 )/vp[3]*2 - 1;
			x2 = ((double) x2 )/vp[2]*2 - 1;
			y2 = ((double) y2 )/vp[3]*2 - 1;
			glColor3f(1,.921875,.5); // light orange, beige
			glBegin(GL_QUADS);
				glVertex2f(x1, y1);
				glVertex2f(x2, y1);
				glVertex2f(x2, y2);
				glVertex2f(x1, y2);
			glEnd();
			glColor3f(0,0,0);
			glLineWidth(1);
			glBegin(GL_LINE_LOOP);
				glVertex2f(x1, y1);
				glVertex2f(x2, y1);
				glVertex2f(x2, y2);
				glVertex2f(x1, y2);
			glEnd();
			
			glutPrint(xs,ys, meta, 0,0,0,1);
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
		virtual void drawMeta(int x, int y) {
			std::cout << meta;

		}

};

#endif
