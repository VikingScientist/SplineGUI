
#include "SurfaceDisplay.h"
#include <GoTools/geometry/ObjectHeader.h>
#include "CurveDisplay.h"
#include "SplineGUI.h"

using namespace Go;
using namespace std;

SurfaceDisplay::SurfaceDisplay(SplineSurface *surf, bool clean) {
	this->surf             = surf;
	positions              = NULL;
	normals                = NULL;
	triangle_strip         = NULL;
	param_values           = NULL;
	xi_buffer              = NULL;
	eta_buffer             = NULL;
	cp_pos                 = NULL;
	cp_lines               = NULL;
	resolution[0]          = 0;
	resolution[1]          = 0;
	draw_contol_mesh       = false;
	colorByParametervalues = false;
	faceIndex              = -1;
	setColor(.8, .4, .05); // orange
	// setColor(.6, .6, .6); // light gray

	vector<double> knotval1;
	vector<double> knotval2;
	surf->basis_u().knotsSimple(knotval1);
	surf->basis_v().knotsSimple(knotval2);
	vector<CurvePointer> const_crvs1;
	vector<CurvePointer> const_crvs2;
	surf->getConstParamCurves(knotval1, knotval2, const_crvs1, const_crvs2); 

	if(!clean) {
		for(uint i=0; i<const_crvs1.size(); i++) {
			CurveDisplay *c = new CurveDisplay(const_crvs1[i]->clone(), true);
			c->setOrigin(this);
			knot_lines.push_back(c);
		}
		for(uint i=0; i<const_crvs2.size(); i++) {
			CurveDisplay *c = new CurveDisplay(const_crvs2[i]->clone(), true);
			c->setOrigin(this);
			knot_lines.push_back(c);
		}
	}
}

SurfaceDisplay::~SurfaceDisplay() {
	if(positions)      delete[] positions;
	if(normals)        delete[] normals;
	if(triangle_strip) delete[] triangle_strip;
	if(param_values)   delete[] param_values;
	if(xi_buffer)      delete[] xi_buffer;
	if(eta_buffer)     delete[] eta_buffer;
	if(cp_pos)         delete[] cp_pos;
	if(cp_lines)       delete[] cp_lines;
}

void SurfaceDisplay::setFaceIndex(int i) {
	faceIndex = i;
}

void SurfaceDisplay::tesselate(int *n) {
	if(positions)      delete[] positions;
	if(normals)        delete[] normals;
	if(triangle_strip) delete[] triangle_strip;
	if(param_values)   delete[] param_values;
	if(cp_pos)         delete[] cp_pos;
	if(cp_lines)       delete[] cp_lines;

	int dim = surf->dimension();

	double p_u_min = surf->startparam_u();
	double p_u_max = surf->endparam_u();
	double p_v_min = surf->startparam_v();
	double p_v_max = surf->endparam_v();
	vector<double> pts;
	vector<double> normals_evaluated;
	vector<double> param_u;
	vector<double> param_v;

	// if no tesselation resolution is given, take an educated guess
	if(n==NULL) {
		vector<double> uniqueKnots_u, uniqueKnots_v;
		int pu = surf->order_u();
		int pv = surf->order_v();
		surf->basis_u().knotsSimple(uniqueKnots_u);
		surf->basis_v().knotsSimple(uniqueKnots_v);
		n = new int[2];
		n[0] = (uniqueKnots_u.size()-1)*(pu-1)*3;
		n[1] = (uniqueKnots_v.size()-1)*(pv-1)*3;
	}

	// evaluate the surface parameters coordinates
	surf->gridEvaluator(n[0], n[1], pts, normals_evaluated, param_u, param_v, false);

	// store the number of the different primitives
	int nCoef[2];
	resolution[0]  = n[0];
	resolution[1]  = n[1];
	triangle_count = (n[0]*2+2)*(n[1]-1);
	nCoef[0]       = surf->numCoefs_u();
	nCoef[1]       = surf->numCoefs_v();
	cp_count       = nCoef[0]*nCoef[1];
	line_count     = (nCoef[0]-1)*nCoef[1] + nCoef[0]*(nCoef[1]-1);
	line_count    *= 2; // two indices for each line-piece

	// allocate memory
	triangle_strip = new GLuint[triangle_count];
	positions      = new GLdouble[n[0]*n[1]*dim];
	normals        = new GLdouble[n[0]*n[1]*dim];
	param_values   = new GLdouble[n[0]*n[1]*3];
	cp_pos         = new GLdouble[cp_count*3];
	cp_lines       = new GLuint[line_count];

	// store the parametrization (i.e. (u,v,w)-coordinates which maps to the (x,y,z)-coordinates)
	int k=0;
	for(int j=0; j<n[1]; j++) {
		for(int i=0; i<n[0]; i++) {
			if(faceIndex < 0) {
				param_values[k++] = (param_u[i]-p_u_min)/(p_u_max-p_u_min);
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
				param_values[k++] = 0;
			} else if(faceIndex == 0) {
				param_values[k++] = 0;
				param_values[k++] = (param_u[i]-p_u_min)/(p_u_max-p_u_min);
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
			} else if(faceIndex == 1) { // reversed parameter direction to get right normals
				param_values[k++] = 1;
				param_values[k++] = (p_u_max-param_u[i])/(p_u_max-p_u_min);
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
			} else if(faceIndex == 2) { // reversed parameter direction to get right normals
				param_values[k++] = (p_u_max-param_u[i])/(p_u_max-p_u_min);
				param_values[k++] = 0;
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
			} else if(faceIndex == 3) {
				param_values[k++] = (param_u[i]-p_u_min)/(p_u_max-p_u_min);
				param_values[k++] = 1;
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
			} else if(faceIndex == 4) {
				param_values[k++] = (param_u[i]-p_u_min)/(p_u_max-p_u_min);
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
				param_values[k++] = 0;
			} else if(faceIndex == 5) { // reversed parameter direction to get right normals
				param_values[k++] = (p_u_max-param_u[i])/(p_u_max-p_u_min);
				param_values[k++] = (param_v[j]-p_v_min)/(p_v_max-p_v_min);
				param_values[k++] = 1;
			}
		}
	}

	// store the actual geometric data points
	k = 0;
	for(int j=0; j<n[1]; j++) {
		for(int i=0; i<n[0]; i++) {
			for(int d=0; d<dim; d++) {
				positions[k] = pts[k];
				normals[k]   = normals_evaluated[k];
				k++;
			}
		}
	}

	// construct the triangulation
	k = 0;
	for(int j=0; j<n[1]-1; j++) {
		for(int i=0; i<n[0]; i++) {
			triangle_strip[k++] =   j  *n[0] + i;
			triangle_strip[k++] = (j+1)*n[0] + i;
		}
		triangle_strip[k++] = (j+2)*n[0] - 1;
		triangle_strip[k++] = (j+1)*n[0];
	}

	// store the control points in case we want to see the control mesh
	vector<double>::iterator cp;
	if(surf->rational()) {
		cp = surf->rcoefs_begin();
		int i=0;
		while(cp != surf->rcoefs_end() ) {
			cp_pos[i  ] = *cp++;
			cp_pos[i+1] = *cp++;
			cp_pos[i+2] = *cp++;
			// goTools store CP premultiplied by weight
			// here we divide this away
			cp_pos[i++] /= *cp; 
			cp_pos[i++] /= *cp;
			cp_pos[i++] /= *cp;
			cp++;
		}
	} else {
		cp = surf->coefs_begin();
		int i = 0;
		while(cp != surf->coefs_end() )
			cp_pos[i++] = *cp++;
	}
	int c = 0;
	for(int j=0; j<nCoef[1]; j++) {
		for(int i=0; i<nCoef[0]-1; i++) {
			cp_lines[c++] = j*nCoef[0] + i;
			cp_lines[c++] = j*nCoef[0] + i+1;
		}
	}
	for(int j=0; j<nCoef[1]-1; j++) {
		for(int i=0; i<nCoef[0]; i++) {
			cp_lines[c++] =   j  *nCoef[0] + i;
			cp_lines[c++] = (j+1)*nCoef[0] + i;
		}
	}
	
	// tesselate all knot lines owned by this surface
	for(uint i=0; i<knot_lines.size(); i++)
		knot_lines[i]->tesselate();

}

void SurfaceDisplay::paint() {
	if(draw_contol_mesh) {
		glColor3f(0,0,0);
		glLineWidth(2);
		glPointSize(6);
		glVertexPointer(3, GL_DOUBLE, 0, cp_pos);
		glDrawElements(GL_LINES, line_count, GL_UNSIGNED_INT, cp_lines);
		glDrawArrays(GL_POINTS, 0, cp_count);
	} else {
		glEnable(GL_LIGHTING);
		glEnableClientState(GL_NORMAL_ARRAY);
		glColor3f(color[0], color[1], color[2]);
		glVertexPointer(3, GL_DOUBLE, 0, positions);
		glNormalPointer(GL_DOUBLE, 0, normals);
		glDrawElements(GL_TRIANGLE_STRIP, triangle_count, GL_UNSIGNED_INT, triangle_strip);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_LIGHTING);
	
		for(uint i=0; i<knot_lines.size(); i++)
			knot_lines[i]->paint();
	}
}

void SurfaceDisplay::paintSelected() {
	glEnable(GL_LIGHTING);
	glColor3f(selected_color[0], selected_color[1], selected_color[2]);
	glEnableClientState(GL_NORMAL_ARRAY);
	if(colorByParametervalues) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_DOUBLE, 0, param_values);
	}
	glVertexPointer(3, GL_DOUBLE, 0, positions);
	glNormalPointer(GL_DOUBLE, 0, normals);
	glDrawElements(GL_TRIANGLE_STRIP, triangle_count, GL_UNSIGNED_INT, triangle_strip);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_LIGHTING);
	if(colorByParametervalues)
		glDisableClientState(GL_COLOR_ARRAY);

	for(uint i=0; i<knot_lines.size(); i++)
		knot_lines[i]->paint();
}

void SurfaceDisplay::paintMouseAreas() {
	glVertexPointer(3, GL_DOUBLE, 0, positions);
	glColorPointer(3, GL_DOUBLE, 0, param_values);
	glDrawElements(GL_TRIANGLE_STRIP, triangle_count, GL_UNSIGNED_INT, triangle_strip);
}

void SurfaceDisplay::readMouseAreas() {
	if(xi_buffer) delete[] xi_buffer;
	if(eta_buffer) delete[] eta_buffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	xi_buffer = new GLfloat[width*height];
	eta_buffer = new GLfloat[width*height];
	glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, xi_buffer);
	glReadPixels(0, 0, width, height, GL_GREEN, GL_FLOAT, eta_buffer);
}

void SurfaceDisplay::paintMouseAreas(float r, float g, float b) {
	glVertexPointer(3, GL_DOUBLE, 0, positions);
	glColor3f(r,g,b);
	glDrawElements(GL_TRIANGLE_STRIP, triangle_count, GL_UNSIGNED_INT, triangle_strip);
}

bool SurfaceDisplay::isAtPosition(int x, int y) {
	return (xi_buffer[y*width+x] > 0.0) || (eta_buffer[y*width+x] > 0.0);
}

double* SurfaceDisplay::parValueAtPosition(int x, int y) {
	double *ans = new double[2];
	ans[0] = xi_buffer[y*width+x];
	ans[1] = eta_buffer[y*width+x];
	return ans;
}

void SurfaceDisplay::setDrawControlMesh(bool draw) {
	draw_contol_mesh = draw;
}

void SurfaceDisplay::setColorByParameterValues(bool draw) {
	colorByParametervalues = draw;
}


void SurfaceDisplay::processMouse(int button, int state, int x, int y) {
}

void SurfaceDisplay::processMouseActiveMotion(int x, int y) {
}

void SurfaceDisplay::processMousePassiveMotion(int x, int y) {
}

void SurfaceDisplay::printDebugInfo() {
	cout << *surf;
}

void SurfaceDisplay::print(ostream *out) {
	ObjectHeader head(Class_SplineSurface, 1, 0);
	(*out) << head;
	(*out) << *surf;
}

void SurfaceDisplay::myGridEvaluator(int n1, int n2, vector<double>& pts, vector<double>& nor, vector<double>& param_u, vector<double>& param_v) {
	double parm_u_length = surf->endparam_u() - surf->startparam_u();
	double parm_v_length = surf->endparam_v() - surf->startparam_v();
	for(int i=0; i<n1; i++) 
		param_u.push_back( surf->startparam_u() + parm_u_length*i/(n1-1) );
	for(int i=0; i<n2; i++) 
		param_v.push_back( surf->startparam_v() + parm_v_length*i/(n2-1) );
	vector<vector<double> > basis_values;
	surf->computeBasisGrid(param_u, param_v, basis_values);
	int dim = surf->dimension();

	pts.resize(basis_values.size()*dim, 0);
	nor.resize(basis_values.size()*dim, 0);
	if(surf->rational() ) {
		vector<double>::const_iterator cp = surf->rcoefs_begin();
		vector<double> weights;
		surf->getWeights(weights);
		// for(uint i=0; i<basis_values[i].size(); i++)
			// printf("CP #%d = [%f, %f]\n", i, cp[i], weights[i]);
		int aa = 0;
		for(uint i=0; i<basis_values.size(); i++) {
			for(int d=0; d<dim; d++) {
				int bb = d;
				for(uint j=0; j<basis_values[i].size(); j++) {
					pts[aa] += basis_values[i][j]*cp[bb]/weights[j];
					bb += dim+1;
				}
				++aa;
			}
		}
	} else {
		vector<double>::const_iterator cp = surf->coefs_begin();
		int aa = 0;
		for(uint i=0; i<basis_values.size(); i++) {
			for(int d=0; d<dim; d++) {
				int bb = d;
				for(uint j=0; j<basis_values[i].size(); j++) {
					pts[aa] += basis_values[i][j]*cp[bb];
					bb += dim;
				}
				++aa;
			}
		}
	}
}

void SurfaceDisplay::initMouseMasks() {
	if(xi_buffer) delete[] xi_buffer;
	if(eta_buffer) delete[] eta_buffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	xi_buffer = new GLfloat[width*height];
	eta_buffer = new GLfloat[width*height];
	// for(int i=0; i<width*height; i++) {
		// xi_buffer[i] = 0.0;
		// eta_buffer[i] = 0.0;
	// }
}

void SurfaceDisplay::setMaskPos(int x, int y, bool value) {
	xi_buffer[y*width+x] = (value) ? 1.0 : 0.0;
}

