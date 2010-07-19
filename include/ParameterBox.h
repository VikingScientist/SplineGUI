
#ifndef _PARAMETERBOX_H
#define _PARAMETERBOX_H

#include <GL/glut.h>
#include "myMatrix.h"
#include <time.h>
#include <sys/time.h>

class ParameterBox {

	public:
		ParameterBox();

		void resize(int w, int h);
		void handleKeypress(unsigned char key, int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		void paint();
		int getWidth()  const { return width; }
		int getHeight() const { return width; }
		void getConstParam(int *face, double *value);

		bool changes;

	private:
		int width;
		int height;
		bool calculated_inv;
		bool plane_choice;
		bool show_all_edges;
		bool left_mousebutton_held;
		enum direction {NONE, X, Y, Z} plane;
		double mouse_pos[3];
		FullMatrix inv_modelMatrix;

		int double_click_threshold; // in nanoseconds
		struct timeval last_click;
		
		void initRendering();
		void one_inv_modelview_calculation();
		bool was_double_click();

};
#endif
