#include "Animator.h"
#include "Logic.h"
#include "Cube.h"


#define M_PI 3.14159265359f

Animator::Animator(Cube** cubes, Logic *logic) : rotationMatrix(Matrix4::identity())
{
	m_currentAnimation.running = false;
	m_currentAnimation.length = 1.0f;
	m_currentAnimation.startAngle = 0.0f;
	m_currentAnimation.targetAngle = M_PI * 0.5f;
	m_currentAnimation.timeElapsed = 0.0f;
	this->cubes = cubes;
	this->logic = logic;
	m_currentAnimation.isVertical = false;
	m_currentAnimation.isHorizontal = false;
}


Animator::~Animator()
{
	
}

void Animator::rotateSelectedSide(bool start, int side){
	
	m_currentAnimation.side = side;
	m_currentAnimation.running = true;	
}

void Animator::AnimateVerticalSegment(bool start, bool vertical){
	m_currentAnimation.isVertical = vertical;
	m_currentAnimation.running = true;
}

void Animator::AnimateHorizontalSegment(bool start, bool horizontal){
	m_currentAnimation.isHorizontal = horizontal;
	m_currentAnimation.running = true;
}

void Animator::AnimateMidZ(bool start, bool midZ){
	m_currentAnimation.isMidZ = midZ;
	m_currentAnimation.running = true;
}

void Animator::update(float delta)
{
	Matrix4 animationTransform;
	if (m_currentAnimation.running)
	{
		float t = m_currentAnimation.timeElapsed / m_currentAnimation.length;
		float targetAngle = m_currentAnimation.targetAngle * t + (1.0f - t) * m_currentAnimation.startAngle;

		

		if (m_currentAnimation.side == Logic::SIDE_FRONT && !m_currentAnimation.isVertical && !m_currentAnimation.isHorizontal && !m_currentAnimation.isMidZ)
		{
			animationTransform = Matrix4::rotationZ(targetAngle);
				for(int i = 0; i < 9; i++) {
					cubes[i]->animationTransformation = animationTransform;
				}
		}

		if (m_currentAnimation.isMidZ && m_currentAnimation.running)
		{
			animationTransform = Matrix4::rotationZ(targetAngle);
				for(int i = 9; i < 18; i++) {
					cubes[i]->animationTransformation = animationTransform;
				}

		}


		
		if(m_currentAnimation.side == Logic::SIDE_BACK && !m_currentAnimation.isMidZ)
		{
			animationTransform = Matrix4::rotationZ(targetAngle);
			for(int i = 18; i <= 26; i++) {
				cubes[i]->animationTransformation = animationTransform;
			}
		}

		if(m_currentAnimation.side == Logic::SIDE_LEFT)
		{
			animationTransform = Matrix4::rotationX(targetAngle);
			for(int i = 0; i <= 8; i++) {
				cubes[i*3]->animationTransformation = animationTransform;
			}
		}

		if(m_currentAnimation.side == Logic::SIDE_RIGHT)
		{
			animationTransform = Matrix4::rotationX(targetAngle);
			for(int i = 0; i <= 8; i++)
				cubes[i*3+2]->animationTransformation = animationTransform;
		}

		if(m_currentAnimation.isVertical && m_currentAnimation.running){
			animationTransform = Matrix4::rotationX(targetAngle);
			for(int i = 1; i < 26; i += 3)
				cubes[i]->animationTransformation = animationTransform;
		}

		if(m_currentAnimation.side == Logic::SIDE_TOP)
		{
			animationTransform = Matrix4::rotationY(targetAngle);
			cubes[0]->animationTransformation = animationTransform;
			cubes[1]->animationTransformation = animationTransform;
			cubes[2]->animationTransformation = animationTransform;

			cubes[9]->animationTransformation = animationTransform;
			cubes[10]->animationTransformation = animationTransform;
			cubes[11]->animationTransformation = animationTransform;

			cubes[18]->animationTransformation = animationTransform;
			cubes[19]->animationTransformation = animationTransform;
			cubes[20]->animationTransformation = animationTransform;
		}

		if(m_currentAnimation.side == Logic::SIDE_BOTTOM)
		{
			animationTransform = Matrix4::rotationY(targetAngle);
			cubes[6]->animationTransformation = animationTransform;
			cubes[7]->animationTransformation = animationTransform;
			cubes[8]->animationTransformation = animationTransform;

			cubes[15]->animationTransformation = animationTransform;
			cubes[16]->animationTransformation = animationTransform;
			cubes[17]->animationTransformation = animationTransform;

			cubes[24]->animationTransformation = animationTransform;
			cubes[25]->animationTransformation = animationTransform;
			cubes[26]->animationTransformation = animationTransform;
		}

		if(m_currentAnimation.isHorizontal && m_currentAnimation.running){
			animationTransform = Matrix4::rotationY(targetAngle);

			cubes[3]->animationTransformation = animationTransform;
			cubes[4]->animationTransformation = animationTransform;
			cubes[5]->animationTransformation = animationTransform;

			cubes[12]->animationTransformation = animationTransform;
			cubes[13]->animationTransformation = animationTransform;
			cubes[14]->animationTransformation = animationTransform;

			cubes[21]->animationTransformation = animationTransform;
			cubes[22]->animationTransformation = animationTransform;
			cubes[23]->animationTransformation = animationTransform;

		}

		m_currentAnimation.timeElapsed += delta;

		if (m_currentAnimation.timeElapsed > m_currentAnimation.length)
		{
			updateLogic();
			m_currentAnimation.running = false;
			m_currentAnimation.isVertical = false;
			m_currentAnimation.isHorizontal = false;
			m_currentAnimation.isMidZ = false;
			m_currentAnimation.timeElapsed = 0.0f;
			m_currentAnimation.side = NULL;


			for (int i=0; i < 27; ++i)
			{
				cubes[i]->animationTransformation = Matrix4::identity();
			}
		} 
	}
}

void Animator::updateLogic(){
	if(m_currentAnimation.isVertical)
		logic->RotateVerticalCenter();
	else if(m_currentAnimation.isHorizontal)
		logic->RotateHorizontalCenter(); 
	else if(m_currentAnimation.isMidZ)
		logic->RotateMidZ(); 
	else if(m_currentAnimation.side == Logic::SIDE_FRONT)
		logic->RotateFront();
	else if(m_currentAnimation.side == Logic::SIDE_LEFT)
		logic->RotateLeft();
	else if(m_currentAnimation.side == Logic::SIDE_RIGHT)
		logic->RotateRight();
	else if(m_currentAnimation.side == Logic::SIDE_BACK)
		logic->RotateBack();
	else if(m_currentAnimation.side == Logic::SIDE_TOP)
		logic->RotateTop();
	else if(m_currentAnimation.side == Logic::SIDE_BOTTOM)
		logic->RotateBottom();
}

//Rotieren
//Farben switchen
//Rotation rückgängig 

