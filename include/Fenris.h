#ifndef _FENRIS_H
#define _FENRIS_H

// GoTools headers
#include <GoTools/trivariate/SplineVolume.h>
#include <GoTools/geometry/SplineSurface.h>
#include <GoTools/geometry/SplineCurve.h>

#include <vector>

class MouseListener;
class ActiveObject;
class Button;

/**
 * \todo Is it possible to remove the use of the Workaround_namespace and replace this by
 * static class-functions? Possible also private.
 */

namespace Workaround_namespace {

	void addMouseListener(MouseListener *ml);
	void actionListener(ActiveObject *caller, int event);
	void updateMouseMasks();
	void drawScene();
	void handleResize(int w, int h);
	void readFile(const char *filename);
	void handleKeypress(unsigned char key, int x, int y);
	void processMouse(int button, int state, int x, int y);
	void processMouseActiveMotion(int x, int y);
	void processMousePassiveMotion(int x, int y);
	void initRendering();
}

/** 
 * \todo Implement a function to allow for command-line input (ideally we would like pop-up
 * dialogs, but GLUT doesn't really make this easy).
 */

class Fenris {
	private:
		Fenris();
		static Fenris *instance_;

		int next_button_x;
		int next_button_y;

	public:

		// data management
		void addFile(const char *filename);
		void addObject(Go::SplineCurve *c);
		void addObject(Go::SplineSurface *s);
		void addObject(Go::SplineVolume *v);

		// interaction management
		void addButton(Button *b);

		// window handling
		void setSize(int width, int height);
		void show();

		/***************************************************//**
		 \brief get Fenris GUI instance

		 Gets the Fenris singelton object. This is the only way of
		 getting this object as the constructor has been made private.
		 The window will not appear, until the show() function has been
		 called. This will halt any program execution until the GUI
		 window has been closed, so make sure to read all input and set
		 all parameters prior to showing the GUI. After the GUI have been
		 shown, all interaction with the program should be with either
		 setActionListener (for buttons) or MouseListener.
		******************************************************/
		static Fenris* getInstance() {
			if(!instance_)
				instance_ = new Fenris();
			return instance_;
		}
};

// initalizing functions
void create_Fenris_GUI();
void create_Fenris_GUI(Go::SplineVolume *v);
void create_Fenris_GUI(const char *filename);


std::vector<Go::SplineVolume*>  getSelectedVolumes();
std::vector<Go::SplineSurface*> getSelectedSurfaces();

#endif
