/**********************************************************************************//**
 * \file main.cpp
 *
 * \author Kjetil A. Johannessen
 * \date July 2010
 *
 * \brief Program for displaying GoTools native file(s) through the simplest use of Fenris
 *************************************************************************************/
 
#include "Fenris.h"
#include "Button.h"
#include <iostream>


using namespace std;

int main(int argc, char** argv) {

	Fenris *f = Fenris::getInstance();
	for(int argi=1; argi<argc; argi++)
		f->addFile(argv[argi]);
	f->show();

	return 0;
}

