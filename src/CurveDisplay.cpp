//==============================================================================
//!
//! \file CurveDisplay.cpp
//!
//! \date July 2010
//!
//! \author Kjetil A. Johannessen / SINTEF
//!
//! \brief Display wrapper for Spline Curves
//! 
//==============================================================================

#include "CurveDisplay.h"
#include "CurvePoint.h"
#include "SplineGUI.h"
#include <GoTools/geometry/ObjectHeader.h>
#include <algorithm>

using namespace Go;
using namespace std;

/**********************************************************************************//**
 * \brief Constructor
 * \param curve the actual Spline Curve data (i.e. knots and control points etc)
 * \param clean false will render a PointDisplay on the start and end point of the curve
 *************************************************************************************/
CurveDisplay::CurveDisplay(SplineCurve *curve, bool clean) {
	this->curve   = curve;
	line_width    = 2;
	positions     = NULL;
	param_values  = NULL;
	xi_buffer     = NULL;
	resolution    = 0;
	selected      = false;
	dim           = curve->dimension();
	if(!clean) {
		Point p ;
		curve->point(p, curve->startparam() );
		start_p = new PointDisplay(p);
		curve->point(p, curve->endparam() );
		stop_p  = new PointDisplay(p);
	} else {
		start_p = NULL;
		stop_p  = NULL;
	}
	setColor(0,0,0);
	setSelectedColor(.8,.8,.8);
}

//! \brief destructor
CurveDisplay::~CurveDisplay() {
	if(positions)    delete[] positions;
	if(param_values) delete[] param_values;
	if(xi_buffer)    delete[] xi_buffer;
	delete curve;
}

bool CurveDisplay::splitPeriodicCurveInFour(vector<CurvePoint*> splits, vector<SplineCurve*> &ret_val) {
	if(splits.size() != 4)
		return false;
	ret_val.clear();
	double param[4];
	for(int i=0; i<4; i++)
		param[i] = splits[i]->getParValue();
	double start_u = param[0];
	double tmp;
	int split_i = 3; // the index where param[i%4]>param[(i+1)%4] i.e. contains the start/end-point of the curve

	sort(param, param+4);
	// if one of the splits is exactly at the start-/endpoint, then we use the GoTools-splitting
	if(param[0] == 0.0) {
		vector<double> splitting_params;
		for(int i=1; i<4; i++)
			splitting_params.push_back(curve->startparam() + param[i]*(curve->endparam() - curve->startparam()) );
		vector<CurvePointer> split_from_gotools = curve->split(splitting_params);
		for(int i=0; i<4; i++) // make duplicate objects since shared_ptr objects are deleted on function return (at least boost)
			ret_val.push_back( split_from_gotools[i]->clone() );
		return true;
	}
	// make the parametric values to the true values instead of (0,1)
	for(int i=0; i<4; i++) {
		param[i] = curve->startparam() + param[i]*(curve->endparam() - curve->startparam());
		curve->basis().knotIntervalFuzzy(param[i], 1e-3); // if splits are close to existing knots, we snap
	}
	start_u = curve->startparam() + start_u*(curve->endparam() - curve->startparam());
	curve->basis().knotIntervalFuzzy(start_u, 1e-3); // if splits are close to existing knots, we snap

	vector<double> knots, new_knots;
	for(int i=0; i<4; i++)
		for(int j=0; j<curve->order(); j++)
			knots.push_back(param[i]);
	// extract the knots to insert
	set_difference(knots.begin(), knots.end(),
	               curve->basis().begin(), curve->basis().end(),
				   back_inserter(new_knots));

	// insert new knots
	curve->insertKnot(new_knots);

	// make the splits cyclic sorted with the correct start (important to cycle after call to set_difference as this relies on sorted lists)
	while(param[0] != start_u) {
		tmp = param[0];
		for(int i=0; i<3;i++)
			param[i] = param[i+1];
		param[3] = tmp;
		split_i--;
	}


	// extract sub curves
	int kk = curve->order();
	int dd = curve->dimension();
	vector<double>::iterator end   = curve->basis().end();
	vector<double>::iterator start = curve->basis().begin();
	SplineCurve* the_subCurve;
	vector<double>::const_iterator coefs_start;
	for(int i=0; i<4; i++) {
		if(i == split_i) {
			vector<double>::iterator knotstart  = find(start, end, param[i]);
			vector<double>::iterator knotend    = find(start, end, param[(i+1)%4]);
			int n = (end-knotstart-1) + (knotend-start);
			vector<double>::iterator coefsstart = curve->coefs_begin() + dd*(         knotstart-start);
			vector<double>::iterator coefsend   = curve->coefs_begin() + dd*( (n-kk)-(end-knotstart-kk-1)); //need (n-kk) coefs. (end-knotstart-kk-1) copied from the end

			vector<double> subCurve_knots(n);
			vector<double> subCurve_coefs((n-kk)*dd);
			copy(knotstart, end-1, subCurve_knots.begin());
			copy(coefsstart, curve->coefs_end()-dd, subCurve_coefs.begin());
			vector<double>::iterator cur = start+kk;
			for(int j=(end-knotstart-1); j<n; j++)
				subCurve_knots[j] = (*cur++) + *(end-1)-*start;

			copy(curve->coefs_begin(), coefsend, subCurve_coefs.begin() + dd*(end-knotstart-kk-1) );

			the_subCurve = new SplineCurve(n-kk, kk, subCurve_knots.begin(), subCurve_coefs.begin(), dd);
			ret_val.push_back(the_subCurve);
			
		} else {
			vector<double>::iterator knotstart = find(start, end, param[i]);
			vector<double>::iterator knotend = find(start, end, param[(i+1)%4]);
			vector<double>::iterator coefsstart = curve->coefs_begin() + dd*(knotstart-start);
			the_subCurve = new SplineCurve(knotend-knotstart, kk,
			                               knotstart, coefsstart, dd);
			ret_val.push_back(the_subCurve);
		}
	}
	
	return true;
}

//! \brief tesselates the curve with the stored resoltuion
void CurveDisplay::reTesselate() {
	tesselate(&resolution);
}

/**********************************************************************************//**
 * \brief tesselates the curve
 * \param n pointer to the number of tesselation points (DisplayCurve takes int[1] as input while DisplaySurface takes n[2] and Volume n[3])
 *************************************************************************************/
void CurveDisplay::tesselate(int *n) {
	if(positions)    delete[] positions;
	if(param_values) delete[] param_values;

	double p_min = curve->startparam();
	double p_max = curve->endparam();
	vector<Point> pts;
	vector<double> param;
	if(n==NULL) {
		n = new int[1];
		vector<double> uniqueKnots;
		curve->basis().knotsSimple(uniqueKnots);
		/*
		int pu = curve->order();
		n[0]   = (pu-1)*3;
		for(uint i=0; i<uniqueKnots.size()-1; i++)
			for(int j=0; j<n[0]; j++)
				param.push_back( uniqueKnots[i] + j*(uniqueKnots[i+1]-uniqueKnots[i])/(n[0]) );
		param.push_back(uniqueKnots.back());
		n[0] = param.size();
		for(int i=0; i<n[0]; i++) {
			Point p;
			curve->point(p, param[i]);
			pts.push_back(p);
		}
	} else {
		curve->uniformEvaluator(n[0], pts, param);
	}
	*/
		n[0] = (uniqueKnots.size()-1)*(curve->order()-1)*3;
	}
	curve->uniformEvaluator(n[0], pts, param);

	resolution = n[0];
	positions    = new double[n[0]*dim];
	param_values = new double[n[0]*3];
	int k=0;
	for(int i=0; i<resolution; i++) {
		for(int d=0; d<pts[i].dimension(); d++) {
			positions[k++] = pts[i][d];
		}
		param_values[i*3  ] = (param[i]-p_min)/(p_max-p_min);
		param_values[i*3+1] = 0;
		param_values[i*3+2] = 0;
	}
	if(start_p) start_p->tesselate(&resolution);
	if(stop_p) stop_p->tesselate(&resolution);
}

//! \brief draw the curve using the proper GL calls
void CurveDisplay::paint() {
	glColor3f(color[0],color[1],color[2]);
	glLineWidth(line_width);
	glVertexPointer(dim, GL_DOUBLE, 0, positions);
	glDrawArrays(GL_LINE_STRIP, 0, resolution);
	if(start_p) start_p->paint();
	if(stop_p)  stop_p->paint();
}

//! \brief draw the curve if it is selected by the user (brighter colors)
void CurveDisplay::paintSelected() {
	glColor3f(selected_color[0], selected_color[1], selected_color[2]);
	glLineWidth(line_width);
	glVertexPointer(dim, GL_DOUBLE, 0, positions);
	glDrawArrays(GL_LINE_STRIP, 0, resolution);
	if(start_p) start_p->paintSelected();
	if(stop_p)  stop_p->paintSelected();
}

//! \brief draws the hidden buffer used for mouse input methods
void CurveDisplay::paintMouseAreas() {
	glLineWidth(line_width+8);
	glVertexPointer(dim, GL_DOUBLE, 0, positions);
	glColorPointer(3, GL_DOUBLE, 0, param_values);
	glDrawArrays(GL_LINE_STRIP, 0, resolution);
}

//! \brief reads the hidden buffer used for mouse input methods
void CurveDisplay::readMouseAreas() {
	if(xi_buffer) delete[] xi_buffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	xi_buffer = new GLfloat[width*height];
	glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, xi_buffer);
}

//! \brief returns true if the curve is at the given screen (x,y) coordinates
bool CurveDisplay::isAtPosition(int x, int y) {
	return (xi_buffer[y*width+x] > 0.0);
}

//! \brief returns the parameter value of the curve at screen (x,y) position (normalized to (0,1) )
double CurveDisplay::parValueAtPosition(int x, int y) {
	return xi_buffer[y*width+x];
}


void CurveDisplay::processMouse(int button, int state, int x, int y) {
	
	/*
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && xi_buffer[y*width+x] > 0.0) {
		selected = !selected;
		fireActionEvent(CURVE_SELECTED);
	}
	*/
}

void CurveDisplay::processMouseActiveMotion(int x, int y) {
}

void CurveDisplay::processMousePassiveMotion(int x, int y) {
}

/*
void CurveDisplay::addActionListener(void (*actionPerformed)(ActiveObject*, int )) {
	this->actionPerformed = actionPerformed;
}
*/

//! \brief dumps the knot vector and control points to standard out
void CurveDisplay::printDebugInfo() {
	cout << *curve;
}

//! \brief writes a proper .g2-format (GoTools) representation of the curve to &out
void CurveDisplay::print(ostream *out) {
	ObjectHeader head(Class_SplineCurve, 1, 0);
	(*out) << head;
	(*out) << *curve;
}

void CurveDisplay::initMouseMasks() {
	if(xi_buffer) delete[] xi_buffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	xi_buffer = new GLfloat[width*height];
	// for(int i=0; i<width*height; i++)
		// xi_buffer[i] = 0.0;
}

void CurveDisplay::setMaskPos(int x, int y, bool value) {
	xi_buffer[y*width+x] = (value) ? 1.0 : 0.0;
}

void CurveDisplay::getBoundingBox(BoundingBox &box) const { 
	box = curve->boundingBox();
}

void CurveDisplay::paintMouseAreas(float r, float g, float b) {
	glLineWidth(line_width+8);
	glColor3f(r,g,b);
	glVertexPointer(dim, GL_DOUBLE, 0, positions);
	glDrawArrays(GL_LINE_STRIP, 0, resolution);
}

//! \brief sets the curve render width (measured in pixels)
void CurveDisplay::setLineWidth(int line_width) {
	this->line_width = line_width;
}
