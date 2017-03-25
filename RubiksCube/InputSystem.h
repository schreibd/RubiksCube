#pragma once
#include <ctrl.h>
#include <iostream>
#include "Animator.h"

class InputSystem
{
public: 
	InputSystem(Animator *animator);
	~InputSystem(void);
	void Update();
	bool checkButtonCross();
	bool checkButtonTriangle();
	bool checkButtonQuad();
	bool checkButtonCircle();
	bool checkButtonUp();
	bool checkButtonDown();
	bool checkButtonLeftRight();
	bool checkButtonL();
	bool checkButtonR();

public:
	SceCtrlData result;
	Animator *animator;
};