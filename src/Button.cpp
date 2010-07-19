
#include "Button.h"
#include <iostream>

Button::Button(string text, int x, int y, int width, int height) : MouseListener(x,y,width,height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->text = text;
	r = 0.6;
	g = 0.6;
	b = 0.6;
	bevel_size = 4; // given in pixels
	pressed = false;
}

void Button::paint() {
	int vp[4]; // vp = viewport(x,y,widht,height)
	glGetIntegerv(GL_VIEWPORT, vp);
	double x1 = ((double)x       )/vp[2]*2 - 1;
	double x2 = ((double)x+width )/vp[2]*2 - 1;
	double y2 = ((double)y       )/vp[3]*2 - 1;
	double y1 = ((double)y+height)/vp[3]*2 - 1;
	// double y2 = (vp[3]-(double)y-height)/vp[3]*2 - 1;
	double bev_x1 = ((double)x+bevel_size        )/vp[2]*2 - 1;
	double bev_x2 = ((double)x+width-bevel_size  )/vp[2]*2 - 1;
	double bev_y2 = ((double)y+bevel_size        )/vp[3]*2 - 1;
	double bev_y1 = ((double)y+height-bevel_size )/vp[3]*2 - 1;
	// double bev_y1 = (vp[3]-(double)y-bevel_size       )/vp[3]*2 - 1;
	// double bev_y2 = (vp[3]-(double)y-height+bevel_size)/vp[3]*2 - 1;

	glBegin(GL_QUADS);
		// main button
		glColor3f(r,g,b);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
		// top bevel
		if(pressed)
			glColor3f(r-.4, g-.4, b-.4);
		else
			glColor3f(r+.3, g+.3, b+.3);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(bev_x2, bev_y1);
		glVertex2f(bev_x1, bev_y1);
		// left bevel
		if(pressed)
			glColor3f(r-.3, g-.3, b-.3);
		else
			glColor3f(r+.2, g+.2, b+.2);
		glVertex2f(x1, y1);
		glVertex2f(bev_x1, bev_y1);
		glVertex2f(bev_x1, bev_y2);
		glVertex2f(x1, y2);
		// right bevel
		if(pressed)
			glColor3f(r+.2, g+.2, b+.2);
		else
			glColor3f(r-.3, g-.3, b-.3);
		glVertex2f(bev_x2, bev_y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(bev_x2, bev_y2);
		// bottom bevel
		if(pressed)
			glColor3f(r+.3, g+.3, b+.3);
		else
			glColor3f(r-.4, g-.4, b-.4);
		glVertex2f(bev_x1, bev_y2);
		glVertex2f(bev_x2, bev_y2);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
	glEnd();
	glColor3f(0,0,0);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);
	glEnd();
	// guessing helvetica_12 letters are around 6x12 pixels
	if(pressed)
		glutPrint((float) width/2-text.length()/2.0*6+1, height/2-5, GLUT_BITMAP_HELVETICA_12, text, 0, 0, 0, 1);
	else
		glutPrint((float) width/2-text.length()/2.0*6, height/2-4, GLUT_BITMAP_HELVETICA_12, text, 0, 0, 0, 1);
		
}

void Button::glutPrint(float x, float y, void* font, string text, float r, float g, float b, float a)
{
	int vp[4]; // vp = viewport(x,y,widht,height)
	glGetIntegerv(GL_VIEWPORT, vp);
	x += this->x;
	y += this->y;
	double x1 = (x )/vp[2]*2 - 1;
	double y1 = (y )/vp[3]*2 - 1;
	bool blending = false;
	if(glIsEnabled(GL_BLEND)) blending = true;
	glEnable(GL_BLEND);
	glColor4f(r,g,b,a);
	glRasterPos2f(x1,y1);
	for(unsigned int i=0; i<text.length(); i++) 
		glutBitmapCharacter(font, text[i]);
	if(!blending) glDisable(GL_BLEND);
}  

void Button::processMouse(int button, int state, int x, int y) {
	// should have an event trigger on mouse exit (pressed=false, no action, request repaint)
	if(state == GLUT_DOWN && !pressed) {
		pressed = true;
		bevel_size += 1;
		fireActionEvent(ACTION_REQUEST_REPAINT);
	} else if(state == GLUT_UP && pressed) {
		pressed = false;
		bevel_size -= 1;
		fireActionEvent(ACTION_BUTTON_PRESSED | ACTION_REQUEST_REPAINT);
	}
}
void Button::processMouseActiveMotion(int x, int y) {

}
void Button::processMousePassiveMotion(int x, int y) {

}
void Button::onEnter() {
	MouseListener::onEnter();
}
void Button::onExit() {
	MouseListener::onExit();
	if(pressed) {
		bevel_size -= 1;
		pressed = false;
		fireActionEvent(ACTION_REQUEST_REPAINT);
	}
}
