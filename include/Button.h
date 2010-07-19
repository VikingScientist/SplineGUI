
#ifndef _BUTTON_H
#define _BUTTON_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <string>
#include <GL/glut.h>

using namespace std;

class Button : public MouseListener, ActiveObject {

	public:
		Button(string text, int x, int y, int width, int height);

		void paint();
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void onEnter() ;
		void onExit() ;

		int x,y;
		int width, height;
	private:
		void glutPrint(float x, float y, void* font, string text, float r, float g, float b, float a);

		string text;
		GLfloat r,g,b;
		int bevel_size;
		bool pressed;
};

#endif
