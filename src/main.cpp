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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef std::vector<DisplayObject*>::iterator objIterator;

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
	bool hideline  = false;
	bool dotexture = false;

	SplineGUI *gui = SplineGUI::getInstance();
	for(int argi=1; argi<argc; argi++) {
		if(argv[argi][0]=='-') {
			if(!strcmp(argv[argi]+1, "help")) {
				cout << argv[0] << " [-flags] inputFile(s)" << endl;
				cout << "  Flags:" << endl;
				cout << "    -help:     displays this help screen"   << endl;
				cout << "    -noline:   hide all surface meshlines" << endl;
				cout << "    -texture:  provide surface texture" << endl;
				cout << "    -res <n>:  tesselation resolution of n" << endl;
				exit(0);
			} else if(!strcmp(argv[argi]+1, "noline")) {
				hideline = true;
			} else if(!strcmp(argv[argi]+1, "texture")) {
				int width, height, nrChannels;
				unsigned char *image = stbi_load(argv[++argi], &width, &height, &nrChannels, 0);
				cout << "Texture information: " << endl;
				cout << "  Color channels: " << nrChannels << endl;
				cout << "  Width         : " << width << endl;
				cout << "  Height        : " << height << endl;
				if( image == NULL ) {
					cerr << "Could not open or find the image " << argv[argi-1] << endl;
					return -1;
				}
				unsigned int texPtr;
				glGenTextures(1, &texPtr);
				cout << "Tex ptr: " << texPtr << endl;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texPtr);
				if(nrChannels == 4) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
				} else if(nrChannels == 3) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				} else if(nrChannels == 2) {
					cerr << "Unsupported image format " << argv[argi-1] << endl;
					return -1;
				} else if(nrChannels == 1) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
				}
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				dotexture = true;
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
	if(dotexture)
		for(objIterator obj=objSet->objects_begin(); obj != objSet->objects_end(); obj++)
			if((*obj)->classType() == SURFACE) {
				cout << "Requesting textured surface\n";
				((SurfaceDisplay*) *obj)->setTextured(true);
			}

	gui->show();

	return 0;
}

