#include "Fenris.h"
#include <iostream>


using namespace std;

int main(int argc, char** argv) {

	Fenris *f = Fenris::getInstance();
	for(int argi=1; argi<argc; argi++)
		f->addFile(argv[argi]);
	f->show();

	return 0;
}
