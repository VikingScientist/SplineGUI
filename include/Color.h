
#ifndef _COLOR_STUFF_H
#define _COLOR_STUFF_H

// copy-paste some color-manipulation tools from a random webpage
#define RETURN_HSV(h, s, v) {HSV.H = h; HSV.S = s; HSV.V = v; return HSV;}
#define RETURN_RGB(r, g, b) {RGB.R = r; RGB.G = g; RGB.B = b; return RGB;}
#define UNDEFINED -1 

typedef struct {float R, G, B;} RGBType;
typedef struct {float H, S, V;} HSVType; 

HSVType RGB_to_HSV( RGBType RGB ) ;

RGBType HSV_to_RGB( HSVType HSV ) ;

#endif

