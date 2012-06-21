
#ifndef _MOUSE_LISTENER_H
#define _MOUSE_LISTENER_H

class MouseListener {
	protected:
		bool hover;
		bool catchAll;
		int  x, y;
		int  width, height;
		int  last_mouse_x;
		int  last_mouse_y;
		int  specialKey;
		bool right_mouse_button_down;
		bool left_mouse_button_down;

	public:
		MouseListener() ;
		MouseListener(int x, int y, int width, int height) ;
		virtual void processMouse(int button, int state, int x, int y) ;
		virtual void processMouseActiveMotion(int x, int y) ;
		virtual void processMousePassiveMotion(int x, int y) ;
		virtual void onEnter(int x, int y) { hover = true;}
		virtual void onExit(int x, int y)  { hover = false;}
		void setSize(int x, int y, int width, int height) ;
		bool isHover()        { return hover; }
		bool isCatchingAll()  { return catchAll; }
		bool isContained(int px, int py)  { return (x<px && px<x+width && y<py && py<y+height); }
		int getX()       { return x; }
		int getY()       { return y; }
		int getWidth()   { return width; }
		int getHeight()  { return height; }
};

#endif
