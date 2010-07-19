
#ifndef _MOUSE_LISTENER_H
#define _MOUSE_LISTENER_H

class MouseListener {
	private:
		bool hover;
		bool catchAll;
		int x, y;
		int width, height;

	public:
		MouseListener() {
			this->x      = 0;
			this->y      = 0;
			this->width  = 0;
			this->height = 0;
			hover        = false;
			catchAll     = true;
		}
		MouseListener(int x, int y, int width, int height) {
			this->x      = x;
			this->y      = y;
			this->width  = width;
			this->height = height;
			hover        = false;
			catchAll     = false;
		}
		virtual void processMouse(int button, int state, int x, int y) { }
		virtual void processMouseActiveMotion(int x, int y) { }
		virtual void processMousePassiveMotion(int x, int y) { }
		virtual void onEnter() { hover = true;}
		virtual void onExit()  { hover = false;}
		void setSize(int x, int y, int width, int height) {
			this->x      = x;
			this->y      = y;
			this->width  = width;
			this->height = height;
			catchAll     = false;
		}
		bool isHover()        { return hover; }
		bool isCatchingAll()  { return catchAll; }
		bool isContained(int px, int py)  { return (x<px && px<x+width && y<py && py<y+height); }
		int getX()       { return x; }
		int getY()       { return y; }
		int getWidth()   { return width; }
		int getHeight()  { return height; }
};

#endif
