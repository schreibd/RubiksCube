#include <ctrl.h>
#include "InputSystem.h"
#include "Animator.h"



InputSystem::InputSystem(Animator *animator)
{
	this->animator = animator;
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG_WIDE);
}

InputSystem::~InputSystem()
{

}

void InputSystem::Update(){
	sceCtrlReadBufferPositive(0, &result, 1);
	if(checkButtonCross() && !animator->m_currentAnimation.running)
		animator->AnimateVerticalSegment(true, true);
	if(checkButtonTriangle()&& !animator->m_currentAnimation.running)
		animator->AnimateHorizontalSegment(true, true);
	if(checkButtonQuad() && !animator->m_currentAnimation.running)
		animator->rotateSelectedSide(true, Logic::SIDE_LEFT);
	if(checkButtonCircle()&& !animator->m_currentAnimation.running)
		animator->rotateSelectedSide(true, Logic::SIDE_RIGHT);
	if(checkButtonUp()&& !animator->m_currentAnimation.running)
		animator->rotateSelectedSide(true, Logic::SIDE_BACK);
	if(checkButtonDown()&& !animator->m_currentAnimation.running)
		animator->rotateSelectedSide(true, Logic::SIDE_FRONT);
	if(checkButtonL()&& !animator->m_currentAnimation.running)
		animator->rotateSelectedSide(true, Logic::SIDE_TOP);
	if(checkButtonR()&& !animator->m_currentAnimation.running)
		animator->rotateSelectedSide(true, Logic::SIDE_BOTTOM);
	if(checkButtonLeftRight()&& !animator->m_currentAnimation.running)
		animator->AnimateMidZ(true, true);

	
}


//Front Rotation ist implementiert muss noch auf einen passenden Button gelegt werden
bool InputSystem::checkButtonCross(){
	if((result.buttons & SCE_CTRL_CROSS) == 0){
            return false;
        }
	else
		return true;
}

bool InputSystem::checkButtonQuad(){
	if((result.buttons & SCE_CTRL_SQUARE) == 0){
            return false;
        }
	else
		return true;
}

bool InputSystem::checkButtonCircle(){
	if((result.buttons & SCE_CTRL_CIRCLE) == 0){
            return false;
        }
	else
		return true;
}

bool InputSystem::checkButtonTriangle(){
	if((result.buttons & SCE_CTRL_TRIANGLE) == 0){
            return false;
        }
	else
		return true;
}

bool InputSystem::checkButtonUp(){
	if((result.buttons & SCE_CTRL_UP) == 0){
            return false;
        }
	else
		return true;
}

bool InputSystem::checkButtonDown(){
	if((result.buttons & SCE_CTRL_DOWN) == 0){
            return false;
        }
	else
		return true;
}

bool InputSystem::checkButtonLeftRight(){
	if((result.buttons & SCE_CTRL_LEFT || result.buttons & SCE_CTRL_RIGHT) == 0) 
		return false;
	else
		return true;

}

bool InputSystem::checkButtonL(){
	if((result.buttons & SCE_CTRL_L || result.buttons & SCE_CTRL_RIGHT) == 0) 
		return false;
	else
		return true;

}

bool InputSystem::checkButtonR(){
	if((result.buttons & SCE_CTRL_R || result.buttons & SCE_CTRL_RIGHT) == 0) 
		return false;
	else
		return true;

}
