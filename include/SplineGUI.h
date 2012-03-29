#ifndef _SPLINEGUI_H
#define _SPLINEGUI_H

// GoTools headers
#include <GoTools/trivariate/SplineVolume.h>
#include <GoTools/geometry/SplineSurface.h>
#include <GoTools/geometry/SplineCurve.h>
#include <GoTools/geometry/GoTools.h>

#include "DisplayObject.h"
#include "PointDisplay.h"
#include "CurveDisplay.h"
#include "SurfaceDisplay.h"
#include "VolumeDisplay.h"

#include <vector>

#if defined(GO_VERSION_MAJOR) && GO_VERSION_MAJOR >= 3
typedef std::shared_ptr<Go::Point>         PointPointer;
typedef std::shared_ptr<Go::SplineCurve>   CurvePointer;
typedef std::shared_ptr<Go::SplineSurface> SurfacePointer;
typedef std::shared_ptr<Go::SplineVolume>  VolumePointer; 
#else
typedef boost::shared_ptr<Go::Point>         PointPointer;
typedef boost::shared_ptr<Go::SplineCurve>   CurvePointer;
typedef boost::shared_ptr<Go::SplineSurface> SurfacePointer;
typedef boost::shared_ptr<Go::SplineVolume>  VolumePointer; 
#endif

class DisplayObjectSet;
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
	void hoverTest();
}

/** 
 * \todo Implement a function to allow for command-line input (ideally we would like pop-up
 * dialogs, but GLUT doesn't really make this easy).
 */

typedef void (*keyListener)(unsigned char);

class SplineGUI {
	private:
		SplineGUI();
		static SplineGUI *instance_;

		int next_button_x;
		int next_button_y;
		
	public:
		// data management
		DisplayObjectSet *getObjectSet();
		void addFile(const char *filename);
		void addObject(Go::Point *p);
		void addObject(Go::SplineCurve *c, bool clean=false);
		void addObject(Go::SplineSurface *s, bool clean=false);
		void addObject(Go::SplineVolume *v);
		PointDisplay* getDisplayObject(Go::Point *p);
		CurveDisplay* getDisplayObject(Go::SplineCurve *c);
		SurfaceDisplay* getDisplayObject(Go::SplineSurface *s);
		VolumeDisplay* getDisplayObject(Go::SplineVolume *v);
		void hideObjects(DISPLAY_CLASS_TYPE type);
		void unHideObjects(DISPLAY_CLASS_TYPE type);
		std::vector<DisplayObject*> getSelectedObjects();

		// interaction management
		void addButton(Button *b);
		void addKeyboardListener(void (*handleKeyPress)(unsigned char));

		// window handling
		void setSplineColor(float r, float g, float b);
		void setSize(int width, int height);
		void show();

		/***************************************************//**
		 \brief get SplineGUI instance

		 Gets the SplineGUI singelton object. This is the only way of
		 getting this object as the constructor has been made private.
		 The window will not appear, until the show() function has been
		 called. This will halt any program execution until the GUI
		 window has been closed, so make sure to read all input and set
		 all parameters prior to showing the GUI. After the GUI have been
		 shown, all interaction with the program should be with either
		 setActionListener (for buttons) or MouseListener.
		******************************************************/
		static SplineGUI* getInstance() {
			if(!instance_)
				instance_ = new SplineGUI();
			return instance_;
		}

		/*! \brief This shouldn't really be here (in the public section,
		           but workaround-stuff requires it :( */
		std::vector<keyListener> keyListeners_;
};


std::vector<Go::SplineVolume*>  getSelectedVolumes();
std::vector<Go::SplineSurface*> getSelectedSurfaces();

#endif
