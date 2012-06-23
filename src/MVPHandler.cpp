#include "MVPHandler.h"
#include <GL/glut.h>

void MVPHandler::setScissortest() { 
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height);
}

void MVPHandler::setViewport() { 
	glViewport(x, y, width, height);
}

void MVPHandler::handleResize(int x, int y, int w, int h) { 
	MouseListener::setSize(x,y,w,h);
}

void MVPHandler::glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a)
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

void MVPHandler::processMouse(int button, int state, int x, int y) {
	MouseListener::processMouse(button, state, x, y);
}

void MVPHandler::processMouseActiveMotion(int x, int y) {
	MouseListener::processMouseActiveMotion(x, y);
}
void MVPHandler::processMousePassiveMotion(int x, int y) {
	MouseListener::processMousePassiveMotion(x, y);
}

void MVPHandler::onEnter(int x, int y) {
	MouseListener::onEnter(x,y);
}

void MVPHandler::onExit(int x, int y) {
	if(right_mouse_button_down) { // navigation (i.e. zooming, paning, rotating)
		if(specialKey == GLUT_ACTIVE_CTRL) {
			if(y >= vp_height) {
				int window_height = glutGet(GLUT_WINDOW_HEIGHT);
				just_warped = true;
				last_mouse_y = 0;
				glutWarpPointer(x+MouseListener::getX(), window_height - MouseListener::getY());
			} else if(y <= 0) {
				int window_height = glutGet(GLUT_WINDOW_HEIGHT);
				just_warped = true;
				last_mouse_y = vp_height;
				glutWarpPointer(x+MouseListener::getX(), window_height - MouseListener::getY()-vp_height);
			}
		}
	}
	MouseListener::onExit(x, y);
}

