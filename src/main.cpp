/**********************************************************************************//**
 * \file main.cpp
 *
 * \author Kjetil A. Johannessen
 * \date July 2010
 *
 * \brief Program for displaying GoTools native file(s) through the simplest use of SplineGUI
 *************************************************************************************/
 
#include "SplineGUI.h"
#include "Button.h"
#include <iostream>


using namespace std;

int main(int argc, char** argv) {

	SplineGUI *gui = SplineGUI::getInstance();
	for(int argi=1; argi<argc; argi++)
		gui->addFile(argv[argi]);
	gui->show();

	return 0;
}

