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
#include <iostream>


using namespace std;

int main(int argc, char** argv) {

	SplineGUI *gui = SplineGUI::getInstance();
	for(int argi=1; argi<argc; argi++)
		gui->addFile(argv[argi]);

        size_t objects = std::distance(gui->getObjectSet()->objects_begin(),
                                      gui->getObjectSet()->objects_end());
        std::cout << "Read " << objects << (objects>1?" objects":" object") << std::endl;

	// could actually type gui->show() here and be done with it
	// but we'll add some usability by adding parametric coloring
	gui->getObjectSet()->colorSelectedByParameterValues(true);
	
	gui->show();

	return 0;
}

