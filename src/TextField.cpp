//==============================================================================
//!
//! \file TextField.cpp
//!
//! \date April 2012
//!
//! \author Kjetil A. Johannessen / SINTEF
//!
//! \brief TextField primitive for GUI control
//!
//! 
//==============================================================================

#include "TextField.h"

TextField::TextField() {
	x = 0;
	y = 0;
	width  = 0;
	height = 0;
	bevel_size = 3; // given in pixels
	caret_position = text.length();
	// set white background color
	r = 0.9;
	g = 0.9;
	b = 0.9;
	// set black text color
	tr = 0;
	tg = 0;
	tb = 0;
}

TextField::TextField(std::string text) {
	this->text = text;
	x = 0;
	y = 0;
	width  = 0;
	height = 0;
	bevel_size = 3; // given in pixels
	caret_position = text.length();
	// set white background color
	r = 0.9;
	g = 0.9;
	b = 0.9;
	// set black text color
	tr = 0;
	tg = 0;
	tb = 0;
}

TextField::TextField(std::string text, int x, int y, int width, int height) : MouseListener(x,y,width,height) {
	this->text = text;
	this->x = x;
	this->y = y;
	this->width  = width;
	this->height = height;
	bevel_size = 3; // given in pixels
	caret_position = text.length();
	// set white background color
	r = 0.9;
	g = 0.9;
	b = 0.9;
	// set black text color
	tr = 0;
	tg = 0;
	tb = 0;
}

TextField::TextField(int x, int y, int width, int height) : MouseListener(x,y,width,height) {
	this->x = x;
	this->y = y;
	this->width  = width;
	this->height = height;
	bevel_size = 3; // given in pixels
	caret_position = 0;
	// set white background color
	r = 0.9;
	g = 0.9;
	b = 0.9;
	// set black text color
	tr = 0;
	tg = 0;
	tb = 0;
}

void TextField::paint() {
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
		// main textfield
		glColor3f(r,g,b);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(x1, y2);

		// top bevel
		glColor3f(r-.4, g-.4, b-.4);
		glVertex2f(x1, y1);
		glVertex2f(x2, y1);
		glVertex2f(bev_x2, bev_y1);
		glVertex2f(bev_x1, bev_y1);

		// left bevel
		glColor3f(r-.3, g-.3, b-.3);
		glVertex2f(x1, y1);
		glVertex2f(bev_x1, bev_y1);
		glVertex2f(bev_x1, bev_y2);
		glVertex2f(x1, y2);

		// right bevel
		glColor3f(r+.2, g+.2, b+.2);
		glVertex2f(bev_x2, bev_y1);
		glVertex2f(x2, y1);
		glVertex2f(x2, y2);
		glVertex2f(bev_x2, bev_y2);

		// bottom bevel
		glColor3f(r+.3, g+.3, b+.3);
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
	glutPrint((float) width/2-text.length()/2.0*6+1, height/2-5, GLUT_BITMAP_HELVETICA_12, text, tr, tg, tb, 1);
}
void TextField::processMouse(int button, int state, int x, int y) {
}
void TextField::processMouseActiveMotion(int x, int y) {
}
void TextField::processMousePassiveMotion(int x, int y) {
}
void TextField::onEnter() {
}
void TextField::onExit() {
}

void TextField::handleKeypress(unsigned char key, int x, int y) {
	if(0x20 <= key && key <= 0x7E) { // printable character range
		text.insert(text.begin()+caret_position, key);
		caret_position++;
	} else if (key == 0x8) { // backspace charater
		if(caret_position > 0) {
			text.erase(text.begin()+caret_position-1, text.end());
			caret_position--;
		}
	} else if (key == 0xD) { // enter character
		fireActionEvent(ACTION_TEXTFIELD_ENTERED);
	}
}

void TextField::setBackgroundColor(GLfloat r, GLfloat g, GLfloat b) {
	this-> r = r;
	this-> g = g;
	this-> b = b;
}
void TextField::setTextColor(GLfloat r, GLfloat g, GLfloat b) {
	tr = r;
	tg = g;
	tb = b;
}

std::string TextField::getText() const {
	return text;
}
void TextField::setText(std::string text) {
	this->text = text;
	caret_position = text.length();
}
void TextField::appendText(std::string text) {
	this->text.append(text);
}
void TextField::appendText(char oneCharacter) {
	text.append(1, oneCharacter);
}

void TextField::setSizeAndPos(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width  = width;
	this->height = height;
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
void TextField::glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a)
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
