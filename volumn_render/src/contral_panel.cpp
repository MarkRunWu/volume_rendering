#include "Resource.h"
#include "ColorTile.h"
#include <cmath>

using namespace std;
//window
extern int window;
//hsv
extern float clickH ;
extern float clickS ;
extern float clickV ;
//ply
extern float diffuse[4];

extern int histogram[256];
extern int path[256];

extern float rgba[256][4];
extern void updateRGBA();
extern list<LittleTriangle*> myList;

HSVType panel_color;
GLuint mouse_button;

ColorTile preferColor[PRFER_COLOR_TILE_NUM];

static int globalPost_x , globalPost_y; 

int panel_height = 900;
int panel_width  = 550;

LittleTriangle* pActiveTriangle = NULL ;
int mouse_move = 0;
int hit = 0;

int pressedG = 0;


extern void string_render(const char *string, GLint x, GLint y);

void fillColor(LittleTriangle* t1 , LittleTriangle* t2 ){
	RGBType rgb1 = HSV_to_RGB( t1->hsv );
	RGBType rgb2 = HSV_to_RGB( t2->hsv );

	float frac;

	for( int i = t1->x ; i <= t2->x ; i++ ){
		frac = ((float)( i - t1->x)) / (t2->x - t1->x);

		rgba[i][0] = (1.0 - frac) * rgb1.R + (frac) * rgb2.R;
		rgba[i][1] = (1.0 - frac) * rgb1.G + (frac) * rgb2.G;
		rgba[i][2] = (1.0 - frac) * rgb1.B + (frac) * rgb2.B;
		
		rgba[i][3] = path[i] / 255.0;
	}
}

bool compare_list( LittleTriangle* first , LittleTriangle* second ){
	if( first->x < second->x ){
		return true;
	}else{
		return false;
	}
}

inline int interpolationY( float x_post , float y_post , float x_last , float y_last , float x){
	return (int) ( (x - x_post )*(y_last - y_post ) / (x_last - x_post)  + y_post );
	
}

void drawHistogram(){
	static double outputs[256];
	for( int i = 0 ; i != 256 ; i++){
		if( pressedG ){
			outputs[i] = log10( (double) histogram[i]);
		}else{
			//outputs[i] = histogram[i] / 1000.0;
			outputs[i] = histogram[i];
		}
	}
	glColor3f(0.f , 0.f , 1.f );
	glBegin( GL_LINES );
		for( int i = 0 ; i != 256 ; i++){
			glVertex2f(HISTORGRAM_POSX + i , HISTORGRAM_POSY);
			glVertex2f(HISTORGRAM_POSX + i  , HISTORGRAM_POSY + outputs[i] );
		}
	glEnd();
	char num[256];
	for( int i = 0 ; i != 256 ; i++){
		if( i % 51  == 0 )
			string_render( itoa( i  + 1 , num , 10 ) , HISTORGRAM_POSX + i , HISTORGRAM_POSY - 10 );
	}
	glColor3f( 1 , 1 , 1);
	glBegin(GL_LINE_STRIP);
	for( int i = 0 ; i != 256 ; i++){
          glVertex2f(100 + i  , 500 + outputs[i] );
	}
	glEnd();
	glColor3f( 1 , 1 , 1);
	glBegin(GL_LINE_LOOP);
	glVertex2i(HISTORGRAM_POSX, HISTORGRAM_POSY);
	glVertex2i(HISTORGRAM_POSX + 512,HISTORGRAM_POSY);
	glVertex2i(HISTORGRAM_POSX + 512 ,HISTORGRAM_POSY + 99);
	glVertex2i(HISTORGRAM_POSX, HISTORGRAM_POSY + 99);
	glEnd();

}
void updateTransferFuctionPartView(int x_post , int y_post , int x_last , int y_last ){
	int tmp ;
	if( x_last < x_post ){
		tmp = x_last;
		x_last = x_post;
		x_post = tmp;
		tmp = y_last;
		y_last = y_post;
		y_post = tmp;
	}
	//printf("post: %d\t%d current: %d\t%d\n", x_post  , y_post  , x_last , y_last );
	for( int i = x_post ; i <= x_last; i++ ){
		path[i] = (int) ( (i - x_post )*(y_last - y_post ) / max( (x_last - x_post) , 1 )  + y_post );
		//printf( "%d\t" , path[i]);
	}
	list<LittleTriangle*>::iterator it , it2;

	it2 = it = myList.begin();
	it2++;
	for( int i =0 ; i < (myList.size() - 1) ; i++){
		fillColor( *it , *it2 );
		it++;
		it2++;
	}

	glutPostRedisplay();
}
void drawTransferFuctionView(){
	// triangle area  y(285 , 300)
	
	
	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_1D );
	glColor3ub( 255,255,255);
	glBegin( GL_QUADS);
	//glColor3f( rgba[i][0] , rgba[i][1] , rgba[i][2] );
	glTexCoord1i(0);
	glVertex2i( TRANSFERVIEW_POSX  , TRANSFERVIEW_POSY  );
	glTexCoord1i(0);
	glVertex2i( TRANSFERVIEW_POSX  , TRANSFERVIEW_POSY + 256 );
	glTexCoord1i(1);
	glVertex2i( TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH , TRANSFERVIEW_POSY + 256 );
	glTexCoord1i(1);
	glVertex2i( TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH , TRANSFERVIEW_POSY);
	glEnd();
	glDisable( GL_TEXTURE_1D );
	glBegin( GL_LINE_LOOP );
	glColor3ub( 0 ,0 ,255);
	glVertex2i( TRANSFERVIEW_POSX , TRANSFERVIEW_POSY );
	glVertex2i( TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH , TRANSFERVIEW_POSY );
	glVertex2i( TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH , TRANSFERVIEW_POSY + 256 );
	glVertex2i( TRANSFERVIEW_POSX , TRANSFERVIEW_POSY + 256 );
	glColor3ub( 255 , 255 , 0 );
	glVertex2i( TRANSFERVIEW_POSX , TRANSFERVIEW_POSY - 15 );
	glVertex2i( TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH , TRANSFERVIEW_POSY - 15 );
	glVertex2i( TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH , TRANSFERVIEW_POSY );
	glVertex2i( TRANSFERVIEW_POSX , TRANSFERVIEW_POSY );
	glEnd();

	glEnable( GL_BLEND );
	glColor4ub( 255 , 255  , 255 , 128 );
	glBegin( GL_LINE_STRIP );
	int step = TRANSFERVIEW_WIDTH / 256 ;
	for( int i = 0 ; i < 256 ; i++){
		glVertex2i( TRANSFERVIEW_POSX + i*step , path[i] + TRANSFERVIEW_POSY  ); 
	}
	glEnd();
	glBegin( GL_LINES );
	for( int i = 0 ; i < 256 ; i++){
		glVertex2i( TRANSFERVIEW_POSX + i*step , path[i] + TRANSFERVIEW_POSY  ); 
		glVertex2i( TRANSFERVIEW_POSX + i*step , TRANSFERVIEW_POSY ); 
	}
	glEnd();
	glDisable( GL_BLEND );
}
void drawColorTiles(){
	RGBType rgb =HSV_to_RGB(panel_color );
	diffuse[0] = rgb.R;
	diffuse[1] = rgb.G;
	diffuse[2] = rgb.B;
	RGBType color;
	for( int i = 0 ; i < PRFER_COLOR_TILE_NUM ; i++){ 
		color = preferColor[i].getColor();
		glColor3f( color.R , color.G , color.B );
		glPushMatrix();
		glTranslatef( i * 60 , 0 , 0);
		glPopMatrix();
	}
	glColor3f(rgb.R , rgb.G , rgb.B );
	glBegin( GL_QUADS );
	glVertex2f( 100 , 0);
	glVertex2f( 150 , 0);
	glVertex2f( 150 , 50);
	glVertex2f( 100 , 50);
	glEnd();
}
void drawTriangle( LittleTriangle* t){
	RGBType rgb = HSV_to_RGB(t->hsv);
	glColor3f( rgb.R , rgb.G , rgb.B );
	glBegin( GL_TRIANGLES);
	glVertex2i( TRANSFERVIEW_POSX + t->x , TRANSFERVIEW_POSY );
	glVertex2i( TRANSFERVIEW_POSX + t->x - 10 , TRANSFERVIEW_POSY - 15);
	glVertex2i( TRANSFERVIEW_POSX + t->x + 10 , TRANSFERVIEW_POSY - 15);
	glEnd();
}
void drawLittleTriangle(){
	list<LittleTriangle*>::iterator it;

	for( it = myList.begin() ; it != myList.end() ; it++)
		drawTriangle(*it);
}
void drawArea(){
	
	glColor3ub( 0  , 0  , 0);	
	//glActiveTexture( GL_TEXTURE0 );
	//glEnable( GL_TEXTURE_1D);
	glBegin( GL_QUADS );
	//glTexCoord1i( 0 );
	glVertex2i(TRANSFERVIEW_POSX , TRANSFERVIEW_POSY + 260);
	//glTexCoord1i( 0 );
	glVertex2i( TRANSFERVIEW_POSX  , TRANSFERVIEW_POSY + 290);
	//glTexCoord1i( 1 );
	glVertex2i( TRANSFERVIEW_POSX + 256 ,  TRANSFERVIEW_POSY + 290);
	//glTexCoord1i( 1 );
	glVertex2i(TRANSFERVIEW_POSX + 256 ,  TRANSFERVIEW_POSY + 260);
	glEnd();
	  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );
	//glDisable( GL_TEXTURE_1D);
	glEnable( GL_BLEND );
	glBegin( GL_QUAD_STRIP );
	int step = TRANSFERVIEW_WIDTH / 256;
	for( int i = 0 ; i != 256 ; i++){
	    glColor4f( rgba[i][0]  , rgba[i][1]  , rgba[i][2] , rgba[i][3]);
		glVertex2i( TRANSFERVIEW_POSX + step*i ,  TRANSFERVIEW_POSY + 260);
		glVertex2i( TRANSFERVIEW_POSX + step*i ,  TRANSFERVIEW_POSY + 290);
	}
	glEnd();
	glDisable( GL_BLEND );
}
void display_(){

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	drawColorWheel();
	drawClickH();
	drawClickSV();

	//drawColorTiles();
	
	
	drawTransferFuctionView();
	drawArea();
	drawHistogram();
	drawLittleTriangle();
	
	if( pActiveTriangle != NULL ){
		glColor3ub( 0 , 0 , 0);
		glBegin( GL_LINE_LOOP );
		glVertex2i( pActiveTriangle->x - 10 + TRANSFERVIEW_POSX , TRANSFERVIEW_POSY );
		glVertex2i( pActiveTriangle->x + 10 + TRANSFERVIEW_POSX , TRANSFERVIEW_POSY );
		glVertex2i( pActiveTriangle->x + 10 + TRANSFERVIEW_POSX , TRANSFERVIEW_POSY - 15 );
		glVertex2i( pActiveTriangle->x - 10 + TRANSFERVIEW_POSX , TRANSFERVIEW_POSY - 15);
		glEnd();
	}
	glutSwapBuffers();
}
void reshape_( int w , int h){
	panel_height = h;
	panel_width = w;
	glViewport( 0 , 0 , w , h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D( 0 , w , 0 , h );
}
LittleTriangle* findTriangle( int x){
	list<LittleTriangle*>::iterator it;
	LittleTriangle* pActiveTriangle = NULL;
				int xx = x - TRANSFERVIEW_POSX;
				for( it = myList.begin() ; it != myList.end() ; it++){
					if( xx  > ((*it)->x - 10) && xx < ((*it)->x + 10)){
						pActiveTriangle = *it;
						break;
					}
				}
	return pActiveTriangle;
}

void mouseButtHandler_(int button, int state, int x, int y)
{


	//壓下標記+新增 三角形  pActiveTriangle

	int yy = panel_height - y;
	printf("hit %d , %d\t " , x , y);
	mouse_button = button;
	switch( button ){
	case GLUT_RIGHT_BUTTON:
	
		if( pActiveTriangle == findTriangle(x) ){
			myList.remove( pActiveTriangle );
			pActiveTriangle = NULL;
			list<LittleTriangle*>::iterator it2;
			list<LittleTriangle*>::iterator it;
			it2 = it = myList.begin();
			it2++;
			for( int i =0 ; i < (myList.size() - 1) ; i++){
				fillColor( *it , *it2 );
				it++;
				it2++;
			}
			updateRGBA();
		}
		break;
	}
	if(state == GLUT_DOWN ){
			LittleTriangle* tmp =  pActiveTriangle;
			if( x > TRANSFERVIEW_POSX && x < TRANSFERVIEW_POSX + TRANSFERVIEW_WIDTH && yy > TRANSFERVIEW_POSY - 15 && yy < TRANSFERVIEW_POSY ){
				list<LittleTriangle*>::iterator it;

				if( !(pActiveTriangle = findTriangle(x)) ){
					HSVType hsv;
					hsv.H = (float) clickH;
					hsv.S = clickS;
					hsv.V = clickV;

					LittleTriangle* t;
					t = new LittleTriangle;
					t->x = x - 100;
					t->hsv = hsv;
					pActiveTriangle = t;

					myList.push_back(t);
					myList.sort( compare_list);

					//list<LittleTriangle>::iterator it;
					list<LittleTriangle*>::iterator it2;

					it2 = it = myList.begin();
					it2++;
					for( int i =0 ; i < (myList.size() - 1) ; i++){
						fillColor( *it , *it2 );
						it++;
						it2++;
					}

					updateRGBA();
				}
			}else
				if( x > CENTRAL_X - WHEEL_RADIUS  && x <  CENTRAL_X + WHEEL_RADIUS && yy >  CENTRAL_Y - WHEEL_RADIUS && yy < CENTRAL_Y + WHEEL_RADIUS){
					wheelDetection( x , yy , CENTRAL_X , CENTRAL_Y , WHEEL_RADIUS , WHEEL_RADIUS - 20);
					triangleDetection( x , yy , CENTRAL_X , CENTRAL_Y , WHEEL_RADIUS );
					panel_color.H = clickH;
					panel_color.V = clickV;
					panel_color.S = clickS;
					if( pActiveTriangle != NULL ){
						pActiveTriangle->hsv = panel_color;
						list<LittleTriangle*>::iterator it2;
						list<LittleTriangle*>::iterator it;
						it2 = it = myList.begin();
						it2++;
						for( int i =0 ; i < (myList.size() - 1) ; i++){
							fillColor( *it , *it2 );
							it++;
							it2++;
						}
						updateRGBA();
					}
				}else{
					pActiveTriangle = NULL;
				}
				globalPost_x = x - TRANSFERVIEW_POSX;
				globalPost_y = yy - TRANSFERVIEW_POSY;
		}else if( state = GLUT_UP ){
			if ( !mouse_move &&  hit ){
				myList.remove( pActiveTriangle );
				pActiveTriangle = NULL;
			}
			int begin = 0,end = 0;
			for( int i = 0 ; i != 256 ; i++){
				if(path[i] != 0){
					begin = i;
					break;
				}
			}
			for( int i = 255 ; i != -1 ; i--){
				if(path[i] != 0){
					end = i;
					break;
				}
			}
			printf("Range: %d ~ %d ,w = %d\n" , begin , end , (end - begin + 1));
		}
}

void mouseMoveHandler_(int x, int y){
	mouse_move = 1;
	int yy = panel_height - y;
	static int current_x , current_y;
	current_x = x - TRANSFERVIEW_POSX;
	current_y = yy - TRANSFERVIEW_POSY;
	int valid = false;
	if( x > TRANSFERVIEW_POSX && x < TRANSFERVIEW_POSX + 256 && yy > TRANSFERVIEW_POSY - 15 && yy < TRANSFERVIEW_POSY ){
		if( pActiveTriangle != NULL ){
			pActiveTriangle->x = current_x;

			myList.sort( compare_list);

					list<LittleTriangle*>::iterator it;
					list<LittleTriangle*>::iterator it2;

					it2 = it = myList.begin();
					it2++;

					for( int i =0 ; i < (myList.size() - 1) ; i++){
						fillColor( *it , *it2 );
						it++;
						it2++;
					}
			updateRGBA();
		}
	}

	if( current_x > -5 && current_x < 260 && current_y >= -5 && current_y  <=   260  ){
		if( globalPost_x  < 0 )globalPost_x = 0;
		if( globalPost_x > 255 )globalPost_x = 255;
		if( globalPost_y < 0 ) globalPost_y = 0;
		if( globalPost_y > 255 ) globalPost_y = 255;
		if( globalPost_x >= 0 && globalPost_x <= 255 && globalPost_y >= 0 && globalPost_y  <=   255  ){
			if( current_x  < 0 ) current_x = 0;
			if( current_x  > 255 ) current_x = 255;
			if( current_y  < 0 ) current_y = 0;
			if( current_y  > 255 ) current_y = 255;
			if( mouse_button == GLUT_RIGHT_BUTTON ){
				globalPost_y = current_y = 0;
			}
			updateTransferFuctionPartView( globalPost_x , globalPost_y , current_x , current_y );
			updateRGBA();
		}
		globalPost_x = current_x;
		globalPost_y = current_y;
	}

} 
void keyboard_(unsigned char key , int x , int y){
	if( key == 'G' || key == 'g'){
		pressedG ^= 1;
	}
}