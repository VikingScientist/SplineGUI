
#ifndef _VOLUME_DISPLAY_H
#define _VOLUME_DISPLAY_H

#include "DisplayObject.h"
#include <GoTools/trivariate/SplineVolume.h>
#include <GoTools/utils/BoundingBox.h>
#include <GL/glut.h>
#include <vector>
class CurveDisplay;
class SurfaceDisplay;


class VolumeDisplay : public DisplayObject {
	
	public:
		Go::SplineVolume *volume;

		VolumeDisplay(Go::SplineVolume *volume);
		~VolumeDisplay();
		void tesselate(int *n=NULL);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		bool isAtPosition(int x, int y);
		SurfaceDisplay* getSurfaceAt(int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		DISPLAY_CLASS_TYPE classType() { return VOLUME; }
		void printDebugInfo();
		void print(std::ostream *out);
		void setDrawControlMesh(bool draw); void setColorByParameterValues(bool draw);
		void setSelectedColor(double r, double g, double b);
		void setColor(double r, double g, double b);
		void getBoundingBox(Go::BoundingBox &box) const ;

		void addDisplacement(Go::SplineVolume *disp);
		void addColor(       Go::SplineVolume *col, double min, double max) ;

		void initMouseMasks() ;
		void setMaskPos(int x, int y, bool value) ;
		void paintMouseAreas(float r, float g, float b) ;
	
	private:
		
		Go::SplineVolume *colors;
		Go::SplineVolume *displacement;
		double cMax; // color max value
		double cMin;

		bool draw_contol_mesh;
		bool colorByParametervalues;
		int resolution[3];
		int triangle_count;
		int line_count;
		int cp_count;
		int width;
		int height;
		GLdouble *cp_pos;
		GLuint   *cp_lines;
		GLdouble *positions;
		GLdouble *normals;
		GLdouble *param_values;
		GLuint   *triangle_strip;
		GLfloat  *wallbuffer;
		std::vector<SurfaceDisplay*> walls;
};

#endif
