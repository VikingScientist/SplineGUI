#ifndef _MVPHANDLER_H
#define _MVPHANDLER_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <string>
#include <GL/gl.h>
#include <GL/glut.h>

class MVPHandler : public MouseListener, ActiveObject {

	private:
		int x, y;
		int w, h;

	protected:
		virtual void fireActionEvent(int meta) { ActiveObject::fireActionEvent(meta); };

	public:
		MVPHandler() { };
		MVPHandler(int x, int y, int w, int h) : MouseListener(x,y,w,h) {
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}
		~MVPHandler() { } ;
		
		virtual void processMouse(int button, int state, int x, int y) { }
		virtual void processMouseActiveMotion(int x, int y) { }
		virtual void processMousePassiveMotion(int x, int y) { }
		virtual void onEnter() { MouseListener::onEnter(); }
		virtual void onExit()  { MouseListener::onExit();  }

		void setScissortest() { 
			glEnable(GL_SCISSOR_TEST);
			glScissor(x, y, w, h);
		}
		void setViewport() { 
			glViewport(x, y, w, h);
		}
		virtual void setModelView() { }
		virtual void setProjection() { }
		virtual void handleResize(int x, int y, int w, int h) { 
			MouseListener::setSize(x,y,w,h);
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}
		virtual void paintBackground() { }
		virtual void paintMeta() { }
		void glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a)
		{
			bool blending = false;
			if(glIsEnabled(GL_BLEND)) blending = true;
			glEnable(GL_BLEND);
			glColor4f(r,g,b,a);
			glRasterPos2f(x,y);
			for(unsigned int i=0; i<text.length(); i++) 
				glutBitmapCharacter(font, text[i]);
			if(!blending) glDisable(GL_BLEND);
		}  
};

#endif
