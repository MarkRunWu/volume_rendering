#include <GL\glut.h>
#include <math.h>
#include <stdio.h>
/*
** draw2DTransferFunction
*/
#define TF_2D_START_X 500
#define TF_2D_START_Y 100
#define TF_2D_SIZE 256
int histogram_2D[TF_2D_SIZE][TF_2D_SIZE] = {0};
extern void updateWindows();
float his2D_max = 0;
void draw2DTransferFunction()
{
	//string_render("2D Histogram", TF_2D_START_X-1, TF_2D_START_Y + TF_2D_SIZE+3, win2Color);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
		glVertex2i(TF_2D_START_X - 1, TF_2D_START_Y - 1);
		glVertex2i(TF_2D_START_X + TF_2D_SIZE, TF_2D_START_Y - 1);
		glVertex2i(TF_2D_START_X + TF_2D_SIZE, TF_2D_START_Y + TF_2D_SIZE);
		glVertex2i(TF_2D_START_X - 1, TF_2D_START_Y + TF_2D_SIZE);
	glEnd();
	glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	//畫 2D Histogram
	glEnable(GL_BLEND);
	glBegin(GL_POINTS);
		for(int i = 0; i < TF_2D_SIZE; i++)
		{
			for(int j = 0; j < TF_2D_SIZE; j++)
			{
				glColor4f( 1.0 , 1.0 , 1.0 , histogram_2D[ i ][ j ] / 10 / log(his2D_max) );
				glVertex2i( TF_2D_START_X + i, TF_2D_START_Y + j );
			}
		}
	glEnd();
	glDisable(GL_BLEND);

}

/*
** compute_gradient
*/
void compute_gradient(float* temp , int WIDTH, int HEIGHT, int DEPTH)
{
	float gradient_max = 0;	
	float gradient_min = 10;	
	float up, down, left, right, forward, behind;

	//t1.start();
	for(int i = 0; i < WIDTH; i++)
	{
		for(int j = 0; j < HEIGHT; j++)
			for(int k = 0; k < DEPTH; k++)
			{
				if(k == 0)		left = 0;	
				else			left = temp[ (k - 1) * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 ];
				if(k == DEPTH - 1)	right = 0;
				else			right = temp[ (k + 1) * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 ];

				if(j == 0)		up = 0;
				else			up = temp[ k * HEIGHT * WIDTH * 2 + (j - 1) * WIDTH * 2 + i * 2 ];
				if(j == HEIGHT - 1)	down = 0;
				else			down = temp[ k * HEIGHT * WIDTH * 2 + (j + 1) * WIDTH * 2 + i * 2 ];

				if(i == 0)		forward = 0;
				else			forward = temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + (i - 1) * 2 ];
				if(i == WIDTH - 1)	behind = 0;
				else			behind = temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + (i + 1) * 2 ];

				temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 + 1 ] = sqrt(
					pow( (double)(forward - behind) / 2, 2.0 ) +
					pow( (double)(right - left) / 2, 2.0) +
					pow( (double)(up - down) / 2, 2.0)
					);
		
				if( temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 + 1] > gradient_max )
					gradient_max = temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 + 1];
				else if( temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 + 1] < gradient_min )
					gradient_min = temp[ k * HEIGHT * WIDTH * 2 + j * WIDTH * 2 + i * 2 + 1];
			}
	}

	//normalize gradient 
	for(int i = 0; i < WIDTH; i++)
		for(int j = 0; j < HEIGHT; j++)
			for(int k = 0; k < DEPTH; k++){
				temp[ i * HEIGHT * DEPTH * 2 + j * DEPTH * 2 + k * 2 + 1] = (temp[ i * HEIGHT * DEPTH * 2 + j * DEPTH * 2 + k * 2 + 1] - gradient_min) / (gradient_max - gradient_min);
			}
	//t1.stop();
	/*printf("time = %f s\n", t1.getElapsedTimeInSec());*/
	printf("gradient max = %f\n", gradient_max);
	printf("gradient min = %f\n", gradient_min);


	//2D Histogram 歸零
	for( int i = 0; i < TF_2D_SIZE; i++ )
		for( int j = 0; j < TF_2D_SIZE; j++ )
			histogram_2D[ i ][ j ] = 0;

	//2D Histogram 統計
	float temp1;
	float temp2;
	for(int i = 0; i < WIDTH; i++)
		for(int j = 0; j < HEIGHT; j++)
			for(int k = 0; k < DEPTH; k++)
			{
				temp1 = temp[ i * HEIGHT * DEPTH * 2 + j * DEPTH * 2 + k * 2];
				temp2 = temp[ i * HEIGHT * DEPTH * 2 + j * DEPTH * 2 + k * 2 + 1];
				histogram_2D[ (int)(temp1 * 255) ][ (int)(temp2 / gradient_max * 255.0 ) ] ++;
			}
	
	
	//2D Histogram 最大值
	float his2D_min = histogram_2D[ 0 ][ 0 ];
	for( int i = 0; i < TF_2D_SIZE; i++ )
		for( int j = 0; j < TF_2D_SIZE; j++ )
		{
			if( histogram_2D[ i ][ j ] > his2D_max )
				his2D_max = histogram_2D[ i ][ j ];
			else if( histogram_2D[ i ][ j ] < his2D_min )
				his2D_min = histogram_2D[ i ][ j ];
		}

	//2D Histogram 內插
	bool intersect = true;
	for( int i = 0; i < TF_2D_SIZE; i++ )
	{
		intersect = true;
		for( int j = 0; j < TF_2D_SIZE; j++ )
		{
			if( histogram_2D[ j ][ i ] > his2D_min )
			{
				intersect = false;
				break;
			}
				
		}
		if(intersect)
		{
			for( int j = 0; j < TF_2D_SIZE; j++ )
				histogram_2D[ j ][ i ] = histogram_2D[ j ][ i - 1 ];
		}
	}

	printf("histogram_2D max = %f\n", his2D_max);

	//showState[0] = '\0';
	//updateWindows();
}