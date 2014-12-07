
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
		virtual ~VolumeDisplay();
		virtual void tesselate(int *n=NULL);
		virtual void paint();
		virtual void paintSelected();
		virtual void paintMouseAreas();
		virtual void readMouseAreas();
		virtual bool isAtPosition(int x, int y);
		virtual SurfaceDisplay* getSurfaceAt(int x, int y);
		virtual void processMouse(int button, int state, int x, int y);
		virtual void processMouseActiveMotion(int x, int y);
		virtual void processMousePassiveMotion(int x, int y);
		virtual DISPLAY_CLASS_TYPE classType() { return VOLUME; }
		virtual void printDebugInfo();
		virtual void print(std::ostream *out);
		virtual void setDrawControlMesh(bool draw); void setColorByParameterValues(bool draw);
		virtual void setSelectedColor(double r, double g, double b);
		virtual void setColor(double r, double g, double b);
		virtual void getBoundingBox(Go::BoundingBox &box) const ;

		virtual void initMouseMasks() ;
		virtual void setMaskPos(int x, int y, bool value) ;
		virtual void paintMouseAreas(float r, float g, float b) ;
	
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
		GLuint   *cp_lines;
		GLdouble *positions;
		GLdouble *normals;
		GLdouble *param_values;
		GLuint   *triangle_strip;
		GLfloat  *wallbuffer;
		std::vector<SurfaceDisplay*> walls;
};

#endif
