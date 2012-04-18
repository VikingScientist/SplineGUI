
#ifndef _TEXTFIELD_H
#define _TEXTFIELD_H

#include "MouseListener.h"
#include "KeyListener.h"
#include "ActiveObject.h"
#include <string>
#include <GL/glut.h>

class TextField : public MouseListener, KeyListener, ActiveObject {

	public:
		TextField();
		TextField(std::string text);
		TextField(std::string text, int x, int y, int width, int height);
		TextField(int x, int y, int width, int height);

		void paint();
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void onEnter() ;
		void onExit() ;

		void handleKeypress(unsigned char key, int x, int y);

		void setBackgroundColor(GLfloat r, GLfloat g, GLfloat b);
		void setTextColor(GLfloat r, GLfloat g, GLfloat b);

		std::string getText() const;
		void setText(std::string text);
		void appendText(std::string text);
		void appendText(char oneCharacter);

		void setSizeAndPos(int x, int y, int width, int height) ;

	private:
		void glutPrint(float x, float y, void* font, std::string text, float r, float g, float b, float a);

		std::string text;
		int x,y;
		int width, height;
		GLfloat r,g,b;    // background color
		GLfloat tr,tg,tb; // text color
		int bevel_size;
		int caret_position;

};

#endif

