
#ifndef _BUTTON_H
#define _BUTTON_H

#include "MouseListener.h"
#include "ActiveObject.h"
#include <string>
#include <GL/glut.h>

/**
  * \todo Distinguish between the functions that the user needs access to and the functions
  *       that the GUI engine needs access to.
  */

class Button : public MouseListener, ActiveObject {

	public:
		Button(std::string text);
		Button(std::string text, int x, int y, int width, int height);

		void paint();
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void onEnter(int x, int y) ;
		void onExit(int x, int y) ;
		void setOnClick(void (*onClick)(Button*)) ;
		void makeOnOffButton();
		void setSelected(bool on);

		std::string getText() const;
		bool isSelected() const;

		void setSizeAndPos(int x, int y, int width, int height) ;

	private:
		void glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a);

		void (*onClick)(Button*);
		std::string text;
		int x,y;
		int width, height;
		GLfloat r,g,b;
		int bevel_size;
		bool pressed;
		bool isOnOffButton;
};

#endif
