#ifndef INPUTMODULE_H
#define INPUTMODULE_H

#include <stdlib.h>
#include <GL/glut.h>

extern GLfloat current_pos[];

extern "C" {
#define IMAGE_WIDTH  800
#define IMAGE_HEIGHT 600

typedef struct{
	float fieldOfView;
	float aspect;
	float nearPlane;
	float farPlane;
}perspectiveData;

typedef struct{
	HSVType hsv;
	int x;
}LittleTriangle;
}
  
void readKeyboard(unsigned char key, int x, int y);
void mouseButtHandler(int button, int state, int x, int y);
void mouseMoveHandler(int x, int y);
void setUserView();
void special_callback(int key, int x, int y);

#endif









