#include <vector>

using namespace std;

struct Color {
	double r,g,b;
};

class Colorbar {

	public:
		// double value[256]; // we might decide to include this for easy access and use
		Colorbar();
		Colorbar(double min, double max);
		Color get(double value);
		void paint();

	private:
		double min;
		double max;

		vector<double> render_quads;
		vector<double> color_quads;
		vector<char*> text_values;
		void setup_rendering_vertexes();
		/* The available fonts for the below method
		 *	GLUT_BITMAP_9_BY_15,
		 *	GLUT_BITMAP_8_BY_13,
		 *	GLUT_BITMAP_TIMES_ROMAN_10,
		 *	GLUT_BITMAP_TIMES_ROMAN_24,
		 *	GLUT_BITMAP_HELVETICA_10,
		 *	GLUT_BITMAP_HELVETICA_12,
		 *	GLUT_BITMAP_HELVETICA_18
		 */
		Color hsv2rgb(double h, double s, double v);
		void glutPrint(float x, float y, void* font, char* text, float r, float g, float b, float a);
};
