#ifndef HSV_H
#define HSV_H


#define RETURN_HSV(h, s, v) {HSV.H = h; HSV.S = s; HSV.V = v; return HSV;}
#define RETURN_RGB(r, g, b) {RGB.R = r; RGB.G = g; RGB.B = b; return RGB;}

#define CENTRAL_X 300.0
#define CENTRAL_Y 100.0
#define WHEEL_RADIUS 80


typedef struct {float R, G, B;} RGBType;
typedef struct {float H, S, V;} HSVType; 

RGBType HSV_to_RGB( HSVType HSV );

void drawColorWheel(void);

void drawTriangle(float radius);

void wheelDetection(float x1,float y1,float central_x,float central_y,float radius_ex,float radius_in);

void triangleDetection(float x1,float y1,float central_x,float central_y,float radius);

void drawLittleTriangle(float x, float y);

void drawClickH(void);
void drawClickSV(void);

static void drawDot(int x, int y);
void drawCircle(int center_x, int center_y, int radius_x, int radius_y);

#endif
