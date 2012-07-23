#ifndef _CAMERA_H
#define _CAMERA_H

#include "MVPHandler.h"

class Camera : public MVPHandler {

	public:
		// note: (phi, theta) defined as NTNU Matte 2, NOT wikipedia (which swithces the two)
		Camera();
		Camera(int x, int y, int w, int h);
		~Camera();
		void rotate(float d_r, float d_phi, float d_theta);
		void pan(float d_u, float d_v);
		void update();
		// void setViewport();
		void setModelView();
		void setProjection();
		void handleResize(int x, int y, int w, int h);
		GLfloat getR();
		virtual void processMouse(int button, int state, int x, int y);
		virtual void processMouseActiveMotion(int x, int y);
		virtual void processMousePassiveMotion(int x, int y);

		virtual void viewBoundingBox(Go::BoundingBox &box) ;

		void setAdaptiveTesselation();
		void paintBackground();
		void paintMeta();
		int getTesselationRes();


	private:
		void recalc_pos();

		GLfloat speed_pan_u;
		GLfloat speed_pan_v;

		GLfloat r;
		GLfloat phi;
		GLfloat theta;

		GLfloat look_at_x;
		GLfloat look_at_y;
		GLfloat look_at_z;
		
		GLfloat x;
		GLfloat y;
		GLfloat z;

		double size;
		bool upside_down;
		bool adaptive_tesselation;
};

#endif

