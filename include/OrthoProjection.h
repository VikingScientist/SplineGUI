#ifndef _ORTHOPROJECTION_H
#define _ORTHOPROJECTION_H

#include "MVPHandler.h"

enum viewplane {TOP, FRONT, LEFT};

class OrthoProjection : public MVPHandler {

	private:
		bool right_mouse_button_down;
		bool just_warped;
		int specialKey;
		int last_mouse_x;
		int last_mouse_y;
		int vp_width, vp_height; // viewport width/height measured in pixels
		double u_w, v_h; // viewport measured in object coordinates
		double u0, v0; // lower left origin measured in object coordinates
		enum viewplane view;

	public:

		OrthoProjection();
		OrthoProjection(enum viewplane view);
		OrthoProjection(int x, int y, int w, int h);
		~OrthoProjection();
		
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);

		void uvAt(int x, int y, double &u, double &v);
		void setModelView();
		void setProjection();
		void handleResize(int x, int y, int w, int h) ;
		void paintBackground();
		void paintMeta();
};

#endif

