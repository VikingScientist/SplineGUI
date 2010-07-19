#include "OrthoProjection.h"
#include <GL/glut.h>

OrthoProjection::OrthoProjection() {
	just_warped             = false;
	right_mouse_button_down = false;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
	specialKey              = 0;
	view                    = TOP;
	u0                      = 0;
	v0                      = 0;
	u_w                     = 20;
	v_h                     = 20;
	vp_width                = 1;
	vp_height               = 1;
}

OrthoProjection::OrthoProjection(enum viewplane view) {
	just_warped             = false;
	right_mouse_button_down = false;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
	specialKey              = 0;
	this->view              = view;
	u0                      = 0;
	v0                      = 0;
	u_w                     = 20;
	v_h                     = 20;
	vp_width                = 1;
	vp_height               = 1;
}

OrthoProjection::OrthoProjection(int x, int y, int w, int h) : MVPHandler(x,y,w,h) {
	just_warped             = false;
	right_mouse_button_down = false;
	last_mouse_x            = 0;
	last_mouse_y            = 0;
	specialKey              = 0;
	view                    = TOP;
	u0                      = 0;
	v0                      = 0;
	u_w                     = 20;
	v_h                     = 20;
	vp_width                = 1;
	vp_height               = 1;
}

OrthoProjection::~OrthoProjection() {
}


void OrthoProjection::processMouse(int button, int state, int x, int y) {
/*
	if(button == GLUT_DOWN) {
		double u, v;
		uvAt(x,y,u,v);
		printf("Mouseclick at (u,v) = (%.3f, %.3f)\n", u,v);
	}
*/
	specialKey = glutGetModifiers();
	if(button == GLUT_RIGHT_BUTTON) {
		if(state == GLUT_UP) {
			right_mouse_button_down = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
			fireActionEvent(ACTION_REQUEST_REPAINT | ACTION_REQUEST_REMASK);
		} else {
			right_mouse_button_down = true;
		}
	}
	last_mouse_x = x;
	last_mouse_y = y;
}

void OrthoProjection::processMouseActiveMotion(int x, int y) {
	if(right_mouse_button_down) {
		if(specialKey == GLUT_ACTIVE_CTRL) {
			// zooming
			glutSetCursor(GLUT_CURSOR_UP_DOWN);
			if(just_warped) {
				just_warped = false;
				last_mouse_y = y;
			}
			double zoom = 0.054*(y-last_mouse_y);
			double scale = v_h/u_w;
			u_w -= zoom;
			v_h -= zoom*scale;
			if(u_w<0) {
				u_w = 1e-4;
				v_h = u_w*scale;
			}
			if(y > vp_height) {
				int window_height = glutGet(GLUT_WINDOW_HEIGHT);
				y = MouseListener::getY();
				just_warped = true;
				glutWarpPointer(x+MouseListener::getX(), window_height - y);
			} else if(y < 0) {
				int window_height = glutGet(GLUT_WINDOW_HEIGHT);
				y = MouseListener::getY() + MouseListener::getHeight();
				just_warped = true;
				glutWarpPointer(x+MouseListener::getX(), window_height - y);
			}
		} else if(specialKey == GLUT_ACTIVE_SHIFT) {
			// paning
			glutSetCursor(GLUT_CURSOR_CROSSHAIR);
			double du = (x-last_mouse_x)*u_w/vp_width;
			double dv = (y-last_mouse_y)*v_h/vp_height;
			u0 -= du;
			v0 -= dv;
		} else {
			glutSetCursor(GLUT_CURSOR_DESTROY);
		}
		fireActionEvent(ACTION_REQUEST_REPAINT);
	}
	last_mouse_x = x;
	last_mouse_y = y;
}

void OrthoProjection::processMousePassiveMotion(int x, int y) {
}


void OrthoProjection::setModelView() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat lightColor[] = {0.8f, 0.8f, 0.8f, 1.0f};
	GLfloat specularColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPos[] = {-14, 7, 28, 1};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	GLfloat lightColor2[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat specularColor2[] = {0.3f, 0.3f, 0.3f, 1.0f};
	GLfloat lightPos2[] = {-14, -7, -28, 1};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularColor2);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);


	glScaled(2/u_w, 2/v_h, -1.0/10); // negative z to invert back the perspective projection
	glTranslatef(-u0, -v0, 0);
	if(view == LEFT) {
		glRotatef(180, 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
	} else if(view == FRONT) {
		glRotatef(-90, 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
	} else { // view == TOP
		// glRotatef(180, 1,0,0);
	}
}

void OrthoProjection::setProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void OrthoProjection::handleResize(int x, int y, int w, int h)  {
	MVPHandler::handleResize(x, y, w, h);
	vp_width = w;
	vp_height = h;
	double scale = (w>h) ? ((double)h)/w : ((double)w)/h;
	double max = (u_w > v_h) ? u_w : v_h;
	if(w>h) {
		u_w = max;
		v_h = max*scale;
	} else {
		u_w = max*scale;
		v_h = max;
	}
}

void OrthoProjection::paintBackground() {
	glClearColor(.5, .5, .5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OrthoProjection::uvAt(int x, int y, double &u, double &v) {
	u = (u_w/vp_width)*x  - u_w/2 + u0;
	v = (v_h/vp_height)*y - v_h/2 + v0;
}

void OrthoProjection::paintMeta() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	string name = "";
	if(view == TOP)
		name = "Top";
	else if(view == LEFT)
		name = "Left";
	else if(view == FRONT)
		name = "Front";
	MVPHandler::glutPrint(-1+12.0/vp_width, 1-30.0/vp_height, GLUT_BITMAP_HELVETICA_12, name, 0,0,0,0);
	glEnable(GL_DEPTH_TEST);
}

