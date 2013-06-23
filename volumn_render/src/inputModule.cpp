/*
** Student ID number: // fill_me
*/


#include <stdio.h>
#include <signal.h>
#include "hsv\hsv.h"
#include "inputModule.h"
#include "PLY.h"
#ifdef  WIN32
#include <afxsock.h>
#include <direct.h>
#endif
extern bool b_show_box;
extern bool b_log10;
/* bat ¼Ò¦¡ */
extern bool b_batMode;
extern int id_file;
extern void updateRGBA();
extern float rgba[256][4];
extern LittleTriangle* pActiveTriangle;
extern PLYObject *ply;
extern int loadPrefernceColorTranslationFunc();
extern int savePrefernceColorTranslationFunc();
extern void saveOpacityTF();
extern void initData2(char const* filename);
extern void guass_diffusion();
extern void updateWindows();
static int motionMode;
static int startX;
static int startY;
static GLfloat angle = 20;    /* in degrees */
static GLfloat angle2 = 30;   /* in degrees */

GLfloat current_pos[] = {0.0, 0.0, 5.0};
GLfloat current_pos_bunny[] = {0.0, 0.0, 0.0};
GLfloat current_rot_bunny[] = {0.0, 0.0, 0.0};
GLfloat current_sca_bunny[] = {1.0, 1.0, 1.0};
int isPressedB = 0;
int record = 0;
int box = 0;
int wire = 1;
int light = 0;
int flat = 1;
int texture = 0;
int ambient=1, diffuse=1, specular=1;
float slipAngle = 0;
extern int slice_mode;



int OpenFileByWinAPI( wchar_t* filebuffer , int filebufferlength){
	wchar_t stringFilter[] = L"(*.hdr)\0*.hdr\0";
	wchar_t strTitle[80] =L"Select Volumn Data";
	wchar_t fileTitle[100] = {};
	int buffersize;
	OPENFILENAME fileinfo;

	ZeroMemory(&fileinfo, sizeof(fileinfo));
	buffersize=sizeof(OPENFILENAME);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filebuffer;
    ofn.nMaxFile = sizeof(wchar_t) * filebufferlength;
    ofn.lpstrFilter = stringFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = fileTitle;
    ofn.nMaxFileTitle = 80;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_SHAREAWARE;
	if( GetOpenFileName(&ofn)){
		return 1;
	}
	return 0;
}

int feature = 0;
void readKeyboard(unsigned char key, int x, int y)
{
  switch(key){
  //case '+':
	 // printf( "+" );
	 // feature = min(feature + 1 , 255 );

	 // for( int i = 0 ; i != 256 ; i++ ){
		//  if( i == feature )
		//	  rgba[i][3] = 255;
		//  else rgba[i][3] = 0;
	 // }
	 // updateRGBA();
	 // break;
  //case '-':
	 // printf( "-" );
	 // feature = max(feature - 1 , 0 );
	 // 
  //	  for( int i = 0 ; i != 256 ; i++ ){
		//  if( i == feature )
		//	  rgba[i][3] = 255;
		//  else rgba[i][3] = 0;
	 // }
	 // updateRGBA();
	 // break;
 // case  0x1B:
 // case  'q':
 // case  'Q':
 //   delete(ply);
 //   exit(0);
 //   break;
 // case 'i':
 // case 'I':
 //   if (ply)
 //     ply->invertNormals();
 //   break;
  case 'v':
  case 'V':
    record = (record + 1) % 2; //---
	id_file = 1;
 //   break;
	///*
 // case 'b':
 // case 'B':
 //   box = (box + 1) % 2;
 //   break;
	//*/
 // case 'w':
 // case 'W':
 //   wire = (wire + 1) % 2;
 //   break;
  case 'h':
  case 'H':
    //printf("\tpress q/Q for quit\n");
    //printf("\tpress i/I to invert normals\n");
    printf("\tpress b/B to display bounding box\n");
    //printf("\tpress w/W to toggle wire frame rendering\n");
    printf("\tpress l/L to enable log value\n");
	printf("\tpress v/V to record\n"); // scale
	printf("\tpress s/S to save TF color (default.tfe)\n");
	printf("\tpress a/A to load TF color (default.tfe)\n");
	printf("\tpress p/P to show an openfiledialog\n"); // rotate
    break;
 // // scale ------------------------------------
 // case 'z':
 // case 'Z':
 //   slice_mode ^= 1;
 //   break;
 // case 'x':
 // case 'X':
 //   // fill_me...
	//guass_diffusion();
	//break;
 // case 'c':
 //   // fill_me...
	//break;
 // case 'C':
 //   // fill_me...
 //   break;
  // rotate ------------------------------------
  case 'a':
  case 'A':
    if( loadPrefernceColorTranslationFunc() ){
		pActiveTriangle = NULL;
	}
	
    break;
  case 's':
  case 'S':
    // fill_me...
	 savePrefernceColorTranslationFunc();
	 //saveOpacityTF();
    break;
 // case 'd':
 //   // fill_me...
	//break;
 // case 'D':
 //   // fill_me...
 //   break;
 // // -------------------------------------------
 // case 'e':
 // case 'E':
 //   // reset initial view parameters
 //   angle = 20;
 //   angle2 = 30;
 //   current_pos[0] = 0.0;
 //   current_pos[1] = 0.0;
 //   current_pos[2] = 5.0;
	//// reset rendering parameters
	//flat = wire = 1;
	//record = box = light = texture = 0;
	//ambient = diffuse = specular = 1;
 //   break;
 // case 'l':case'L':
	//light ^= 1;
	//break;
 case 'b':case'B':
	 b_show_box ^= 1;
	 break;
	// isPressedB ^= 1;
	// break;
  case 'l':case'L':
	  b_log10 ^= 1;
	  updateWindows();
	  break;
  case 'p':case'P':
	  wchar_t filename[100] = L"";
	  char charname[100];

	  if( OpenFileByWinAPI(filename , 100) ){
		  // covert wchar to char
		  for( int i = 0 ; i != 100 ; i++){
			  charname[i] = (char)filename[i];
		  }
		  //printf("%s\n" , charname);
		  initData2( charname );
	  }
  }
  glutPostRedisplay();
}

void mouseButtHandler(int button, int state, int x, int y)
{
  motionMode = 0;
  
  switch(button){
  case GLUT_LEFT_BUTTON:  
    if(state == GLUT_DOWN) {
      motionMode = 1;		// Rotate object
      startX = x;
      startY = y;
    }
    break;
  case GLUT_MIDDLE_BUTTON:  
    if(state == GLUT_DOWN) {
      motionMode = 2;		// Translate object
      startX = x;
      startY = y;
    }
    break;
  case GLUT_RIGHT_BUTTON: 
    if(state == GLUT_DOWN) { 
      motionMode = 3;		// Zoom
      startX = x;
      startY = y;
    }
    break;
  }
  glutPostRedisplay();
}

void special_callback(int key, int x, int y)
{
  switch (key) {
    case GLUT_KEY_DOWN:
      // fill_me...

	  printf("%f\n", current_pos_bunny[0]);
      break;
    case GLUT_KEY_UP:
      // fill_me...

	  printf("%f\n", current_pos_bunny[0]);
      break;
    case GLUT_KEY_LEFT:
      // fill_me...

	  printf("%f\n", current_pos_bunny[0]);
      break;
    case GLUT_KEY_RIGHT:
      // fill_me...

	  printf("%f\n", current_pos_bunny[0]);
      break;
  }
}

void mouseMoveHandler(int x, int y)
{
  // No mouse button is pressed... return 
  switch(motionMode){
  case 0:
    return;
    break;

  case 1: // Calculate the rotations
    angle = angle + (x - startX);
    angle2 = angle2 + (y - startY);
    startX = x;
    startY = y;
	slipAngle += 5;
	if( slipAngle >= 90 )slipAngle = 0;
    break;

  case 2:
    current_pos[0] = current_pos[0] - (x - startX)/100.0;
    current_pos[1] = current_pos[1] - (y - startY)/100.0;
    startX = x;
    startY = y;
    break;
    
  case 3:
    current_pos[2] = current_pos[2] - (y - startY)/10.0;
    startX = x;
    startY = y;
    break;
  }
  
  glutPostRedisplay();
}

void setUserView()
{
  glLoadIdentity();

  glTranslatef(-current_pos[0], current_pos[1], -current_pos[2]);
  glRotatef(angle2, 1.0, 0.0, 0.0);
  glRotatef(angle, 0.0, 1.0, 0.0);
}