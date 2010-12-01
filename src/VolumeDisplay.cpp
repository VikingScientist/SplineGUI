#include "VolumeDisplay.h"
#include "SurfaceDisplay.h"
#include <GL/glut.h>
#include <GoTools/geometry/ObjectHeader.h>

VolumeDisplay::VolumeDisplay(SplineVolume *volume) {
	this->volume = volume;
	positions      = NULL;
	normals        = NULL;
	triangle_strip = NULL;
	param_values   = NULL;
	wallbuffer     = NULL;
	cp_pos         = NULL;
	cp_lines       = NULL;
	resolution[0]  = 0;
	resolution[1]  = 0;
	resolution[2]  = 0;
	draw_contol_mesh = false;

	vector<boost::shared_ptr<SplineSurface> > edges = volume->getBoundarySurfaces(true);
	for(int i=0; i<6; i++) {
		walls.push_back( new SurfaceDisplay(edges[i]->clone()) );
		walls[i]->setFaceIndex(i);
		walls[i]->setOrigin(this);
	}
	/*  === Number the sides ===
	walls[0]->setColor(0.,0.,1.);
	walls[1]->setColor(0.,1.,0.);
	walls[2]->setColor(0.,1.,1.);
	walls[3]->setColor(1.,0.,0.);
	walls[4]->setColor(1.,0.,1.);
	walls[5]->setColor(1.,1.,0.);
	*/
	// setColor(.6, .6, .6); // light gray
	setColor(.8, .4, .05); // orange

	walls[1]->surf->reverseParameterDirection(true);
	walls[2]->surf->reverseParameterDirection(true);
	walls[5]->surf->reverseParameterDirection(true);
}

VolumeDisplay::~VolumeDisplay() {
	if(positions)      delete[] positions;
	if(normals)        delete[] normals;
	if(triangle_strip) delete[] triangle_strip;
	if(param_values)   delete[] param_values;
	if(wallbuffer)     delete[] wallbuffer;
	if(cp_pos)         delete[] cp_pos;
	if(cp_lines)       delete[] cp_lines;
}

void VolumeDisplay::setSelectedColor(double r, double g, double b) {
	DisplayObject::setSelectedColor(r,g,b);
	for(int i=0; i<6; i++)
		walls[i]->setSelectedColor(r,g,b);
}

void VolumeDisplay::setColor(double r, double g, double b) {
	DisplayObject::setColor(r,g,b);
	for(int i=0; i<6; i++)
		walls[i]->setColor(r,g,b);
}

void VolumeDisplay::tesselate(int *n) {
	int xi_eta[]   = {n[0], n[1]};
	int xi_zeta[]  = {n[0], n[2]};
	int eta_zeta[] = {n[1], n[2]};
	resolution[0]  = n[0];
	resolution[1]  = n[1];
	resolution[2]  = n[2];
	
	/*
	walls[0]->tesselate(eta_zeta);
	walls[1]->tesselate(eta_zeta);
	walls[2]->tesselate(xi_zeta);
	walls[3]->tesselate(xi_zeta);
	walls[4]->tesselate(xi_eta);
	walls[5]->tesselate(xi_eta);
	*/
	for(int i=0; i<6; i++)
		walls[i]->tesselate();

	if(cp_pos)   delete[] cp_pos;
	if(cp_lines) delete[] cp_lines;
	int nCoef[3];
	nCoef[0] = volume->numCoefs(0);
	nCoef[1] = volume->numCoefs(1);
	nCoef[2] = volume->numCoefs(2);
	line_count = (nCoef[0]-1)*nCoef[1]*nCoef[2] + (nCoef[1]-1)*nCoef[0]*nCoef[2] + (nCoef[2]-1)*nCoef[0]*nCoef[1];
	line_count *= 2; // two indices for each line-piece
	cp_count = nCoef[0]*nCoef[1]*nCoef[2];
	cp_pos = new GLdouble[cp_count*3];
	cp_lines = new GLuint[line_count];
	vector<double>::iterator cp;
	if(volume->rational()) {
		cp = volume->rcoefs_begin();
		int i=0;
		while(cp != volume->rcoefs_end() ) {
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
		cp = volume->coefs_begin();
		int i = 0;
		while(cp != volume->coefs_end() )
			cp_pos[i++] = *cp++;
	}
	int c = 0;
	for(int k=0; k<nCoef[2]; k++) {
		for(int j=0; j<nCoef[1]; j++) {
			for(int i=0; i<nCoef[0]-1; i++) {
				cp_lines[c++] = k*nCoef[0]*nCoef[1] + j*nCoef[0] + i;
				cp_lines[c++] = k*nCoef[0]*nCoef[1] + j*nCoef[0] + i+1;
			}
		}
	}
	for(int k=0; k<nCoef[2]; k++) {
		for(int j=0; j<nCoef[1]-1; j++) {
			for(int i=0; i<nCoef[0]; i++) {
				cp_lines[c++] = k*nCoef[0]*nCoef[1] +   j  *nCoef[0] + i;
				cp_lines[c++] = k*nCoef[0]*nCoef[1] + (j+1)*nCoef[0] + i;
			}
		}
	}
	for(int k=0; k<nCoef[2]-1; k++) {
		for(int j=0; j<nCoef[1]; j++) {
			for(int i=0; i<nCoef[0]; i++) {
				cp_lines[c++] =   k  *nCoef[0]*nCoef[1] + j*nCoef[0] + i;
				cp_lines[c++] = (k+1)*nCoef[0]*nCoef[1] + j*nCoef[0] + i;
			}
		}
	}
	
}

void VolumeDisplay::paint() {
	if(draw_contol_mesh) {
		glColor3f(0,0,0);
		glLineWidth(2);
		glPointSize(6);
		glVertexPointer(3, GL_DOUBLE, 0, cp_pos);
		glDrawElements(GL_LINES, line_count, GL_UNSIGNED_INT, cp_lines);
		glDrawArrays(GL_POINTS, 0, cp_count);
	} else {
		for(int i=0; i<6; i++)
			if(! ((DisplayObject*)walls[i])->isSelected())
				walls[i]->paint();
	}
}

void VolumeDisplay::paintSelected() {
	for(int i=0; i<6; i++)
		walls[i]->paintSelected();
}

void VolumeDisplay::paintMouseAreas() {
	glDisable(GL_COLOR_ARRAY);
	walls[0]->paintMouseAreas(0,0,1);
	walls[1]->paintMouseAreas(0,1,0);
	walls[2]->paintMouseAreas(0,1,1);
	walls[3]->paintMouseAreas(1,0,0);
	walls[4]->paintMouseAreas(1,0,1);
	walls[5]->paintMouseAreas(1,1,0);
	glEnable(GL_COLOR_ARRAY);
}

void VolumeDisplay::readMouseAreas() {
	if(wallbuffer) delete[] wallbuffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	wallbuffer = new GLfloat[3*width*height];
	glReadPixels(0, 0, width, height, GL_RGB,  GL_FLOAT, wallbuffer);
}

bool VolumeDisplay::isAtPosition(int x, int y) {
	int k = (y*width+x)*3;
	// return (xi_buffer[y*width+x] > 0.0) || (eta_buffer[y*width+x] > 0.0);
	return wallbuffer[k]>0 || wallbuffer[k+1]>0 || wallbuffer[k+2]>0;
}

SurfaceDisplay* VolumeDisplay::getSurfaceAt(int x, int y) {
	int k = (y*width+x)*3;
	if(wallbuffer[k]==0 && wallbuffer[k+1]==0 && wallbuffer[k+2]==1)      // 001
		return walls[0];
	else if(wallbuffer[k]==0 && wallbuffer[k+1]==1 && wallbuffer[k+2]==0) // 010
		return walls[1];
	else if(wallbuffer[k]==0 && wallbuffer[k+1]==1 && wallbuffer[k+2]==1) // 011
		return walls[2];
	else if(wallbuffer[k]==1 && wallbuffer[k+1]==0 && wallbuffer[k+2]==0) // 100
		return walls[3];
	else if(wallbuffer[k]==1 && wallbuffer[k+1]==0 && wallbuffer[k+2]==1) // 101
		return walls[4];
	else if(wallbuffer[k]==1 && wallbuffer[k+1]==1 && wallbuffer[k+2]==0) // 110
		return walls[5];
	else
		return NULL;
}


void VolumeDisplay::processMouse(int button, int state, int x, int y) {
	/*
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && xi_buffer[y*width+x] > 0.0) {
		selected = !selected;
	}
	*/
}

void VolumeDisplay::processMouseActiveMotion(int x, int y) {
}

void VolumeDisplay::processMousePassiveMotion(int x, int y) {
}

void VolumeDisplay::printDebugInfo() {
	cout << *volume;
}

void VolumeDisplay::print(ostream *out) {
	ObjectHeader head(Class_SplineVolume, 1, 0);
	(*out) << head;
	(*out) << *volume;
}

void VolumeDisplay::setDrawControlMesh(bool draw) {
	draw_contol_mesh = draw;
}

void VolumeDisplay::initMouseMasks() {
	if(wallbuffer) delete[] wallbuffer;
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	wallbuffer = new GLfloat[3*width*height];
	// for(int i=0; i<3*width*height; i++)
		// wallbuffer[i] = 0.0;
}

void VolumeDisplay::setMaskPos(int x, int y, bool value) {
	int k = (y*width+x)*3;
	wallbuffer[k] = (value) ? 1.0 : 0.0;
}

void VolumeDisplay::paintMouseAreas(float r, float g, float b) {
	walls[0]->paintMouseAreas(r,g,b);
	walls[1]->paintMouseAreas(r,g,b);
	walls[2]->paintMouseAreas(r,g,b);
	walls[3]->paintMouseAreas(r,g,b);
	walls[4]->paintMouseAreas(r,g,b);
	walls[5]->paintMouseAreas(r,g,b);
}

