/**********************************************************************************//**
 * \file main_setBndryCodes.cpp
 *
 * \author Kjetil A. Johannessen
 * \date January 2012
 *
 * \brief spline GUI for GPM-interaction
 *************************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>

#include "DisplayObjectSet.h"
#include "SplineGUI.h"
#include "VolumeDisplay.h"
#include "SurfaceDisplay.h"
#include "CurveDisplay.h"
#include "PointDisplay.h"
#include "Button.h"

#include <GPM/SplineModel.h>
#include <GPM/TopologySet.h>
#include <GPM/primitives.h>
#include <GoTools/trivariate/SplineVolume.h>
#include <GoTools/geometry/SplineSurface.h>
#include <GoTools/geometry/SplineCurve.h>
#include <GoTools/utils/Point.h>

typedef unsigned int uint;

using namespace std;
// using std::shared_ptr;
// using namespace Go;

// copy-paste some color-manipulation tools from a random webpage
#define RETURN_HSV(h, s, v) {HSV.H = h; HSV.S = s; HSV.V = v; return HSV;}
#define RETURN_RGB(r, g, b) {RGB.R = r; RGB.G = g; RGB.B = b; return RGB;}
#define min(x,y,z) (x<y) ? ((x<z)?x:z) : ((y<z)?y:z)
#define max(x,y,z) (x>y) ? ((x>z)?x:z) : ((y>z)?y:z)
#define UNDEFINED -1 
typedef struct {float R, G, B;} RGBType;
typedef struct {float H, S, V;} HSVType; 
HSVType RGB_to_HSV( RGBType RGB ) {
	// RGB are each on [0, 1]. S and V are returned on [0, 1] and H is
	// returned on [0, 6]. Exception: H is returned UNDEFINED if S==0.
	float R = RGB.R, G = RGB.G, B = RGB.B, v, x, f;
	int i;
	HSVType HSV;
	
	x = min(R, G, B);
	v = max(R, G, B);
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

SplineModel model;
vector<shared_ptr<Go::SplineVolume> >           volumes;
vector<shared_ptr<Go::SplineSurface> >          surfPatches;
vector<vector<shared_ptr<Go::SplineSurface> > > surfaces;
vector<vector<shared_ptr<Go::SplineCurve> > >   curves;
vector<vector<shared_ptr<Go::Point> > >         points;

Button *showVolumes   = new Button("Volumes");
Button *showFaces     = new Button("Faces");
Button *showLines     = new Button("Lines");
Button *showPoints    = new Button("Vertices");

void processParameters(int argc, char** argv) {
	for(int argi=1; argi<argc; argi++) {
		const char *arg = argv[argi];
		ifstream inFile;
		inFile.open(arg);
		if(!inFile.good()) {
			cerr << "Error reading input file \"" << arg << "\"" << endl;
			exit(1);
		}
		model.readSplines(inFile);
		inFile.close();
	}
}

void keyClick(unsigned char key) {
	if('0' <= key && key <= '9') {
		SplineGUI *gui = SplineGUI::getInstance();
		vector<DisplayObject*> selected = gui->getSelectedObjects();
		int code = key - '0';
		HSVType col_hsv;
		col_hsv.H = 6.*((3*code)%10)/9;
		col_hsv.V = 0.7;
		col_hsv.S = (code==0) ? 0.0 : 0.7;
		RGBType color = HSV_to_RGB(col_hsv);
		for(uint i=0; i<selected.size(); i++) {
			// cout << "setting code #" << code << "(" << color.R << ", " << color.G << ", " << color.B << ")\n";
			selected[i]->setColor(color.R,color.G,color.B);
			if(selected[i]->classType() == VOLUME) {
				Go::SplineVolume *v = ((VolumeDisplay*)selected[i])->volume;
				for(uint j=0; j<volumes.size(); j++)
					if(volumes[j].get() == v) 
						model.addVolumePropertyCode(j, code);

			} else if(selected[i]->classType() == SURFACE) {
				Go::SplineSurface *s = ((SurfaceDisplay*)selected[i])->surf;
				bool setOnce = false;
				for(uint j=0; j<surfaces.size() && !setOnce; j++) {
					for(uint k=0; k<surfaces[j].size(); k++) {
						if(surfaces[j][k].get() == s)  {
							if(showLines->isSelected()) {
								model.addFacePropertyCode(j, k, code); // this sets all internal codes
								// inclusive endpoints, so we need to colorize edges & vertices as well
								vector<int> edgeLines = Line::getLineEnumeration(k);
								vector<int> edgePoint = Vertex::getVertexEnumeration(k);
								for(int ii=0; ii<4; ii++) {
									CurveDisplay *cd = gui->getDisplayObject(curves[j][edgeLines[ii]].get());
									PointDisplay *pd = gui->getDisplayObject(points[j][edgePoint[ii]].get());
									if(cd) cd->setColor(color.R, color.G, color.B);
									pd->setColor(color.R, color.G, color.B);
								}
							} else {
								model.addFacePropertyCode(j, k, code, false); // not include end lines/vertices
							}
							setOnce = true;
							break;
						}
					}
				}

			} else if(selected[i]->classType() == CURVE) {
				Go::SplineCurve *c = ((CurveDisplay*)selected[i])->curve;
				for(uint j=0; j<curves.size(); j++) {
					for(uint k=0; k<curves[j].size(); k++) {
						if(curves[j][k].get() == c)  {
							if(showPoints->isSelected()) {
								model.addLinePropertyCode(j, k, code);
								int v1, v2;
								if(model.isVolumetricModel())
									Vertex::getVertexEnumerationOnVolume(k, v1, v2);
								else 
									Vertex::getVertexEnumerationOnFace(k, v1, v2);
								PointDisplay *pd = gui->getDisplayObject(points[j][v1].get());
								pd->setColor(color.R, color.G, color.B);
								pd = gui->getDisplayObject(points[j][v2].get());
								pd->setColor(color.R, color.G, color.B);
							} else {
								model.addLinePropertyCode(j, k, code, false);
							}
						}
					}
				}

			} else if(selected[i]->classType() == POINT) {
				Go::Point p = ((PointDisplay*)selected[i])->point;
				for(uint j=0; j<points.size(); j++)
					for(uint k=0; k<points[j].size(); k++)
						if(points[j][k]->dist(p) < 1e-4)
							model.addVertexPropertyCode(j, k, code);
			}
		}
	}
}

void ButtonClick(Button *caller) {
	SplineGUI *gui = SplineGUI::getInstance();
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
}

void colorPrimitive(int patch, DISPLAY_CLASS_TYPE type, int locIndex, int code) {
	SplineGUI *gui = SplineGUI::getInstance();
	HSVType col_hsv;
	col_hsv.H = 6.*((3*code)%10)/9;
	col_hsv.V = 0.7;
	col_hsv.S = (code==0) ? 0.0 : 0.7;
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
	}
}

void readPropertyCodesFromStdin() {
	int patch, primitive, locIndex, code;
	while(cin >> code) {
		DISPLAY_CLASS_TYPE type;
		cin >> patch;
		cin >> primitive;
		if(primitive == 0) {
			cin >> locIndex;
			model.addVertexPropertyCode(patch, locIndex, code);
			type = POINT;
		} else if(primitive == 1) {
			cin >> locIndex;
			model.addLinePropertyCode(patch, locIndex, code, false);
			type = CURVE;
		} else if(primitive == 2) {
			if(model.isVolumetricModel())
				cin >> locIndex;
			model.addFacePropertyCode(patch, locIndex, code, false);
			type = SURFACE;
		} else if(primitive == 3) {
			model.addVolumePropertyCode(patch, code, false);
			type = VOLUME;
		}
		colorPrimitive(patch, type, locIndex, code);
	}
}

void dumpOutput() {
	model.writeModelProperties(std::cout);
}


int main(int argc, char **argv) {
	processParameters(argc, argv);
	TopologySet *topology = model.getTopology();
	if( model.enforceRightHandSystem() ) {
		cerr << "WARNING: system reparameterized to strict right-hand-system. \n";
		cerr << "         stored in \"reparameterized.g2\"\n";
		ofstream outFile;
		outFile.open("reparameterized.g2");
		model.writeSplines(outFile);
		outFile.close();
	}

	bool isVolumeModel = model.isVolumetricModel();

	if(isVolumeModel) {
		volumes = model.getSplineVolumes();
		points.resize(volumes.size());
		for(uint i=0; i<volumes.size(); i++) {
			Go::Array<double, 6> pSpan = volumes[i]->parameterSpan();
			// add faces
			surfaces.push_back(volumes[i]->getBoundarySurfaces(true));
			surfaces[i][1]->reverseParameterDirection(true); // swap direction to make normals point outwards
			surfaces[i][2]->reverseParameterDirection(true);
			surfaces[i][5]->reverseParameterDirection(true);

			// add edges
			vector<shared_ptr<Go::SplineCurve> > c;
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][4]->constParamCurve(pSpan[2], true)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][4]->constParamCurve(pSpan[3], true)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][5]->constParamCurve(pSpan[2], true)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][5]->constParamCurve(pSpan[3], true)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][4]->constParamCurve(pSpan[0], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][4]->constParamCurve(pSpan[1], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][5]->constParamCurve(pSpan[1], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][5]->constParamCurve(pSpan[0], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][2]->constParamCurve(pSpan[1], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][2]->constParamCurve(pSpan[0], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][3]->constParamCurve(pSpan[0], false)) );
			c.push_back(shared_ptr<Go::SplineCurve>(surfaces[i][3]->constParamCurve(pSpan[1], false)) );
			curves.push_back(c);

			// add corners
			for(int w=0; w<2; w++) {
				for(int v=0; v<2; v++) {
					for(int u=0; u<2; u++) {
						Go::Point p;
						volumes[i]->point(p, pSpan[0]*(1-u)+pSpan[1]*u,
											 pSpan[2]*(1-v)+pSpan[3]*v,
											 pSpan[4]*(1-w)+pSpan[5]*w);
						points[i].push_back(shared_ptr<Go::Point>(new Go::Point(p)));
					}
				}
			}
		}
	} else { // surface model
		surfPatches = model.getSplineSurfaces();
		points.resize(surfPatches.size());
		curves.resize(surfPatches.size());
		for(uint i=0; i<surfPatches.size(); i++) {
			Go::RectDomain pSpan = surfPatches[i]->parameterDomain();
			// add edges
			curves[i].push_back(shared_ptr<Go::SplineCurve>(surfPatches[i]->edgeCurve(3)));
			curves[i].push_back(shared_ptr<Go::SplineCurve>(surfPatches[i]->edgeCurve(1)));
			curves[i].push_back(shared_ptr<Go::SplineCurve>(surfPatches[i]->edgeCurve(0)));
			curves[i].push_back(shared_ptr<Go::SplineCurve>(surfPatches[i]->edgeCurve(2)));

			// add corners
			for(int v=0; v<2; v++) {
				for(int u=0; u<2; u++) {
					Go::Point p;
					surfPatches[i]->point(p, pSpan.umin()*(1-u)+pSpan.umax()*u,
										     pSpan.vmin()*(1-v)+pSpan.vmax()*v);
					points[i].push_back(shared_ptr<Go::Point>(new Go::Point(p)));
				}
			}
		}
	}



	set<Vertex*>::iterator vertIt;
	set<Line*>::iterator   lineIt;
	set<Face*>::iterator   faceIt;
	set<Volume*>::iterator volIt;

	SplineGUI *gui = SplineGUI::getInstance();
	for(vertIt=topology->vertex_begin(); vertIt!=topology->vertex_end(); vertIt++) {
		Vertex *v   = *vertIt;
		if(isVolumeModel) {
			Volume *vol = *v->volume.begin();
			vector<int> all_corners = vol->getVertexEnumeration(v);
			int volId  = vol->id;
			int vertId = all_corners[0];
			gui->addObject(points[volId][vertId].get());
			// overwrite all pointers to the one used to create the DisplayObject
			for(volIt=v->volume.begin(); volIt!=v->volume.end(); volIt++) {
				vol = *volIt;
				all_corners = vol->getVertexEnumeration(v);
				for(uint i=0; i<all_corners.size(); i++)
					points[vol->id][all_corners[i]] = points[volId][vertId];
			}
		} else { // surface model
			Face *f = *v->face.begin();
			vector<int> all_corners = f->getVertexEnumeration(v);
			int faceId = f->id;
			int vertId = all_corners[0];
			gui->addObject(points[faceId][vertId].get());
			// overwrite all pointers to the one used to create the DisplayObject
			for(faceIt=v->face.begin(); faceIt!=v->face.end(); faceIt++) {
				f = *faceIt;
				all_corners = f->getVertexEnumeration(v);
				for(uint i=0; i<all_corners.size(); i++)
					points[f->id][all_corners[i]] = points[faceId][vertId];
			}
		}
	}
	for(lineIt=topology->line_begin(); lineIt!=topology->line_end(); lineIt++) {
		Line   *l   = *lineIt;
		if(l->degen) continue;
		if(isVolumeModel) {
			Volume *vol = *l->volume.begin();
			vector<int> numb, parDir, parStep;
			vol->getEdgeEnumeration(l, numb, parDir, parStep);
			int volId  = vol->id;
			int lineId = numb[0];
			gui->addObject(curves[volId][lineId].get(), true);
			// overwrite all pointers to the one used to create the DisplayObject
			for(volIt=l->volume.begin(); volIt!=l->volume.end(); volIt++) {
				vol = *volIt;
				vol->getEdgeEnumeration(l, numb, parDir, parStep);
				for(uint i=0; i<numb.size(); i++)
					curves[vol->id][numb[i]] = curves[volId][lineId];
			}
		} else { // surface model
			Face *f = *l->face.begin();
			vector<int> numb, parDir, parStep;
			f->getEdgeEnumeration(l, numb, parDir, parStep);
			int faceId  = f->id;
			int lineId = numb[0];
			gui->addObject(curves[faceId][lineId].get(), true);
			// overwrite all pointers to the one used to create the DisplayObject
			for(faceIt=l->face.begin(); faceIt!=l->face.end(); faceIt++) {
				f = *faceIt;
				f->getEdgeEnumeration(l, numb, parDir, parStep);
				for(uint i=0; i<numb.size(); i++)
					curves[f->id][numb[i]] = curves[faceId][lineId];
			}
		}
	}
	for(faceIt=topology->face_begin(); faceIt!=topology->face_end(); faceIt++) {
		Face   *f   = *faceIt;
		if(f->isDegen()) continue;
		if(isVolumeModel) {
			Volume *vol = f->volume[0];
			vector<int> all_faces = vol->getSurfaceEnumeration(f);
			int volId  = vol->id;
			int faceId = all_faces[0];
			gui->addObject(surfaces[volId][faceId].get(), false);
			// overwrite all pointers to the one used to create the DisplayObject
			for(uint i=0; i<f->volume.size(); i++) {
				vol = f->volume[i];
				all_faces = vol->getSurfaceEnumeration(f);
				for(uint j=0; j<all_faces.size(); j++)
					surfaces[vol->id][all_faces[j]] = surfaces[volId][faceId];
			}
		} else {
			Face *f = *faceIt;
			gui->addObject(surfPatches[f->id].get());
		}
	}
	for(volIt=topology->volume_begin(); volIt!=topology->volume_end(); volIt++) {
		Volume *vol = *volIt;
		gui->addObject(volumes[vol->id].get());
	}

	DisplayObjectSet *objSet = gui->getObjectSet();
	objSet->enableControlMesh(false);
	objSet->setLineWidth(5);
	objSet->setPointSize(14);
	gui->hideObjects(VOLUME);
	// gui->hideObjects(SURFACE);

	showVolumes->makeOnOffButton();
	showFaces->makeOnOffButton();
	showLines->makeOnOffButton();
	showPoints->makeOnOffButton();

	showVolumes->setSelected(false);
	showFaces->setSelected(true);
	showLines->setSelected(true);
	showPoints->setSelected(true);

	showVolumes->setOnClick(ButtonClick);
	showFaces->setOnClick(ButtonClick);
	showLines->setOnClick(ButtonClick);
	showPoints->setOnClick(ButtonClick);

	if(isVolumeModel)
		gui->addButton(showVolumes);
	gui->addButton(showFaces);
	gui->addButton(showLines);
	gui->addButton(showPoints);

	gui->addKeyboardListener(keyClick);

	gui->setSplineColor(.7, .7, .7);

	// if piped input then add the proper colorcodes
	if(! isatty(STDIN_FILENO) ) 
		readPropertyCodesFromStdin();

	// dump output on program termination
	atexit(dumpOutput); 

	gui->show();

	return 0;
}

