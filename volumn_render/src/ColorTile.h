#pragma once
#include "hsv\hsv.h"
class ColorTile
{
public:
	ColorTile();
	ColorTile( RGBType rgb);
	RGBType getColor();
	void setColor(int r , int g , int b);
	~ColorTile(void);
private:
	RGBType mColor;
};

