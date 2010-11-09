
#include "SurfaceDisplay.h"
#include <GoTools/geometry/ObjectHeader.h>
#include "CurveDisplay.h"

using namespace std;

SurfaceDisplay::SurfaceDisplay(SplineSurface *surf, bool clean) {
	this->surf = surf;
	positions      = NULL;
	normals        = NULL;
	triangle_strip = NULL;
	param_values   = NULL;
	xi_buffer      = NULL;
	eta_buffer     = NULL;
	resolution[0]  = 0;
	resolution[1]  = 0;
	selected       = false;
	faceIndex      = -1;
	setColor(.8, .4, .05); // orange
	// setColor(.6, .6, .6); // light gray

	vector<double> knotval1;
	vector<double> knotval2;
	surf->basis_u().knotsSimple(knotval1);
	surf->basis_v().knotsSimple(knotval2);
	vector<boost::shared_ptr<SplineCurve> > const_crvs1;
	vector<boost::shared_ptr<SplineCurve> > const_crvs2;
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
}

void SurfaceDisplay::setFaceIndex(int i) {
	faceIndex = i;
}

void SurfaceDisplay::tesselate(int *n) {
	if(positions)      delete[] positions;
	if(normals)        delete[] normals;
	if(triangle_strip) delete[] triangle_strip;
	if(param_values)   delete[] param_values;
	int dim = surf->dimension();

	resolution[0]  = n[0];
	resolution[1]  = n[1];
	triangle_count = (n[0]*2+2)*(n[1]-1);
	triangle_strip = new GLuint[triangle_count];
	positions      = new GLdouble[n[0]*n[1]*dim];
	normals        = new GLdouble[n[0]*n[1]*dim];
	param_values   = new GLdouble[n[0]*n[1]*3];
	double p_u_min = surf->startparam_u();
	double p_u_max = surf->endparam_u();
	double p_v_min = surf->startparam_v();
	double p_v_max = surf->endparam_v();
	vector<double> pts;
	vector<double> normals_evaluated;
	vector<double> param_u;
	vector<double> param_v;
	// myGridEvaluator(n[0], n[1], pts, normals_evaluated, param_u, param_v);
	surf->gridEvaluator(n[0], n[1], pts, normals_evaluated, param_u, param_v, false);
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
	k = 0;
	for(int j=0; j<n[1]-1; j++) {
		for(int i=0; i<n[0]; i++) {
			triangle_strip[k++] =   j  *n[0] + i;
			triangle_strip[k++] = (j+1)*n[0] + i;
		}
		triangle_strip[k++] = (j+2)*n[0] - 1;
		triangle_strip[k++] = (j+1)*n[0];
	}
	
	for(uint i=0; i<knot_lines.size(); i++)
		knot_lines[i]->tesselate(n);

}

void SurfaceDisplay::paint() {
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

void SurfaceDisplay::paintSelected() {
	glEnable(GL_LIGHTING);
	glColor3f(selected_color[0], selected_color[1], selected_color[2]);
	// glEnableClientState(GL_NORMAL_ARRAY);
	// glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_DOUBLE, 0, param_values);
	glVertexPointer(3, GL_DOUBLE, 0, positions);
	glNormalPointer(GL_DOUBLE, 0, normals);
	glDrawElements(GL_TRIANGLE_STRIP, triangle_count, GL_UNSIGNED_INT, triangle_strip);
	// glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_LIGHTING);

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


void SurfaceDisplay::processMouse(int button, int state, int x, int y) {
	
	/*
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && xi_buffer[y*width+x] > 0.0) {
		selected = !selected;
	}
	*/
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

