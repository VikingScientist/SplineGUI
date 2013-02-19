
#ifndef _LR_VOLUME_DISPLAY_H
#define _LR_VOLUME_DISPLAY_H

#include "DisplayObject.h"

#include <LRSpline/LRSplineVolume.h>

#include <GoTools/utils/BoundingBox.h>
#include <GL/glut.h>
#include <vector>
class CurveDisplay;
class SurfaceDisplay;
class VolumeDisplay;


class LRVolDisplay : public DisplayObject {
	
	public:
		LR::LRSplineVolume *volume;

		LRVolDisplay(LR::LRSplineVolume *volume);
		~LRVolDisplay();
		virtual void tesselate(int *n=NULL);
		void paint();
		void paintSelected();
		void paintMouseAreas();
		void readMouseAreas();
		bool isAtPosition(int x, int y);
		SurfaceDisplay* getSurfaceAt(int x, int y);
		void processMouse(int button, int state, int x, int y);
		void processMouseActiveMotion(int x, int y);
		void processMousePassiveMotion(int x, int y);
		DISPLAY_CLASS_TYPE classType() { return LRVOLUME; }
		void printDebugInfo();
		void print(std::ostream *out);
		void setDrawControlMesh(bool draw);
		void setColorByParameterValues(bool draw);
		void setSelectedColor(double r, double g, double b);
		void setColor(double r, double g, double b);
		void getBoundingBox(Go::BoundingBox &box) const ;

		void initMouseMasks() ;
		void setMaskPos(int x, int y, bool value) ;
		void paintMouseAreas(float r, float g, float b) ;
	
	protected:

		bool draw_contol_mesh;
		bool colorByParametervalues;
		int resolution[3];
		int triangle_count;
		int line_count;
		int cp_count;
		int width;
		int height;
		GLdouble *cp_pos;
		GLdouble *positions;
		GLdouble *normals;
		GLdouble *param_values;
		GLuint   *triangle_strip;
		GLfloat  *wallbuffer;
		std::vector<VolumeDisplay*> bezierVols;
};

#endif
