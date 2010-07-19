
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include "Colorbar.h"

using namespace std;

Colorbar::Colorbar() {
	min = 0;
	max = 1;
	setup_rendering_vertexes();
}

Colorbar::Colorbar(double min, double max) {
	this->min = min;
	this->max = max;
	setup_rendering_vertexes();
}

void Colorbar::setup_rendering_vertexes() {
	int n = 50;
	int n_text = 6;
	double border = 0.3;
	double x[] = {.75, .85};
	double y, value;
	Color c;
	for(int i=0; i<n; i++) {
		// x = 0.6;
		y = -1.0 + border + (2-2*border)*i/(n-1);
		value = min + (max-min)*i/(n-1);
		c = get(value);
		render_quads.push_back(x[0]);
		render_quads.push_back(y);
		color_quads.push_back(c.r);
		color_quads.push_back(c.g);
		color_quads.push_back(c.b);

		render_quads.push_back(x[1]);
		render_quads.push_back(y);
		color_quads.push_back(c.r);
		color_quads.push_back(c.g);
		color_quads.push_back(c.b);
	}
	for(int i=0; i<n_text; i++) {
		text_values.push_back((char*) malloc(10*sizeof(char)));
		sprintf(text_values[text_values.size()-1], "%.3g", min + (max-min)*i/(n_text-1));
	}
}

Color Colorbar::get(double value) {
	double sat = 1.0;
	double val = 0.8;
	double hue = (1 - (value-min) / (max-min))*2/3;
	return hsv2rgb(hue, sat, val);
}

Color Colorbar::hsv2rgb(double h, double s, double v) {
	int k;
	float aa, bb, cc, f;
	double r, g, b;
	if (s <= 0.0)
		r = g = b = v;
	else {
		if(h==1)
			h = 0;
		h *= 6;
		k = floor(h);
		f = h -k ;
		aa = v*(1-s);
		bb = v*(1-(s*f));
		cc = v*(1-(s*(1-f)));
		switch(k) {
			case 0: r=v;  g=cc; b=aa; break;
			case 1: r=bb; g=v;  b=aa; break;
			case 2: r=aa; g=v;  b=cc; break;
			case 3: r=aa; g=bb; b=v;  break;
			case 4: r=cc; g=aa; b=v;  break;
			case 5: r=v;  g=aa; b=bb; break;
		}
	}
	Color ret_val;
	ret_val.r = r;
	ret_val.g = g;
	ret_val.b = b;
	return ret_val;
}

void Colorbar::glutPrint(float x, float y, void* font, char* text, float r, float g, float b, float a)
{
	if(!text || !strlen(text)) return;
	bool blending = false;
	if(glIsEnabled(GL_BLEND)) blending = true;
	glEnable(GL_BLEND);
	glColor4f(r,g,b,a);
	glRasterPos2f(x,y);
	while (*text) {
		glutBitmapCharacter(font, *text);
		text++;
	}
	if(!blending) glDisable(GL_BLEND);
}  

void Colorbar::paint() {
	// disable and set to default
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// setup needed things
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(2, GL_DOUBLE, 0, &(render_quads.at(0)));
	glColorPointer(3, GL_DOUBLE, 0, &(color_quads.at(0)));
	glDrawArrays(GL_QUAD_STRIP, 0, render_quads.size()/2);

	// draw frame
	glColor3f(0,0,0);
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
		glVertex2f(render_quads[0], render_quads[1]);
		glVertex2f(render_quads[2], render_quads[3]);
		glVertex2f(render_quads[render_quads.size()-2], render_quads[render_quads.size()-1]);
		glVertex2f(render_quads[render_quads.size()-4], render_quads[render_quads.size()-3]);
		glVertex2f(render_quads[0], render_quads[1]);
	glEnd();

	// draw values
	for(unsigned int i=0; i<text_values.size(); i++) {
		double x = .88;
		double y = -.7 + 1.4*i/(text_values.size()-1) - .03;
		glutPrint(x, y, GLUT_BITMAP_HELVETICA_10, text_values[i], 0, 0, 0, 1);
	}

	// re-enable stuff
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}
