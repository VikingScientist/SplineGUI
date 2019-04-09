/**********************************************************************************//**
 * \file main.cpp
 *
 * \author Kjetil A. Johannessen
 * \date July 2010
 *
 * \brief Program for displaying GoTools native file(s) through the simplest use of SplineGUI
 *************************************************************************************/

#include "SplineGUI.h"
#include "DisplayObjectSet.h"
#include "Button.h"
#include <string.h>
#include <iostream>
#include <vector>

typedef std::vector<DisplayObject*>::iterator objIterator;
SplineGUI *gui;

bool isInt(char* str) {
	while(*str != 0) {
		if(*str < '0' || *str > '9')
			return false;
		str++;
	}
	return true;
}

void keyClick(unsigned char key) {
	if(key == 13) { // enter key
		std::vector<DisplayObject*> selected = gui->getSelectedObjects();
		for(DisplayObject* obj : selected)
			gui->hideObjects(obj);
	} else if(key == 27) { // esc key
		gui->unHideObjects(VOLUME);
		gui->unHideObjects(SURFACE);
	}
}



using namespace std;

int main(int argc, char** argv) {
	vector<int> customTesselation;
	bool hideline = false;

	gui = SplineGUI::getInstance();
	for(int argi=1; argi<argc; argi++) {
		if(argv[argi][0]=='-') {
			if(!strcmp(argv[argi]+1, "help")) {
				cout << argv[0] << " [-flags] inputFile(s)" << endl;
				cout << "  Flags:" << endl;
				cout << "    -help:     displays this help screen"   << endl;
				cout << "    -noline:   hide all surface meshlines" << endl;
				cout << "    -res <n>:  tesselation resolution of n" << endl;
				exit(0);
			} else if(!strcmp(argv[argi]+1, "noline")) {
				hideline = true;
			} else if(!strcmp(argv[argi]+1, "res")) {
				if( argi+1<argc &&  isInt(argv[argi+1] ))
					customTesselation.push_back(atoi(argv[++argi]));
				if( argi+1<argc && isInt(argv[argi+1] ))
					customTesselation.push_back(atoi(argv[++argi]));
				if( argi+1<argc && isInt(argv[argi+1] ))
					customTesselation.push_back(atoi(argv[++argi]));
			}
		} else {
			gui->addFile(argv[argi]);
		}
	}

	size_t objects = std::distance(gui->getObjectSet()->objects_begin(),
	                               gui->getObjectSet()->objects_end());
	std::cout << "Read " << objects << (objects>1?" objects":" object") << std::endl;

	// could actually type gui->show() here and be done with it
	// but we'll add some usability by adding parametric coloring
	DisplayObjectSet* objSet = gui->getObjectSet();
	objSet->colorSelectedByParameterValues(true);

	if(customTesselation.size() > 0) {
		while(customTesselation.size()<3)
			customTesselation.push_back(customTesselation.back());

		objSet->tesselateAll(&(customTesselation[0]));
	}
	if(hideline)
		for(objIterator obj=objSet->objects_begin(); obj != objSet->objects_end(); obj++)
			if((*obj)->classType() == SURFACE)
				((SurfaceDisplay*) *obj)->setDrawMeshlines(false);

    // listen for 'enter' inputs to hide selected volumes
	gui->addKeyboardListener(keyClick);

	gui->show();

	return 0;
}

