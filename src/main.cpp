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

	// could actually type gui->show() here and be done with it
	// but we'll add some usability by adding parametric coloring
	gui->getObjectSet()->colorSelectedByParameterValues(true);
	
	gui->show();

        delete gui;

	return 0;
}

