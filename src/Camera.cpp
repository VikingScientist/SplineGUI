//==============================================================================
//!
//! \file Camera.cpp
//!
//! \date July 2010
//!
//! \author Kjetil A. Johannessen / SINTEF
//!
//! \brief Camera render view window
//! 
//==============================================================================

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include "Camera.h"
#include "ActiveObject.h"
#include <stdio.h>

using namespace std;

// set by trial and error
static const GLfloat speed_scale_rotate = 0.003;
static const GLfloat speed_scale_zoom = 0.10;
static const GLfloat speed_scale_pan = 0.0014;

void Camera::handleResize(int x, int y, int w, int h) {
	MVPHandler::handleResize(x,y, w, h);
	vp_width = w;
	vp_height = h;
	// setProjection();
	// fireActionEvent(ACTION_REQUEST_REPAINT | ACTION_REQUEST_REMASK);
}

/**********************************************************************************//**
 * \brief Default constructor
 * Sets a camera at 15 length units from the origin and looking towards if from 
 * a default angle
 *************************************************************************************/
Camera::Camera() {
	r                       = 15;
	phi                     = M_PI_4;
	theta                   = M_PI_4;
	look_at_x               = 0;
	look_at_y               = 0;
	look_at_z               = 0;
	right_mouse_button_down = false;
	adaptive_tesselation    = false;
	just_warped             = false;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
	recalc_pos();
}

/**********************************************************************************//**
 * \brief Constructor
 * \param x specifies the GLUT window coordinates where the camera view should be renedered
 * \param y specifies the GLUT window coordinates where the camera view should be renedered
 * \param w camera render area width
 * \param h camera render area height
 *************************************************************************************/
Camera::Camera(int x, int y, int w, int h) : MVPHandler(x,y,w,h) {
	r                       = 15;
	phi                     = M_PI_4;
	theta                   = M_PI_4;
	look_at_x               = 0;
	look_at_y               = 0;
	look_at_z               = 0;
	right_mouse_button_down = false;
	adaptive_tesselation    = false;
	just_warped             = false;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
	recalc_pos();
}

//! \brief Destructor
Camera::~Camera() {
	r     = 0;
	phi   = 0;
	theta = 0;
}

/**********************************************************************************//**
 * \brief get an estimate of the tesselation resolution based on camera length
 * \returns The number of tesselation points required in each parametric direction
 *
 * This function is a hand-waving estimate based solely on the length from the camera
 * position to the camera look-at-point. The number returned is 120/sqrt(r)
 *************************************************************************************/
int Camera::getTesselationRes() {
	return (int) (4.0*30/sqrt(r));
}

/**********************************************************************************//**
 * \brief rotate camera
 * \param d_r distance change from the the look-at-point (positive value for zooming in)
 * \param d_phi angle-change from the z-axis (positive tilts the camera down making it look upwards)
 * \param d_theta angle-change in the xy-plane (rotate around the object keeping z-height unchanged)
 *************************************************************************************/
void Camera::rotate(float d_r, float d_phi, float d_theta) {
	float prev_r = r;
	r     += d_r;
	phi   += (upside_down) ? -d_phi : d_phi;
	theta += (upside_down) ? -d_theta : d_theta;
	GLfloat epsilon = 1e-3;

	// if(phi==M_PI)
		// phi = M_PI-epsilon;
	if(phi>M_PI) {
		phi = M_PI-(phi-M_PI);
		theta += M_PI;
		upside_down = !upside_down;
	} else if(phi<0) {
		phi = -phi;
		theta += M_PI;
		upside_down = !upside_down;
	}

	if(theta>2*M_PI)
		theta -= 2*M_PI;
	else if(theta<0)
		theta += 2*M_PI;

	if(r < 0)
		r = epsilon;
	
	if(adaptive_tesselation && floor(prev_r) != floor(r))
		fireActionEvent(ACTION_REQUEST_RETESSELATE | ACTION_REQUEST_REPAINT);
		
	recalc_pos();
}

/**********************************************************************************//**
 * \brief pans the camera
 * \param d_u change in x-coordinate
 * \param d_v change in y-coordinate
 *************************************************************************************/
void Camera::pan(float d_u, float d_v) {
	
	GLfloat cam[] = {look_at_x-x, look_at_y-y, look_at_z-z};
	GLfloat pan_u[] = {0,0,0};
	GLfloat pan_v[] = {0,0,0};
	GLfloat length;

	// pan_u = cross_product(up, cam)
	pan_u[0] = (upside_down) ?  cam[1] : -cam[1];
	pan_u[1] = (upside_down) ? -cam[0] :  cam[0];

	// normalize the vector
	length = sqrt(pan_u[0]*pan_u[0] + pan_u[1]*pan_u[1]);
	pan_u[0] /= length;
	pan_u[1] /= length;

	// pan_v = cross_product(pan_v, cam)
	pan_v[0] = pan_u[1]*cam[2] - pan_u[2]*cam[1];
	pan_v[1] = pan_u[2]*cam[0] - pan_u[0]*cam[2];
	pan_v[2] = pan_u[0]*cam[1] - pan_u[1]*cam[0];

	// normalize the vector
	length = sqrt(pan_v[0]*pan_v[0] + pan_v[1]*pan_v[1] + pan_v[2]*pan_v[2]);
	pan_v[0] /= length;
	pan_v[1] /= length;
	pan_v[2] /= length;

	look_at_x += d_u*pan_u[0] + d_v*pan_v[0];
	look_at_y += d_u*pan_u[1] + d_v*pan_v[1];
	look_at_z += d_u*pan_u[2] + d_v*pan_v[2];

	recalc_pos();
}

/**********************************************************************************//**
 * \brief recalculate the x,y,z position based on the (r,theta,phi) position 
 *************************************************************************************/
void Camera::recalc_pos() {
	x = r*cos(theta)*sin(phi) + look_at_x;
	y = r*sin(theta)*sin(phi) + look_at_y;
	z = r*cos(phi) + look_at_z;
}

/**********************************************************************************//**
 * \brief DEPRECATED: used to update camera position in animations
 *************************************************************************************/
void Camera::update() {
	// rotate(speed_r, speed_phi, speed_theta);
	// pan(speed_pan_u, speed_pan_v);
}

//! \brief returns the distance between the view-to-point and the camera
GLfloat Camera::getR() {
	return r;
}

// void Camera::setViewport() {
	// glViewport(0, 0, vp_width, vp_height);
// }

//! \brief sets the GL_PROJECTION matrix based on camera parameters
void Camera::setProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)vp_width / (float)vp_height, 1.0, 700.0);
}

//! \brief sets the GL_MODELVIEW matrix based on camera parameters (plus lighting conditions)
void Camera::setModelView() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


	GLfloat lightColor[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat specularColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// GLfloat lightPos[] = {-7, 7, 28, 1};
	GLfloat lightPos[] = {2, -1, -4, 0}; // last value=0, directional light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	GLfloat lightColor2[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat specularColor2[] = {0.3f, 0.3f, 0.3f, 1.0f};
	GLfloat lightPos2[] = {-14, -7, -28, 1}; // last value=1, positional light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularColor2);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
	
	if(upside_down)
		gluLookAt(x,y,z,                            // camera pos
		          look_at_x, look_at_y, look_at_z,  // viewing pos
		          0,0,-1);                          // up vector
	else
		gluLookAt(x,y,z,                            // camera pos
		          look_at_x, look_at_y, look_at_z,  // viewing pos
		          0,0,1);                           // up vector

}

/**********************************************************************************//**
 * \brief Mouse action event (clicking)
 * \param button which mouse button was pressed or released
 * \param state button pressed or released
 * \param x position of the mouse at the time of use
 * \param y position of the mouse at the time of use
 *************************************************************************************/
void Camera::processMouse(int button, int state, int x, int y) {
	MouseListener::processMouse(button, state, x, y);

	if(button == GLUT_RIGHT_BUTTON) {
		if(state == GLUT_UP) {
			glutSetCursor(GLUT_CURSOR_INHERIT);
			fireActionEvent(ACTION_REQUEST_REPAINT | ACTION_REQUEST_REMASK);
		} 
	}
}

/**********************************************************************************//**
 * \brief Mouse action event (dragging)
 * \param x position of the mouse at the time of use
 * \param y position of the mouse at the time of use
 *************************************************************************************/
void Camera::processMouseActiveMotion(int x, int y) {
	if(right_mouse_button_down) {
		if(specialKey == GLUT_ACTIVE_CTRL) {
			// zooming
			glutSetCursor(GLUT_CURSOR_UP_DOWN);
			if(just_warped) {
				just_warped = false;
				y = last_mouse_y;
			}
			GLfloat speed_r = -speed_scale_zoom*((float) y-last_mouse_y);
			rotate(speed_r, 0, 0);
		} else if(specialKey == GLUT_ACTIVE_SHIFT) {
			// paning
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
			GLfloat speed_pan_u = r*speed_scale_pan*((float) x-last_mouse_x);
			GLfloat speed_pan_v = r*speed_scale_pan*((float) y-last_mouse_y);
			pan(speed_pan_u, speed_pan_v);
		} else { // no special-key (ALT is not an option strange enough)
			// yawing
			glutSetCursor(GLUT_CURSOR_CYCLE);
			GLfloat speed_theta = -2*speed_scale_rotate*((float) x-last_mouse_x);
			GLfloat speed_phi   =  2*speed_scale_rotate*((float) y-last_mouse_y);
			rotate(0, speed_phi, speed_theta);
		}
		fireActionEvent(ACTION_REQUEST_REPAINT);
	}
	MouseListener::processMouseActiveMotion(x,y);
}

/**********************************************************************************//**
 * \brief Mouse action event (moving)
 * \param x position of the mouse at the time of use
 * \param y position of the mouse at the time of use
 *************************************************************************************/
void Camera::processMousePassiveMotion(int x, int y) {
	MouseListener::processMousePassiveMotion(x,y);
}


/**********************************************************************************//**
 * \brief sets adaptive tesselation
 * if adaptive tesselation is turned on, then camera zooming will trigger events where
 * the distance between the camera and look-at-point changes enough to request a
 * retesselation of the splines
 *************************************************************************************/
void Camera::setAdaptiveTesselation() {
	adaptive_tesselation = true;
}

//! \brief draws the bacground colors 
void Camera::paintBackground() {
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_QUADS);
		glColor3f(.3, .3, .3);
		glVertex2f(-1, -1);
		glVertex2f( 1, -1);
		glColor3f(.9, .9, .9);
		glVertex2f( 1, 1);
		glVertex2f(-1, 1);
	glEnd();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

//! \brief draws meta-information, i.e. the axis cross and the panel name "Perspective"
void Camera::paintMeta() {
	// draw axis cross
	GLfloat axis_size = r / 10.0;
	glLineWidth(3);
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(axis_size, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, axis_size, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, axis_size);
	glEnd();

	// draw panel name
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	MVPHandler::glutPrint(-1+12.0/vp_width, 1-30.0/vp_height, GLUT_BITMAP_HELVETICA_12, "Perspective", 0,0,0,0);
	glEnable(GL_DEPTH_TEST);
}

