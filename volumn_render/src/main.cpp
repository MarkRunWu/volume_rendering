/*
** Student ID number: // fill_me
*/

#include "Resource.h"
#include "textfile.h"
#define DEGREE2RADIAN( x )( ((x) / 180.0) * 3.14159265)
extern GLfloat current_pos_bunny[];
extern GLfloat current_rot_bunny[];
extern GLfloat current_sca_bunny[];
extern float slipAngle;
extern int record;
extern int panel_height;
extern int panel_width;
extern int isPressedB;
unsigned char image[512][512][3];

void guass_diffusion();

int window , window2;
GLint transferfunctionLoc , dataloc , viewloc , normal_of_viewplane , lightpos_loc;
int updateFlag;

PLYObject *ply = NULL;
GLuint texture_index;
perspectiveData pD;

GLuint v0, f0, p0; // shader

int histogram[256] = {0};
int path[256] = {0};

int data_width = 256;
int data_height = 256;
int data_depth = 240;

GLuint texName3D;

int slice_mode = 1;
float index = 0 ;
float rgba[256][4];

list<LittleTriangle*> myList;
GLuint texName_1D;
unsigned char mdata[240][256][256];
unsigned char out [240][256][256];
void updateTexture(int window){
	glutSetWindow(window);
	glActiveTexture( GL_TEXTURE0 );
	int level = 0;
	int border = 0;
	int width = 256;
	glTexImage1D( GL_TEXTURE_1D , level , GL_RGBA , width , border , GL_RGBA , GL_FLOAT , rgba );

	glTexParameterf( GL_TEXTURE_1D , GL_TEXTURE_WRAP_S , GL_CLAMP );
	glTexParameterf( GL_TEXTURE_1D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
	glTexParameterf( GL_TEXTURE_1D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );

	glDisable( GL_TEXTURE_1D );
}

void updateRGBA(){
	updateTexture(window);
	updateTexture(window2);
}



int savePrefernceColorTranslationFunc(){
	FILE *fout = fopen( "default.tfe", "wb");
	if( fout != NULL ){
		printf("save to file 'default.tfe");
		fwrite( rgba , sizeof(float) , 256*4 , fout);
		list<LittleTriangle*>::iterator it = myList.begin();
		while( it != myList.end() ){
			fwrite( (*it) , sizeof(LittleTriangle) , 1 , fout  );
			it++;
		}
		fclose(fout);
		return 1;
	}else{
		fprintf(stderr, "no such file 'default.tfe'!\n");
		return 0;
	}
}

void saveOpacityTF(){
	FILE *pOpacityTF = fopen("hurricane.tfe" , "wb");
	//tf resolution
	float resolution = 256;
	fwrite( &resolution , sizeof(float) , 1 ,  pOpacityTF );
	for( int i = 0 ; i != 256 ; i++ ){
		fwrite( &rgba[i][3] , sizeof(float) , 1 , pOpacityTF );
	}
	fclose( pOpacityTF );
}
int loadPrefernceColorTranslationFunc(){
	FILE *fin = fopen( "default.tfe", "rb");
	if( fin != NULL ){
		printf("load from file 'default.tfe'");
		fread( rgba , sizeof(float) , 256*4 , fin);
		myList.clear();
		while( !feof (fin) ){
			LittleTriangle* tmp = new LittleTriangle;
			if( fread( tmp , sizeof(LittleTriangle) , 1 , fin  ) ){
				myList.push_back( tmp );
				/*
				printf("load %d\n" , tmp->x);
				printf("load %.3f\n" , tmp->hsv.H);
				printf("load %.3f\n" , tmp->hsv.S);
				printf("load %.3f\n" , tmp->hsv.V);
				*/
			}
		}
		fclose(fin);
		updateRGBA();
		return 1;
	}else{
		fprintf(stderr, "no such file 'default.tfe'!\n");
		return 0;
	}
}

/*
**
*/
void gl_ortho_begin(unsigned size_x, unsigned size_y)
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.375f, 0.375f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, size_x, 0, size_y);
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}


/*
**
*/
void gl_ortho_end(void)
{
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


/*
**  cleanup
*/
void cleanup(int sig)
{
	//delete(ply);
	exit(0);
}


/*
**  drawPlane
*/
void drawPlane()
{
	// floor
	glColor3f(0.8f, 0.9f, 0.8f);
	glBegin(GL_POLYGON);
	glVertex3f(-5,-1, 5);
	glVertex3f( 5,-1, 5);
	glVertex3f( 5,-1,-5);
	glVertex3f(-5,-1,-5);
	glEnd();

	// wire
	float i;
	glColor3f(0.5,0,0);
	glBegin(GL_LINES);
	for (i=-5.0f; i<=5.0f; i+=1.0f){
		glVertex3f( i,-0.98f,  5);
		glVertex3f( i,-0.98f, -5);
		glVertex3f( 5,-0.98f,  i);
		glVertex3f(-5,-0.98f,  i);
	}
	glEnd();

	// axis
	glBegin(GL_LINES);
	for (i=-5.0f; i<=5.0f; i+=1.0f){
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(5,0,0);
		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,5,0);
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,5);
	}
	glEnd();
}


/*
**  string render function
*/
void string_render(const char *string, GLint x, GLint y)
{
	unsigned      i;

	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2i(x + 1, y - 1);
	for (i = 0; string[i]; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
}


/*
**  screenshot
*/
void screenshot(int x, int y, int w, int h)
{
	FIBITMAP *image = FreeImage_Allocate(w, h, 24);

	unsigned char *data = new unsigned char[w*h*3];
	unsigned char *temp = new unsigned char[w*3];

	if(!data || !temp)
	{
		fprintf(stderr, "no more memory!\n");
	}

	glReadPixels(x,y,w,h, GL_RGB, GL_UNSIGNED_BYTE, data);

	int i;
	unsigned char tempData;
	for(i=0; i<w*h*3; i+=3){
		tempData = data[i+2];
		data[i+2] = data[i];
		data[i]   = tempData;
	}

	// Save image
	image=FreeImage_ConvertFromRawBits(data, w,h,w*3, 24, 0,0,0);
	if(!image)
	{
		fprintf(stderr, "Failed to convert bits!\n");
	}


	static int count = 0;
	char filename[50];

	sprintf(filename,"%05d.png", count);

	if(FreeImage_Save(FIF_PNG, image, filename))
		fprintf(stderr, "Wrote %s!\n", filename);
	else
		fprintf(stderr, "Failed!\n");

	count++;

	FreeImage_Unload(image);

	delete [] temp;
	delete [] data;
}

void drawVolumnBox(){
	glPushMatrix();
	//glTranslatef( 0.5 , 0.5, 0.5 );
	//glScalef( 0.5 ,0.5, 0.5);
	glColor3f( 1.f , 0.f , 0.f );
	glBegin(GL_LINE_LOOP);
	glVertex3f( 1 , 1 , -1 );
	glVertex3f( 1 ,-1 , -1 );
	glVertex3f( -1 ,-1  , -1 );
	glVertex3f( -1 , 1 , -1 );
	glEnd();
	glColor3f( 0.f , 1.f , 0.f );
	glBegin(GL_LINE_LOOP);
	glVertex3f( 1 , 1 , 1 );
	glVertex3f( 1 ,-1 , 1 );
	glVertex3f( -1 ,-1  , 1 );
	glVertex3f( -1 , 1 , 1 );
	glEnd();

	glColor3f( 0.f , 0.f , 1.f );
	glBegin(GL_LINE_LOOP);
	glVertex3f( 1 , 1 , 1 );
	glVertex3f( 1 ,1 , -1 );
	glVertex3f( -1 ,1  , -1 );
	glVertex3f( -1 , 1 , 1 );
	glEnd();


	glBegin(GL_LINE_LOOP);
	glVertex3f( 1 , -1 , 1 );
	glVertex3f( 1 ,-1 , -1 );
	glVertex3f( -1 ,-1  , -1 );
	glVertex3f( -1 , -1 , 1 );
	glEnd();
	glPopMatrix();
}

void showVolumnDataByPlane( Vector3f normal , int pos  ){
	//0,0,1 z
	//0,1,0 y
	//1,0,0 x


}
// using shader to draw volumn data
void drawDataUsingShader(){
	glUseProgram( p0 );
	glPushMatrix();
	glScalef(2 , 2, 2);
	glTranslatef( -0.5 , -0.5 , -0.5 );
	glColor3f( 1 , 1 , 1);
	glBegin( GL_QUADS );
	glVertex3f( 0  , index/240.0 , 0);
	glVertex3f( 1  , index/240.0 , 0);
	glVertex3f( 1  , index/240.0 , 1);
	glVertex3f( 0  , index/240.0 , 1);
	glEnd();
	glPopMatrix();
	glUseProgram(0);

}
/*
**
*/
void display(void)
{  
	glutSetWindow( window );
	if( !isPressedB )
		glClearColor(1.0, 1.0, 1.0, 1.0);
	else
		glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setUserView();



	float view[3];
	float mv[16];
	float axis_z[3];
	float axis_x[3];
	float axis_y[3];
	

	glUseProgram(0);
	
	//drawPlane();

	/*
	glPushMatrix();
	// fill_me...
	ply->draw();  
	glPopMatrix();
	*/
	// string render
	gl_ortho_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
	string_render("Computer Graphics (Spring 2012), Name: Mark Run Wu, ID: 100598020", 20, IMAGE_HEIGHT-20); // fill_me...
	string_render("v: start/end screenshot", 20, IMAGE_HEIGHT-40);
	string_render("p: Open volume data", 20, IMAGE_HEIGHT-60);
	gl_ortho_end();


	// kamon
	glBegin(GL_POINTS);
	for (int i=0; i<512; i++){
		for (int j=0; j<512; j++){
			glColor3ubv(image[i][j]);
			glVertex3f(2.5f + j/200.0f, -0.99, 2.5f + i/200.0f);
		}
	}
	glEnd();

	drawVolumnBox();

	

	//showVolumnData();
	/*
	switch( slice_mode ){
	case 0:
	showVolumnData();
	break;
	case 1:
	showVolumnDataByPlane(0);
	break;
	}
	*/
	glDisable(GL_TEXTURE_3D);
	glActiveTexture( GL_TEXTURE0 );
	glColor3ub( 255 , 255 ,255);
	glEnable( GL_TEXTURE_1D );
	gl_ortho_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
	glBegin( GL_QUADS );
	glTexCoord1i( 0 );
	glVertex2f( 0.f , 0.f );
	glTexCoord1i( 0 );
	glVertex2f( 0.f , 100.f );
	glTexCoord1i( 1 );
	glVertex2f( 400.f , 100.f );
	glTexCoord1i( 1 );
	glVertex2f( 400.f , 0.f );
	glEnd();
	gl_ortho_end();

	// shader render
	glDisable( GL_TEXTURE_1D );

	glGetFloatv( GL_MODELVIEW_MATRIX , mv );
	for( int i = 0 ; i != 3 ; i++){
		// orientation
		axis_x[i] = (float)mv[4 * i + 0];
		axis_y[i] = (float)mv[4 * i + 1];
		axis_z[i] = (float)mv[4 * i + 2];
		
		view[i] = 0.0f;
		for( int j = 0 ; j != 3 ; j++){
			view[i] -= (float)( mv[4*i+j]*mv[12 + j]);
		}
	}
	
	
	float viewplan[3];



	//glDisable( GL_BLEND );
	glUseProgram(p0);
	
	
	// uniform
	glUniform1i( transferfunctionLoc , 0);
	glUniform1i(dataloc,1);
	glUniform3f( viewloc , view[0] , view[1] , view[2] );
	glUniform3f( normal_of_viewplane , axis_z[0] , axis_z[1] , axis_z[2] );
	glUniform3f( lightpos_loc , 1 , -1 , 0 );
	/* viewplane
	glPushMatrix();
	glTranslatef( -0.5*axis_z[0] , -0.5*axis_z[1] , -0.5*axis_z[2] );
	//glTranslatef( -10*axis_z[0] , -10*axis_z[1] , -10*axis_z[2] );
	glBegin( GL_QUADS );
	glVertex3f( view[0] + (axis_x[0] + axis_y[0]) , view[1] + (axis_x[1] + axis_y[1]) , view[2] + (axis_x[2] + axis_y[2]) );
	glVertex3f( view[0] + (-axis_x[0] + axis_y[0]) , view[1] + (-axis_x[1] + axis_y[1]) , view[2] + (-axis_x[2] + axis_y[2]) );
	glVertex3f( view[0] - (axis_x[0] + axis_y[0])  , view[1] - (axis_x[1] + axis_y[1]) , view[2] - (axis_x[2] + axis_y[2]) );
	glVertex3f( view[0] + (axis_x[0] - axis_y[0])  , view[1] + (axis_x[1] - axis_y[1]) , view[2] + (axis_x[2] - axis_y[2]) );
	glEnd();
	glPopMatrix();
	*/
	/*
	* bounding box (-1 , -1 , -1) ~ (1 , 1 , 1)
	*/
	glPushMatrix();
	glEnable( GL_BLEND );
	glBegin( GL_QUADS );
	glNormal3f( 0 , 1 , 0);
	glVertex3f( -1.f , 1 , -1.f );
	glVertex3f( 1.f ,1, -1.f );
	glVertex3f( 1.f , 1 , 1.f );
	glVertex3f( -1.f ,1, 1.f );
	glEnd();
	glBegin( GL_QUADS );
	glNormal3f( 0 , -1 , 0);
	glVertex3f( -1.f , -1.f , -1.f );
	glVertex3f( 1.f ,-1.f, -1.f );
	glVertex3f( 1.f , -1.f , 1.f );
	glVertex3f( -1.f ,-1.f, 1.f );
	glEnd();
	glBegin( GL_QUADS );
	glNormal3f( 0 , 0 , -1);
	glVertex3f( -1.f , -1.f ,-1.f);
	glVertex3f( -1.f , 1.f ,-1.f);
	glVertex3f( 1.f , 1.f ,-1.f);
	glVertex3f( 1.f , -1.f ,-1.f);
	glEnd();
	glBegin( GL_QUADS );
	glNormal3f( 0 , 0 , 1);
	glVertex3f( -1.f , -1.f ,1);
	glVertex3f( 1.f , -1.f ,1);
	glVertex3f( 1.f , 1.f ,1);
	glVertex3f( -1.f , 1.f ,1);
	glEnd();
	glBegin( GL_QUADS );
	glNormal3f( -1 , 0 , 0);
	glVertex3f( -1.f , -1.f , -1.f );
	glVertex3f( -1.f , -1.f , 1.f );
	glVertex3f( -1.f , 1.f , 1.f );
	glVertex3f( -1.f , 1.f , -1.f );
	glEnd();
	glBegin( GL_QUADS );
	glNormal3f( 1 , 0 , 0);
	glVertex3f( 1 , -1.f , -1.f );
	glVertex3f( 1 , 1.f , -1.f );
	glVertex3f( 1 , 1.f , 1.f );
	glVertex3f( 1 , -1.f , 1.f );
	glEnd();
	glPopMatrix();
		/*
	glBegin( GL_QUADS );
	glVertex3f( 0.f , index/240.0 , 0.f );
	glVertex3f( 0.f ,index/240.0, 1.f );
	glVertex3f( 1.f , index/240.0 , 1.f );
	glVertex3f( 1.f ,index/240.0 , 0.f );
	glEnd();
	*/

	// screenshot

	// screenshot
	if (record){
		screenshot(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
	}

	glutSwapBuffers();
}


/*
**
*/
void reshape(int width, int height)
{
	int screenWidth  = width;
	int screenHeight = height;

	if (screenWidth <= 0 || screenHeight <=0)
		return;

	glViewport(0, 0, screenWidth, screenHeight);
	glutReshapeWindow(screenWidth, screenHeight);

	pD.aspect = (float)screenWidth / screenHeight;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(pD.fieldOfView, pD.aspect, pD.nearPlane, pD.farPlane);

	// set basic matrix mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*
**
*/
void initDisplay()
{
	// Perspective projection parameters
	pD.fieldOfView = 45.0;
	pD.aspect      = (float)IMAGE_WIDTH/IMAGE_HEIGHT;
	pD.nearPlane   = 0.1;
	pD.farPlane    = 50.0;

	// setup context
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(pD.fieldOfView, pD.aspect, pD.nearPlane, pD.farPlane);

	// set basic matrix mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearIndex(0);
	glClearDepth(1);

	LittleTriangle* start ,* end;
	start = new LittleTriangle;
	end = new LittleTriangle;
	end->x = TRANSFERVIEW_WIDTH;
	start->x = 0;
	HSVType a;
	a.H = a.S = a.V = 1;
	start->hsv = end->hsv = a;// RETURN_HSV( 1 , 2 , 3 );
	myList.push_back(start);
	myList.push_back(end);

	//-------------------------------------------------------
	// Kamon (image.raw) must be 256x256 (color image) pixels
	FILE *p;
	p = fopen("qr.raw", "rb");  // fill_me, change it to your student ID number
	fread(image, sizeof(unsigned char), 512*512*3, p);
	fclose(p);
	//-------------------------------------------------------

}


/*
**
*/
void idle_callback(void){
	index++;
	if( index > data_depth){
		index = 0;
	}
	glutSetWindow( window );
	glutPostRedisplay();
	glutSetWindow( window2 );
	glutPostRedisplay();
}
void initData2( const char* volumn_file){
	char filename[100];
	
	strcpy(  filename , volumn_file);
	printf("open %s\n" , filename);
	char* extension = strstr( filename , ".hdr");
	FILE* fheader = fopen(filename , "r");
	unsigned char* data;
	fscanf( fheader ,  "%d%d%d" , &data_width , &data_height , &data_depth );
	fclose( fheader );

	//allocate w * h * d memory
	data = (unsigned char*) malloc(sizeof(unsigned char)*data_depth * data_width * data_height );
	//teapot.Data = new unsigned char[width*height*depth];
	
	strncpy( extension , ".dat", 4 );
	
	//read data from disk
	FILE* fin = fopen(filename , "rb");
	if( !fin){
		fprintf(stderr, "Cannot open input file %s.\n", volumn_file);
		exit(1);
	}
	fread( data , sizeof(unsigned char) , data_depth*data_width*data_height , fin );
	fclose(fin);

	//reset histogram
	for( int k = 0 ; k != 256 ; k++){
		histogram[ k ] = 0;
	}
	//histogram
	
	for( int k = 0 ; k < data_depth ; k++){
		for( int i = 0 ; i < data_width ; i++){
			for( int j = 0 ; j < data_height ; j++){

				histogram[ (int)data[k*data_width*data_height + i*data_height + j] ]++;
			}
		}
	}
	
	
	glActiveTexture( GL_TEXTURE1 );
	fprintf(stderr, "texture id: %u.\n" , texName3D);
	glBindTexture(GL_TEXTURE_3D , texName3D );
	//set Texture mapping parameters
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_WRAP_T , GL_CLAMP);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_WRAP_S , GL_CLAMP);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_WRAP_R , GL_CLAMP);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

	//generate a 3D texture
	glTexImage3D(GL_TEXTURE_3D , 0 , GL_LUMINANCE , data_width , data_height , data_depth , 0 , GL_LUMINANCE , GL_UNSIGNED_BYTE , data );

	//write into mData;

	for( int k = 0 ; k < data_depth ; k++){
		for( int i = 0 ; i < data_width ; i++){
			for( int j = 0 ; j < data_height ; j++){
				mdata[k][i][j] = data[k*data_width*data_height + i*data_height + j] ;
			}
		}
	}
	//guass_diffusion();
	
	glDisable(GL_TEXTURE_3D);

	
}

void init_ply(void)
{
	FILE *in;
	char *filename = "bunny.ply"; // "dragon.ply", "happy.ply"

	if (!(in = fopen(filename, "r"))) {
		fprintf(stderr, "Cannot open input file %s.\n", filename);
		exit(1);
	}
	ply = new PLYObject(in);
	ply->resize();
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}


void setShaders() 
{
	char *vs = NULL,*fs = NULL;

	v0 = glCreateShader(GL_VERTEX_SHADER);
	f0 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shaders/volumn.vert");
	fs = textFileRead("shaders/volumn.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v0, 1, &vv,NULL);
	glShaderSource(f0, 1, &ff,NULL);


	free(vs);free(fs);

	glCompileShader(v0);
	glCompileShader(f0);

	printShaderInfoLog(v0);
	printShaderInfoLog(f0);


	p0 = glCreateProgram();
	glAttachShader(p0,v0);
	glAttachShader(p0,f0);

	glLinkProgram(p0);
	printProgramInfoLog(p0);

	
	transferfunctionLoc = glGetUniformLocation( p0 , "transerfunction" );
	glUniform1i( transferfunctionLoc , 0);

	
	dataloc = glGetUniformLocation(p0 , "data");
	glUniform1i(dataloc,1);

	viewloc = glGetUniformLocation(p0 , "viewVec");
	normal_of_viewplane = glGetUniformLocation(p0 , "NofViewPlan");
	lightpos_loc = glGetUniformLocation(p0 , "lightDir");
	
}

//©|¥¼¥[¤J
#define min( x , y )( (x) < (y) ? (x) : (y))
#define max( x , y )( (x) > (y) ? (x) : (y))
void guass_diffusion(){
	int left,right,top,bottom,front,end;
	for(int i = 0 ; i != 240 ; i++){
		for( int j = 0 ; j != 256 ; j++ ){
			for( int k = 0 ; k != 256 ; k++ ){
				front = min( i + 1 , 239 );
				end   = max( i - 1 , 0 );
				left  = min( j + 1 , 255 );
				right = max( j - 1 , 0 );
				top   = min( k + 1 , 255 );
				bottom= max( k - 1 , 0 );
			   out[i][j][k] = (mdata[front][j][k] + mdata[end][j][k] + 
				mdata[i][left][k] + mdata[i][right][k] +
				mdata[i][j][top] + mdata[i][j][bottom] + 
				4 * mdata[i][j][k]) / 10;
			}
		}
	}
	
	for( int k = 0 ; k < data_depth ; k++){
		for( int i = 0 ; i < data_width ; i++){
			for( int j = 0 ; j < data_height ; j++){
				mdata[k][i][j] = out[k][i][j];
			}
		}
	}
	//replace texture3D with out
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture(GL_TEXTURE_3D , texName3D );
	//set Texture mapping parameters
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_WRAP_T , GL_CLAMP);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_WRAP_S , GL_CLAMP);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_WRAP_R , GL_CLAMP);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
	glTexParameterf( GL_TEXTURE_3D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);

	//generate a 3D texture and load out into shader
	glTexImage3D(GL_TEXTURE_3D , 0 , GL_LUMINANCE , data_width , data_height , data_depth , 0 , GL_LUMINANCE , GL_UNSIGNED_BYTE , mdata );
	for( int i = 0 ; i != 256 ; i++ ){
		histogram[i] = 0;
	}
	for( int k = 0 ; k < data_depth ; k++){
		for( int i = 0 ; i < data_width ; i++){
			for( int j = 0 ; j < data_height ; j++){

				histogram[ (int)mdata[k][i][j] ]++;
			}
		}
	}
	
}
/*
**
*/
int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(IMAGE_WIDTH,IMAGE_HEIGHT);
	glutInitWindowPosition(100,100);
	window = glutCreateWindow("Lab4, Name:§dÀà§Ó, ID:100598020");  // fill_me

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(readKeyboard);
	glutMouseFunc(mouseButtHandler);
	glutMotionFunc(mouseMoveHandler);
	glutPassiveMotionFunc(mouseMoveHandler);
	glutSpecialFunc(special_callback);
	glutIdleFunc(idle_callback);


	glutInitWindowSize( panel_width , panel_height );
	glutRemoveOverlay();
	glutInitWindowPosition(100 + IMAGE_WIDTH , 100 );
	window2 = glutCreateWindow( "control panel");
	glClearColor( .5f , .5f , .5f , 1.f );
	glutDisplayFunc(display_);
	glutReshapeFunc(reshape_);
	glutMouseFunc( mouseButtHandler_ );
	glutMotionFunc( mouseMoveHandler_ );
	glutKeyboardFunc( keyboard_ );
	glutSetWindow( window );

	//init_ply();
	//init_obj();
	glEnable( GL_BLEND );
	glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	//generate a texture id
	glGenTextures(1 , &texName3D);
	initData2("CT_head.hdr");
	initDisplay();
	setShaders();
	///////////////////////////////////
	//glutSetWindow( window2 );
	glGenTextures(1 , &texName_1D);
	fprintf(stderr, "texture1D id: %u.\n" , texName_1D);
	glBindTexture( GL_TEXTURE_1D , texName_1D );
	updateRGBA();
	
	glutMainLoop();


	return 0;             /* ANSI C requires main to return int. */
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         