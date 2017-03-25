#pragma once
#include <vectormath.h>
#include "Logic.h"
#include <iostream>

using namespace sce::Vectormath::Simd::Aos;

class Cube;

class Animator
{
public:
	Animator(Cube** cubes, Logic *logic);
	~Animator(void);
	void rotateSelectedSide(bool start, int side);
	void updateLogic();
	void update(float delta);
	void AnimateVerticalSegment(bool start, bool vertical);
	void AnimateHorizontalSegment(bool start, bool horizontal);
	void AnimateMidZ(bool start, bool midZ);

public:
	Matrix4 rotationMatrix;
	Logic* logic;
	float angle;
	Cube** cubes;

	struct Animation {
		float length;
		float timeElapsed;
		int side;
		float startAngle;
		float targetAngle;
		bool running;
		bool isVertical;
		bool isHorizontal;
		bool isMidZ;
	} m_currentAnimation;
};