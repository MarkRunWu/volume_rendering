#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include "hsv.h"
#include "../geometry.h"
#include "../inputModule.h"
#include <list>
float clickH = 0.0;
float clickS = 1.0;
float clickV = 1.0;

int clickSV_x = CENTRAL_X;
int clickSV_y = CENTRAL_Y;


extern std::list<LittleTriangle> myList;
// ! Draw Filled Circle to bitmap or passed bitmap
// H is given on [0, 6] or UNDEFINED. S and V are given on [0, 1].
// RGB are each returned on [0, 1].
RGBType HSV_to_RGB( HSVType HSV ) 
{
	float h = HSV.H /60, s = HSV.S, v = HSV.V, m, n, f;
	int i;
	RGBType RGB;
	
	if (h == -1) RETURN_RGB(v, v, v);  // -1 means UNDEFINED
	i = floor(h);
	f = h - i;
	if ( !(i&1) ) f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
		case 6:
		case 0: RETURN_RGB(v, n, m);
		case 1: RETURN_RGB(n, v, m);
		case 2: RETURN_RGB(m, v, n)
		case 3: RETURN_RGB(m, n, v);
		case 4: RETURN_RGB(n, m, v);
		case 5: RETURN_RGB(v, m, n);
	}

} 

void drawColorWheel(void) // , CENTRAL_Y, WHEEL_RADIUS
{
	int wheel_thick = 20;
	
	// HSV color wheel
	double y1=CENTRAL_Y;
	double x1=CENTRAL_X;
	double x2, y2;
	double angle;

	glBegin(GL_TRIANGLES);  
	for(int i=0;i<=360;i++)
	{
		HSVType hsv;
		hsv.H=(float)i; // H is given on [0, 6] or UNDEFINED (-1)
		hsv.S=1.0;
		hsv.V=1.0;
		RGBType rgb = HSV_to_RGB(hsv);
		glColor4f(rgb.R,rgb.G,rgb.B,1);

		angle = i/57.29577957795135; // * 180.0 / 3.14
		//angle = (i*180.0)/M_PI;
		x2 = CENTRAL_X+(WHEEL_RADIUS*cos(angle));
		y2 = CENTRAL_Y+(WHEEL_RADIUS*sin(angle));
		glVertex2d(CENTRAL_X,CENTRAL_Y);   // circle center
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
		y1=y2;
		x1=x2;
	}
	glEnd();

	// black circle
	y1=CENTRAL_Y;
	x1=CENTRAL_X;
	glColor4f(0,0,0,1);
	glBegin(GL_POLYGON);
	for(int i=0;i<=360;i++)
	{
		angle = i*180.0/M_PI;
		x2 = CENTRAL_X+((WHEEL_RADIUS-wheel_thick)*sin(angle));
		y2 = CENTRAL_Y+((WHEEL_RADIUS-wheel_thick)*cos(angle));             
		glVertex2d(x2,y2);
	}
	glEnd();

	// triangle
    drawTriangle(WHEEL_RADIUS - wheel_thick); // make it smaller
}
/*
void drawTriangle( LittleTriangle t ){
	RGBType rgb = HSV_to_RGB (t.hsv );
	glColor3f( rgb.R , rgb.G , rgb.B );

	glBegin( GL_TRIANGLES );
	glVertex2i( 100 + t.x , 300 );
	glVertex2i( 100 + t.x - 10 , 285 );
	glVertex2i( 100 + t.x + 10 , 285 );
	glEnd();
}
*/


void drawTriangle(GLfloat radius)
{
   HSVType hsv;
   hsv.H=(float)clickH;
   hsv.S=0.0;
   hsv.V=1.0;

   RGBType a = HSV_to_RGB(hsv);
   hsv.S=0.0;
   hsv.V=0.0;
   RGBType b = HSV_to_RGB(hsv);
   hsv.S=1.0;
   hsv.V=1.0;
   RGBType c = HSV_to_RGB(hsv);


   float x, y;

   glBegin (GL_TRIANGLES);
     glColor3f (a.R, a.G, a.B);
     x=CENTRAL_X+(radius*(float)sin(240.0/57.29577957795135));
     y=CENTRAL_Y+(radius*(float)cos(240.0/57.29577957795135));
     glVertex2f (x, y);

     glColor3f (b.R, b.G, b.B);
     x=CENTRAL_X+(radius*(float)sin(120.0/57.29577957795135));
     y=CENTRAL_Y+(radius*(float)cos(120.0/57.29577957795135));
     glVertex2f (x, y);

     glColor3f (c.R, c.G, c.B);
     x=CENTRAL_X+(radius*(float)sin(0.0));
     y=CENTRAL_Y+(radius*(float)cos(0.0));
     glVertex2f (x, y);
   glEnd();
}

void drawLittleTriangle(float x, float y)
{
	HSVType hsv;
	hsv.H=(float)clickH;
	hsv.S=clickS;
	hsv.V=clickV;
	RGBType rgb = HSV_to_RGB(hsv);

	glColor3f(rgb.R,rgb.G,rgb.B);
	glBegin(GL_TRIANGLES);
		glVertex2i(x,y);
		glVertex2i(x-10,y-15);
		glVertex2i(x+10,y-15);
	glEnd();
}

void drawClickH(void)
{
	float radius = WHEEL_RADIUS-10; // raduis of wheel middle line

	// center of circle
	float x = CENTRAL_X+(radius*cos(clickH/180.0*M_PI)); // clickH = (int)angle;
	float y = CENTRAL_Y+(radius*sin(clickH/180.0*M_PI));

	// a point on circle
	float x2 = CENTRAL_X+((radius+5)*cos(clickH/180.0*M_PI)); 
	float y2 = CENTRAL_Y+((radius+5)*sin(clickH/180.0*M_PI));

	glColor3f(0, 0, 0);
	drawCircle(x, y, x2, y2);
}

void drawClickSV(void)
{
	glColor3f(0, 0, 0);
	drawCircle(clickSV_x, clickSV_y, clickSV_x+5, clickSV_y);
}

void wheelDetection(float x1,float y1,float central_x,float central_y,float radius_ex,float radius_in)
{
	float distance = sqrt(pow((float)(x1-central_x),(float)2.0)+pow((float)(y1-central_y),(float)2.0));

	if(distance>=radius_in&&distance<=radius_ex)
	{
			double angle = atan2( (y1-central_y),(x1-central_x))*180/M_PI;
			angle += 360;
			angle = ((int)angle) % 360;
			printf("angle = %f\n",angle);
			clickH = (int)angle;
	}
}

//  c
// a b
void triangleDetection(float x1,float y1,float central_x,float central_y,float radius)
{
	float a_x=central_x+(radius*(float)sin(240.0/57.29577957795135));
	float a_y=central_y+(radius*(float)cos(240.0/57.29577957795135));
	float b_x=central_x+(radius*(float)sin(120.0/57.29577957795135));
	float b_y=central_y+(radius*(float)cos(120.0/57.29577957795135));
	float c_x=central_x+(radius*(float)sin(0.0));
	float c_y=central_y+(radius*(float)cos(0.0));
	double triangle_y;
	if((x1-a_x)<(c_x-a_x))
		triangle_y = a_y+(x1-a_x)*1.732050807568877;
	else
		triangle_y = a_y+(b_x-x1)*1.732050807568877;
	//slope  = sqrt(3);
	if( x1>=a_x && x1<=b_x && y1>=a_y && y1<= triangle_y) 
	{
		printf("in triangle\n");
		clickS = (((y1-a_y)/1.732050807568877)*2)/(b_x-a_x);
		clickV = (b_x-(x1-((y1-a_y)/1.732050807568877)))/(b_x-a_x);

		clickSV_x = x1;
        clickSV_y = y1;
	}

	printf("hsv = (%f,%f,%f)\n", clickH, clickS, clickV);
}

static void drawDot(int x, int y) 
{
  glBegin(GL_POINTS);
    glVertex2i(x, y);  // invert y since opengl's origin is at top-left
  glEnd();
}

void drawCircle(int center_x, int center_y, int radius_x, int radius_y)
{
	double R; // R is radius 
	double d; // d is the decision variable 
	int incE, incSE;
	int current_x, current_y;

	R = sqrt( pow( (double)(radius_x - center_x), 2) + pow((double)(radius_y - center_y) , 2));
	d = 1 - R;
	incE = 3;
	incSE = (int) (-2 * R + 5);

	current_x = 0;
	current_y = (int) (R);

	while (current_x <= current_y){
		if (d < 0){ // E
			d += incE;
			current_x++;
			incE += 2;
			incSE += 2;
		}
		else{ // SE
			d += incSE;
			current_x++;
			current_y--;
			incE += 2;
			incSE += 4;
		}
		//cout << current_x << " " << current_y << endl;
		drawDot( (center_x + current_x), (int) (center_y + current_y));
		drawDot( (center_x + current_x), (int) (center_y - current_y)); // mirror
		drawDot( (center_x - current_x), (int) (center_y + current_y)); // mirror
		drawDot( (center_x - current_x), (int) (center_y - current_y)); // mirror
		drawDot((int)  (center_x + current_y), (center_y + current_x)); // mirror
		drawDot((int)  (center_x + current_y), (center_y - current_x)); // mirror
		drawDot((int)  (center_x - current_y), (center_y + current_x)); // mirror
		drawDot((int)  (center_x - current_y), (center_y - current_x)); // mirror
	}
    drawDot(center_x + (int) R, center_y); // the up point to fill the blank pixel
    drawDot(center_x - (int) R, center_y); // the up point to fill the blank pixel
	drawDot(center_x, center_y + (int) R); // the up point to fill the blank pixel
	drawDot(center_x, center_y - (int) R); // the button point 
}