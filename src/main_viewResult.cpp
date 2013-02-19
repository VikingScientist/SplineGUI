/**********************************************************************************//**
 * \file main_viewResult.cpp
 *
 * \author Kjetil A. Johannessen
 * \date February 2012
 *
 * \brief View 3D adaptive LR spline results (displacements and stresses)
 *************************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>
#include <float.h>
#include <algorithm>

#include "DisplayObjectSet.h"
#include "SplineGUI.h"
#include "LRVolDisplay.h"
#include "VolumeDisplay.h"
#include "SurfaceDisplay.h"
#include "CurveDisplay.h"
#include "PointDisplay.h"
#include "Button.h"
#include "TextField.h"

#include <GoTools/trivariate/SplineVolume.h>
#include <GoTools/geometry/SplineSurface.h>
#include <GoTools/geometry/SplineCurve.h>
#include <GoTools/utils/Point.h>

typedef unsigned int uint;

using namespace std;
using namespace LR;
// using std::shared_ptr;
// using namespace Go;

// copy-paste some color-manipulation tools from a random webpage
#define RETURN_HSV(h, s, v) {HSV.H = h; HSV.S = s; HSV.V = v; return HSV;}
#define RETURN_RGB(r, g, b) {RGB.R = r; RGB.G = g; RGB.B = b; return RGB;}
#define UNDEFINED -1 
typedef struct {float R, G, B;} RGBType;
typedef struct {float H, S, V;} HSVType; 
HSVType RGB_to_HSV( RGBType RGB ) {
	// RGB are each on [0, 1]. S and V are returned on [0, 1] and H is
	// returned on [0, 6]. Exception: H is returned UNDEFINED if S==0.
	float R = RGB.R, G = RGB.G, B = RGB.B, v, x, f;
	int i;
	HSVType HSV;
	
	x = min(min(R,G),B);
	v = max(max(R,G),B);
	if(v == x) RETURN_HSV(UNDEFINED, 0, v);
	f = (R == x) ? G - B : ((G == x) ? B - R : R - G);
	i = (R == x) ? 3 : ((G == x) ? 5 : 1);
	RETURN_HSV(i - f /(v - x), (v - x)/v, v);
}
RGBType HSV_to_RGB( HSVType HSV ) {
	// H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
	// RGB are each returned on [0, 1].
	float h = HSV.H, s = HSV.S, v = HSV.V, m, n, f;
	int i;
	RGBType RGB;
	
	if (h == UNDEFINED) RETURN_RGB(v, v, v);
	i = floor(h);
	f = h - i;
	if ( !(i&1) ) f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: RETURN_RGB(v, n, m);
		case 1: RETURN_RGB(n, v, m);
		case 2: RETURN_RGB(m, v, n)
		case 3: RETURN_RGB(m, n, v);
		case 4: RETURN_RGB(n, m, v);
		case 5: RETURN_RGB(v, m, n);
	}
	RETURN_RGB(0,0,0); // should never reach here, but compiler complains
} 

class LRguy : public LRVolDisplay {

public:
	LRguy(LRSplineVolume *volume) : LRVolDisplay(volume) {
	}
	void tesselate(int *n=NULL) {
		LRVolDisplay::tesselate(n);
		cout << "LRguy tesselate" << std::endl;
	}
	void setColor(vector<double> norm) {
		double cmax = DBL_MIN;
		double cmin = DBL_MAX;
		for(double d : norm) {
			cmax = max(cmax, d);
			cmin = min(cmin, d);
		}
		int i=0;
		for(VolumeDisplay *v : bezierVols) {
			HSVType hue;
 			// max = 0 hue (red), min = 5.5 hue (blue
			hue.H = 5.5 - (norm[i++] - cmin) / (cmax-cmin) * 5.5;
			hue.S = 1.0;
			hue.V = 1.0;
			RGBType col = HSV_to_RGB(hue);
			v->setColor(col.R, col.G, col.B);
		}
	}
	
};

TextField*            textInput;
vector<const char*>   boundaryTags;

Button *showVolumes   = new Button("Volumes");
Button *showFaces     = new Button("Faces");
Button *showLines     = new Button("Lines");
Button *showPoints    = new Button("Vertices");

int iteration = 0;
DisplayObjectSet* objects; 
vector<LRguy*>    geometry;
vector<vector<double> >   norms;
vector<vector<double> >   displace;

void readNorm(const char *filename) {
	ifstream inFile;
	inFile.open(filename);
	if(!inFile.good()) {
		cerr << "Error reading input file \"" << filename << "\"" << endl;
		exit(1);
	}

	for(int i=0; i<geometry.size(); i++) {
		ws(inFile);
		int nEl = geometry[i]->volume->nElements();
		norms.push_back(vector<double>(nEl));
		for(int j=0; j<nEl; j++) 
			inFile >> norms[i][j];
		geometry[i]->setColor(norms[i]);
	}

	inFile.close();
}

void readFile(const char *filename) {
	ifstream inFile;
	inFile.open(filename);
	if(!inFile.good()) {
		cerr << "Error reading input file \"" << filename << "\"" << endl;
		exit(1);
	}
	int patchCount = 0;

	// read (multipatch) GoTools object
	while(!inFile.eof()) {
		char buffer[512];
		ws(inFile);
		int pos = inFile.tellg();
		inFile.getline(buffer, 512); // peek the first line to figure out if it's an LRSpline or a GoTools spline
		inFile.seekg(pos);

		char *patchName = new char[256];
		sprintf(patchName, "Iteration %d", patchCount++);

		if(strncmp(buffer, "# LRSPLINE VOLUME",17)==0) {
			LR::LRSplineVolume *v = new LR::LRSplineVolume();
			v->read(inFile);
			LRguy *obj = new LRguy(v);
			obj->setMeta(patchName);
			geometry.push_back( obj );
			cout << "LRSpline volume succesfully read" << endl;
		} else { 
			cerr << "Error: Corrupt file \"" << filename << "\", only LR volumes permitted" << endl;
			exit(3);
		}
	}
	objects->addObject( geometry[0] );
	inFile.close();
	
	SplineGUI *gui = SplineGUI::getInstance();

	gui->updateBoundingBox();

}

void processParameters(int argc, char** argv) {

	readFile(argv[1]);
	readNorm(argv[2]);

}

void keyClick(unsigned char key) {
	SplineGUI *gui = SplineGUI::getInstance();
	if(key < '0' || key > '9')
		return;
	unsigned int newIter = key - '0';
	if( newIter >= geometry.size())
		return;

	objects->removeObject(geometry[iteration]);
	objects->addObject(geometry[newIter]);
	iteration = newIter;
}

void ButtonClick(Button *caller) {
	SplineGUI *gui = SplineGUI::getInstance();
	/*
	if(caller == showVolumes) {
		if(showVolumes->isSelected()) {
			gui->unHideObjects(VOLUME);
			showFaces->setSelected(false);
		} else {
			gui->hideObjects(VOLUME);
		}
	} else if(caller == showFaces) {
		if(showFaces->isSelected()) {
			gui->unHideObjects(SURFACE);
			showVolumes->setSelected(false);
		} else {
			gui->hideObjects(SURFACE);
		}
	} else if(caller == showLines) {
		if(showLines->isSelected()) {
			gui->unHideObjects(CURVE);
		} else {
			gui->hideObjects(CURVE);
		}
	} else if(caller == showPoints) {
		if(showPoints->isSelected()) {
			gui->unHideObjects(POINT);
		} else {
			gui->hideObjects(POINT);
		}
	}
	*/
}

void colorPrimitive(int patch, DISPLAY_CLASS_TYPE type, int locIndex, int iCode, const char* code) {
	SplineGUI *gui = SplineGUI::getInstance();
	/*
	HSVType col_hsv;
	col_hsv.H = 6.*((3*iCode)%10)/9;
	col_hsv.V = 0.7;
	col_hsv.S = (iCode==0) ? 0.0 : 0.7;
	RGBType color = HSV_to_RGB(col_hsv);

	DisplayObject *obj = NULL;
	if(type == POINT)
		obj = gui->getDisplayObject(points[patch][locIndex].get());
	else if(type == CURVE)
		obj = gui->getDisplayObject(curves[patch][locIndex].get());
	else if(type == SURFACE && model.isVolumetricModel())
		obj = gui->getDisplayObject(surfaces[patch][locIndex].get());
	else if(type == SURFACE && !model.isVolumetricModel())
		obj = gui->getDisplayObject(surfPatches[patch].get());
	else if(type == VOLUME)
		obj = gui->getDisplayObject(volumes[patch].get());

	if(obj == NULL) {
		cerr << "Error parsing input stream: primitive not found\n" ;
	} else {
		obj->setColor(color.R,color.G,color.B);
		obj->setMeta(code);
	}
	*/
}

int main(int argc, char **argv) {
	SplineGUI *gui = SplineGUI::getInstance();
	objects = gui->getObjectSet();

	processParameters(argc, argv);

	/*
	DisplayObjectSet *objSet = gui->getObjectSet();
	objSet->enableControlMesh(false);
	objSet->setLineWidth(5);
	objSet->setPointSize(14);
	gui->hideObjects(VOLUME);
	if(isVolumeModel)
		gui->hideObjects(CURVE);
	else
		gui->hideObjects(SURFACE);
	gui->hideObjects(POINT);
	*/

	showVolumes->makeOnOffButton();
	showFaces->makeOnOffButton();
	showLines->makeOnOffButton();
	showPoints->makeOnOffButton();

	showVolumes->setSelected(false);
	showFaces->setSelected(true);
	showLines->setSelected(false);
	showPoints->setSelected(false);

	showVolumes->setOnClick(ButtonClick);
	showFaces->setOnClick(ButtonClick);
	showLines->setOnClick(ButtonClick);
	showPoints->setOnClick(ButtonClick);

	gui->addButton(showFaces);
	gui->addButton(showLines);
	gui->addButton(showPoints);

	gui->addKeyboardListener(keyClick);

	gui->setSplineColor(.7, .7, .7);

	gui->show();

	return 0;
}

