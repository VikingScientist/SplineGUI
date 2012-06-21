#ifndef _MVPHANDLER_H
#define _MVPHANDLER_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <string>
#include <GL/gl.h>

class MVPHandler : public MouseListener, ActiveObject {

	protected:
		GLint vp_width;   // viewport width (in pixels)
		GLint vp_height;  // viewport height (in pixels)
		bool just_warped; // true if mouse pointer was set by GLUT (invalidating last_mouse_xy)

		virtual void fireActionEvent(int meta) { ActiveObject::fireActionEvent(meta); };

	public:
		MVPHandler() { };
		MVPHandler(int x, int y, int w, int h) : MouseListener(x,y,w,h) { };
		~MVPHandler() { } ;
		
		virtual void processMouse(int button, int state, int x, int y) ;
		virtual void processMouseActiveMotion(int x, int y) ;
		virtual void processMousePassiveMotion(int x, int y) ;
		virtual void onEnter(int x, int y);
		virtual void onExit(int x, int y) ;

		void setScissortest() ;
		void setViewport() ;
		virtual void setModelView() { }
		virtual void setProjection() { }
		virtual void handleResize(int x, int y, int w, int h) ;
		virtual void paintBackground() { }
		virtual void paintMeta() { }
		void glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a) ;
};

#endif
