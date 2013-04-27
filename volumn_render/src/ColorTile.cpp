#include "ColorTile.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>
ColorTile::ColorTile(){
	srand( (unsigned int) time(NULL) );
	setColor( rand() % 255 , rand() % 255 , rand() % 255 );
}
ColorTile::ColorTile( RGBType rgb )
{
	mColor = rgb;
}


ColorTile::~ColorTile(void)
{
}

RGBType ColorTile::getColor(){
	return mColor;
}

void ColorTile::setColor(int r , int g , int b){
	assert( r <= 255 && r >= 0);
	assert( g <= 255 && g >= 0);
	assert( b <= 255 && b >= 0);
	mColor.R = r/255.f;
	mColor.G = g/255.f;
	mColor.B = b/255.f;
}

