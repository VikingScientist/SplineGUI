//==============================================================================
//!
//! \file Button.cpp
//!
//! \date July 2010
//!
//! \author Kjetil A. Johannessen / SINTEF
//!
//! \brief Button primitive for GUI control
//!
//! 
//==============================================================================


#include "Button.h"
#include <iostream>

/**********************************************************************************//**
 * \brief Constructor
 * \param text Text which is to appear on the button
 *************************************************************************************/
Button::Button(std::string text) : MouseListener(0,0,0,0) {
	this->x      = 0;
	this->y      = 0;
	this->width  = 0;
	this->height = 0;
	this->text = text;
	r = 0.6;
	g = 0.6;
	b = 0.6;
	bevel_size = 4; // given in pixels
	pressed = false;
	isOnOffButton = false;
	onClick = NULL;
}

/**********************************************************************************//**
 * \brief Constructor
 * \param text Text which is to appear on the button
 * \param x Absolute x-position in the GLU window for the button
 * \param y Absolute y-position in the GLU window for the button
 * \param width The horizontal size of the button
 * \param height The vertical size of the button
 *************************************************************************************/
Button::Button(std::string text, int x, int y, int width, int height) : MouseListener(x,y,width,height) {
	this->x      = x;
	this->y      = y;
	this->width  = width;
	this->height = height;
	this->text   = text;
	r = 0.6;
	g = 0.6;
	b = 0.6;
	bevel_size = 4; // given in pixels
	pressed = false;
	onClick = NULL;
}

/**********************************************************************************//**
 * \brief draw method
 *
 * Called whenever the button needs to be drawn on screen
 *************************************************************************************/
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

/**********************************************************************************//**
 * \brief Print GLUT text
 * \param x position of the lower left corner of the text start
 * \param y position of the lower left corner of the text start
 * \param font font used for text display
 * \param text the displayed text
 * \param r red color 
 * \param g green color
 * \param b blue color
 * \param a opacity alpha 
 *************************************************************************************/
void Button::glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a)
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

/**********************************************************************************//**
 * \brief Mouse action event
 * \param button which mouse button was pressed or released
 * \param state button pressed or released
 * \param x position of the mouse at the time of use
 * \param y position of the mouse at the time of use
 *************************************************************************************/
void Button::processMouse(int button, int state, int x, int y) {
	if(button != GLUT_LEFT_BUTTON) return;
	if(state == GLUT_DOWN && !pressed) {
		pressed = true;
		bevel_size += 1;
		if(isOnOffButton && onClick != NULL)
			onClick(this);
		fireActionEvent(ACTION_REQUEST_REPAINT);
	} else if(state == GLUT_UP && pressed && !isOnOffButton) {
		pressed = false;
		bevel_size -= 1;
		fireActionEvent(ACTION_BUTTON_PRESSED | ACTION_REQUEST_REPAINT);
		if(onClick != NULL)
			onClick(this);
	} else if(state == GLUT_DOWN && pressed && isOnOffButton) {
		pressed = false;
		bevel_size -= 1;
		fireActionEvent(ACTION_REQUEST_REPAINT);
		if(onClick != NULL)
			onClick(this);
	}
}

void Button::processMouseActiveMotion(int x, int y) {

}
void Button::processMousePassiveMotion(int x, int y) {

}
void Button::onEnter(int x, int y) {
	MouseListener::onEnter(x,y);
}
void Button::onExit(int x, int y) {
	MouseListener::onExit(x,y);
	if(pressed && !isOnOffButton) {
		bevel_size -= 1;
		pressed = false;
		fireActionEvent(ACTION_REQUEST_REPAINT);
	}
}

void Button::setSizeAndPos(int x, int y, int width, int height) {
	this->x      = x;
	this->y      = y;
	this->width  = width;
	this->height = height;
	MouseListener::setSize(x,y,width,height);
}

/**********************************************************************************//**
 * \brief Button will be stay in "pushed" state until another click for release
 *************************************************************************************/
void Button::makeOnOffButton() {
	isOnOffButton = true;
}

/**********************************************************************************//**
 * \brief In case of on/off button this sets the button in pushed state or not
 * \param on is pushed down
 *************************************************************************************/
void Button::setSelected(bool on) {
	if(!isOnOffButton) return;
	if(on != pressed) {
		if(pressed)
			bevel_size -= 1;
		else
			bevel_size += 1;
		pressed = on;
		fireActionEvent(ACTION_REQUEST_REPAINT);
		if(onClick != NULL)
			onClick(this);
	}
}

/**********************************************************************************//**
 * \brief In case of on/off button this gets the button in pushed state or not
 * \returns true if the button is pushed down
 *************************************************************************************/
bool Button::isSelected() const {
	return pressed;
}

//! \brief Get the descriptive button text 
std::string Button::getText() const {
	return text;
}

/**********************************************************************************//**
 * \brief Sets the function which is to be called in the case of the button being clicked
 * \param onClick pointer to the function which is to be executed
 * 
 * This is the primary interaction point for the Fenris program. It is possible to distinguish
 * between different buttons by testing on the caller (which is passed as the Button* argument)
 * if one where to use the same function for different buttons.
 *************************************************************************************/
void Button::setOnClick(void (*onClick)(Button*)) {
	this->onClick = onClick;
}
