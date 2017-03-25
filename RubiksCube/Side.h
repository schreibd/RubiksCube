#pragma once
#include <iostream>
#include <gxm.h>

enum Color
{
	WHITE	= 0,
	RED		= 1,
	GREEN	= 2,
	BLUE	= 3,
	PINK	= 4,
	YELLOW	= 5,
};

struct face{
	int order;
	Color color;
};

class Side {
public:
	int logicalSide;
	face faces[3][3];
public:
	Side();
	Side(int value, Color c);
	void print();

	static uint32_t getColorValue(Color c);
};
