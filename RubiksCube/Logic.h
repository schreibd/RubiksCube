#pragma once

#include "Side.h"
#include <gxm.h>

class Logic{
public:
	enum SIDE_INDEX {
		SIDE_FRONT = 0, //CHECK //WORKS
		SIDE_BACK = 1, //CHECK //WORKS
		SIDE_TOP = 2, //CHECK //WORKS
		SIDE_BOTTOM = 3, // CHECK //WORKS
		SIDE_LEFT = 4, //CHECK //WORKS
		SIDE_RIGHT = 5 //CHECK //WORKS
	};

	Side* sides[6];

public:
	Logic();
	~Logic();
	void RotateSelectedFace(int value);
	void RotateFront();
	void RotateBack();
	void RotateTop();
	void RotateBottom();
	void RotateLeft();
	void RotateRight();
	void RotateVerticalCenter();
	void RotateHorizontalCenter();
	void RotateMidZ();
};