#include "MouseListener.h"
#include <GL/glut.h>
#include <iostream>

using namespace std;

MouseListener::MouseListener() {
	this->x                 = 0;
	this->y                 = 0;
	this->width             = 0;
	this->height            = 0;
	hover                   = false;
	catchAll                = true;
	right_mouse_button_down = false;
	left_mouse_button_down  = false;
	specialKey              = 0;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
}

MouseListener::MouseListener(int x, int y, int width, int height) {
	this->x                 = x;
	this->y                 = y;
	this->width             = width;
	this->height            = height;
	hover                   = false;
	catchAll                = false;
	right_mouse_button_down = false;
	left_mouse_button_down  = false;
	specialKey              = 0;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
}

void MouseListener::setSize(int x, int y, int width, int height) {
	this->x      = x;
	this->y      = y;
	this->width  = width;
	this->height = height;
	catchAll     = false;
}

void MouseListener::processMouse(int button, int state, int x, int y)  {
	specialKey = glutGetModifiers();
	if(button == GLUT_RIGHT_BUTTON) {
		if(state == GLUT_UP)
			right_mouse_button_down = false;
		else
			right_mouse_button_down = true;
	} else if( button == GLUT_LEFT_BUTTON ) {
		if(state == GLUT_UP)
			left_mouse_button_down = false;
		else
			left_mouse_button_down = true;
	}
	last_mouse_x = x;
	last_mouse_y = y;
}

void MouseListener::processMouseActiveMotion(int x, int y)  {
	last_mouse_x = x;
	last_mouse_y = y;
}

void MouseListener::processMousePassiveMotion(int x, int y)  {
	last_mouse_x = x;
	last_mouse_y = y;
}
