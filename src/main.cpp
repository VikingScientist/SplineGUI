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

bool isInt(char* str) {
	while(*str != 0) {
		if(*str < '0' || *str > '9')
			return false;
		str++;
	}
	return true;
}


using namespace std;

int main(int argc, char** argv) {
	vector<int> customTesselation;

	SplineGUI *gui = SplineGUI::getInstance();
	for(int argi=1; argi<argc; argi++) {
		if(argv[argi][0]=='-') {
			if(!strcmp(argv[argi]+1, "help")) {
				cout << argv[0] << " [-flags] inputFile(s)" << endl;
				cout << "  Flags:" << endl;
				cout << "    -help:     displays this help screen"   << endl;
				cout << "    -res <n>:  tesselation resolution of n" << endl;
				exit(0);
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
	gui->getObjectSet()->colorSelectedByParameterValues(true);
	
	if(customTesselation.size() > 0) {
		while(customTesselation.size()<3)
			customTesselation.push_back(customTesselation.back());
		
		gui->getObjectSet()->tesselateAll(&(customTesselation[0]));
	}
	gui->show();

	return 0;
}

