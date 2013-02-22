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
#include "LRSpline/Element.h"
#include "LRVolDisplay.h"
#include "VolumeDisplay.h"
#include "SurfaceDisplay.h"
#include "CurveDisplay.h"
#include "PointDisplay.h"
#include "Button.h"
// #include "Color.h"
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


class LRguy : public LRVolDisplay {

public:
	LRguy(LRSplineVolume *volume) : LRVolDisplay(volume) {
	}
	void tesselate(int *n=NULL) {
		LRVolDisplay::tesselate(n);
		cout << "LRguy tesselate" << std::endl;
	}

	void setColor(vector<double> norm, double *limits=NULL) {
		double cmax = DBL_MIN;
		double cmin = DBL_MAX;
		if(limits != NULL) {
			cmin = limits[0];
			cmax = limits[1];
		} else {
			for(double d : norm) {
				cmax = max(cmax, d);
				cmin = min(cmin, d);
			}
		}
		int i=0;
		int iel = -1;
		for(Element *el : volume->getAllElements()) {
			iel++;
			if(el->getParmin(0) != volume->startparam(0) && 
			   el->getParmin(1) != volume->startparam(1) && 
			   el->getParmin(2) != volume->startparam(2) && 
			   el->getParmax(0) != volume->endparam(0)  && 
			   el->getParmax(1) != volume->endparam(1)  && 
			   el->getParmax(2) != volume->endparam(2) )
				continue;
			HSVType hue;
 			// max = 0 hue (red), min = 5.0 hue (blue
			hue.H = 4.0 - (norm[iel] - cmin) / (cmax-cmin) * 4.0;
			hue.S = 1.0;
			hue.V = 1.0;
			RGBType col = HSV_to_RGB(hue);
			bezierVols[i++]->setColor(col.R, col.G, col.B);
		}
	}

	void displace(vector<double> &result, double scale) {

		int p1 = volume->order(0);
		int p2 = volume->order(1);
		int p3 = volume->order(2);
		vector<double> knot1(2*p1);
		vector<double> knot2(2*p2);
		vector<double> knot3(2*p3);
		for(int i=0; i<p1; i++) knot1[ i  ] = 0.0;
		for(int i=0; i<p1; i++) knot1[p1+i] = 1.0;
		for(int i=0; i<p2; i++) knot2[ i  ] = 0.0;
		for(int i=0; i<p2; i++) knot2[p2+i] = 1.0;
		for(int i=0; i<p3; i++) knot3[ i  ] = 0.0;
		for(int i=0; i<p3; i++) knot3[p3+i] = 1.0;

		int iel = -1;
		int i=0;
		for(Element *el : volume->getAllElements()) {
			iel++;
			if(el->getParmin(0) != volume->startparam(0) && 
			   el->getParmin(1) != volume->startparam(1) && 
			   el->getParmin(2) != volume->startparam(2) && 
			   el->getParmax(0) != volume->endparam(0)  && 
			   el->getParmax(1) != volume->endparam(1)  && 
			   el->getParmax(2) != volume->endparam(2) )
				continue;
			vector<double> C;
			vector<double> coefs(3*p1*p2*p3,0);
			vector<int> ind;
			int width  = p1*p2*p3;
			int height = el->nBasisFunctions();
			for(Basisfunction *b : el->support())
				ind.push_back(b->getId());

			volume->getBezierExtraction(iel, C);
			int k=0;
			for(int col=0; col<width; col++) 
				for(int row=0; row<height; row++,k++) 
					for(int d=0; d<3; d++) 
						coefs[3*col+d] += C[k] * result[3*ind[row]+d] * scale;

			
			Go::SplineVolume *oneDisp = new Go::SplineVolume(p1, p2, p3,
		                                        	         p1, p2, p3,
		                                        	         knot1.begin(), knot2.begin(), knot3.begin(),
		                                        	         coefs.begin(), 3, false);
			bezierVols[i++]->addDisplacement(oneDisp);
		}
	}

	void colorPrimRes(vector<double> &result, double *limits=NULL) {
		double cmax = DBL_MIN;
		double cmin = DBL_MAX;
		if(limits != NULL) {
			cmin = limits[0];
			cmax = limits[1];
		} else {
			for(double d : result) {
				cmax = max(cmax, d);
				cmin = min(cmin, d);
			}
		}

		int p1 = volume->order(0);
		int p2 = volume->order(1);
		int p3 = volume->order(2);
		vector<double> knot1(2*p1);
		vector<double> knot2(2*p2);
		vector<double> knot3(2*p3);
		for(int i=0; i<p1; i++) knot1[ i  ] = 0.0;
		for(int i=0; i<p1; i++) knot1[p1+i] = 1.0;
		for(int i=0; i<p2; i++) knot2[ i  ] = 0.0;
		for(int i=0; i<p2; i++) knot2[p2+i] = 1.0;
		for(int i=0; i<p3; i++) knot3[ i  ] = 0.0;
		for(int i=0; i<p3; i++) knot3[p3+i] = 1.0;

		int iel = -1;
		int i=0;
		for(Element *el : volume->getAllElements()) {
			iel++;
			if(el->getParmin(0) != volume->startparam(0) && 
			   el->getParmin(1) != volume->startparam(1) && 
			   el->getParmin(2) != volume->startparam(2) && 
			   el->getParmax(0) != volume->endparam(0)  && 
			   el->getParmax(1) != volume->endparam(1)  && 
			   el->getParmax(2) != volume->endparam(2) )
				continue;
			vector<double> C;
			vector<double> coefs(p1*p2*p3,0);
			vector<int> ind;
			int width  = p1*p2*p3;
			int height = el->nBasisFunctions();
			for(Basisfunction *b : el->support())
				ind.push_back(b->getId());

			volume->getBezierExtraction(iel, C);
			int k=0;
			for(int col=0; col<width; col++) 
				for(int row=0; row<height; row++) 
					coefs[col] += C[k++] * result[ind[row]];

			
			Go::SplineVolume *oneCol = new Go::SplineVolume(p1, p2, p3,
		                                        	        p1, p2, p3,
		                                        	        knot1.begin(), knot2.begin(), knot3.begin(),
		                                        	        coefs.begin(), 1, false);
			bezierVols[i++]->addColor(oneCol, cmin, cmax);
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
vector<vector<double> >   primSol;
vector<vector<double> >   displace;

void readNorm(const char *filename) {
	ifstream inFile;
	inFile.open(filename);
	if(!inFile.good()) {
		cerr << "Error reading input file \"" << filename << "\"" << endl;
		exit(1);
	}

	double cmax = DBL_MIN;
	double cmin = DBL_MAX;
	for(int i=0; i<geometry.size(); i++) {
		ws(inFile);
		int nEl = geometry[i]->volume->nElements();
		norms.push_back(vector<double>(nEl));
		for(int j=0; j<nEl; j++)  {
			inFile >> norms[i][j];
			cmax = max(cmax, norms[i][j]);
			cmin = min(cmin, norms[i][j]);
		}
	}
	double limits[] = {cmin,cmax};
	for(int i=0; i<geometry.size(); i++)
		geometry[i]->setColor(norms[i]);

	inFile.close();
}

void readDispl(const char *filename, double scale) {
	ifstream inFile;
	inFile.open(filename);
	if(!inFile.good()) {
		cerr << "Error reading input file \"" << filename << "\"" << endl;
		exit(1);
	}

	double cmax = DBL_MIN;
	double cmin = DBL_MAX;
	for(int i=0; i<geometry.size(); i++) {
		ws(inFile);
		int nBasis = geometry[i]->volume->nBasisFunctions();
		displace.push_back(vector<double>(3*nBasis));
		for(int j=0; j<3*nBasis; j++) 
			inFile >> displace[i][j];
		geometry[i]->displace(displace[i], scale);
	}

	inFile.close();
}

void readPrimCol(const char *filename) {
	ifstream inFile;
	inFile.open(filename);
	if(!inFile.good()) {
		cerr << "Error reading input file \"" << filename << "\"" << endl;
		exit(1);
	}

	double cmax = DBL_MIN;
	double cmin = DBL_MAX;
	for(int i=0; i<geometry.size(); i++) {
		ws(inFile);
		int nBasis = geometry[i]->volume->nBasisFunctions();
		primSol.push_back(vector<double>(nBasis));
		for(int j=0; j<nBasis; j++)  {
			inFile >> primSol[i][j];
			primSol[i][j] = fabs(primSol[i][j]);
			cmax = max(cmax, primSol[i][j]);
			cmin = min(cmin, primSol[i][j]);
		}
	}
	double limits[] = {cmin,cmax};
	for(int i=0; i<geometry.size(); i++)
		geometry[i]->colorPrimRes(primSol[i]);

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
			obj->setColorByParameterValues(true);
			geometry.push_back( obj );
			cout << "LRSpline volume succesfully read" << endl;
		} else { 
			cerr << "Error: Corrupt file \"" << filename << "\", only LR volumes permitted" << endl;
			exit(3);
		}
	}
	objects->addObject( geometry[0] );
	objects->setSelected( geometry[0] );
	inFile.close();
	
	SplineGUI *gui = SplineGUI::getInstance();

	gui->updateBoundingBox();

}

void processParameters(int argc, char** argv) {

	double scale = atof(argv[5]);
	readFile(argv[1]);
	readNorm(argv[2]);
	readPrimCol(argv[3]);
	readDispl(argv[4], scale);

	int res[] = {4,4,4};
	for(LRguy *v : geometry)
		v->tesselate(res);

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
	objects->setSelected(geometry[newIter]);
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

