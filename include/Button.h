
#ifndef _BUTTON_H
#define _BUTTON_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <string>
#include <GL/glut.h>

using namespace std;

/**
  * \todo Distinguish between the functions that the user needs access to and the functions
  *       that the GUI engine needs access to.
  */

class Button : public MouseListener, ActiveObject {

	public:
		Button(string text);
		Button(string text, int x, int y, int width, int height);

		void paint();
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void onEnter() ;
		void onExit() ;
		void setOnClick(void (*onClick)(Button*)) ;

		string getText() const;

		void setSizeAndPos(int x, int y, int width, int height) ;

	private:
		void glutPrint(float x, float y, void* font, string text, float r, float g, float b, float a);

		void (*onClick)(Button*);
		string text;
		int x,y;
		int width, height;
		GLfloat r,g,b;
		int bevel_size;
		bool pressed;
};

#endif
