#include "Logic.h"
#include <iostream>

#include "Side.h"

Logic::Logic()
{

	for(int i = 0; i <= 5; i++)
	{
		sides[i] = new Side(i, (Color)i);
	}
}

Logic::~Logic()
{
	for(int i = 0; i <= 5; i++)
		delete sides[i];
}

void Logic::RotateSelectedFace(int value){

	face temp[3][3];

	//Erstelle Kopie der Seite, die rotiert wird
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			temp[i][j] = sides[value]->faces[i][j];
		}
	}

	if(value == SIDE_BOTTOM){
	sides[value]->faces[0][2] = temp[2][2];
	sides[value]->faces[0][1] = temp[1][2];
	sides[value]->faces[0][0] = temp[0][2];

	//Zweite Zeile rotiert
	sides[value]->faces[1][0] = temp[0][1];
	sides[value]->faces[1][2] = temp[2][1];

	//Dritte Zeile rotiert
	sides[value]->faces[2][0] = temp[0][0];
	sides[value]->faces[2][1] = temp[1][0];
	sides[value]->faces[2][2] = temp[2][0];
	}
	else if(value == SIDE_BACK){
	sides[value]->faces[0][0] = temp[0][2];
	sides[value]->faces[0][1] = temp[1][2];
	sides[value]->faces[0][2] = temp[2][2];

	//Zweite Zeile rotiert
	sides[value]->faces[1][0] = temp[0][1];
	sides[value]->faces[1][2] = temp[2][1];

	//Dritte Zeile rotiert
	sides[value]->faces[2][0] = temp[0][0];
	sides[value]->faces[2][1] = temp[1][0];
	sides[value]->faces[2][2] = temp[2][0];
	}
	else
	{
	//Erste Zeile rotiert
	sides[value]->faces[0][2] = temp[0][0];
	sides[value]->faces[0][1] = temp[1][0];
	sides[value]->faces[0][0] = temp[2][0];

	//Zweite Zeile rotiert
	sides[value]->faces[1][0] = temp[2][1];
	sides[value]->faces[1][2] = temp[0][1];

	//Dritte Zeile rotiert
	sides[value]->faces[2][0] = temp[2][2];
	sides[value]->faces[2][1] = temp[1][2];
	sides[value]->faces[2][2] = temp[0][2];
	}
}

void Logic::RotateFront()
{
	RotateSelectedFace(SIDE_FRONT);
	
	face temp[3];

	temp[0] = sides[SIDE_TOP]->faces[2][0];
	temp[1] = sides[SIDE_TOP]->faces[2][1];
	temp[2] = sides[SIDE_TOP]->faces[2][2];

	sides[SIDE_TOP]->faces[2][0] = sides[SIDE_LEFT]->faces[2][2];
	sides[SIDE_TOP]->faces[2][1] = sides[SIDE_LEFT]->faces[1][2];
	sides[SIDE_TOP]->faces[2][2] = sides[SIDE_LEFT]->faces[0][2];

	sides[SIDE_LEFT]->faces[0][2] = sides[SIDE_BOTTOM]->faces[0][0];
	sides[SIDE_LEFT]->faces[1][2] = sides[SIDE_BOTTOM]->faces[0][1];
	sides[SIDE_LEFT]->faces[2][2] = sides[SIDE_BOTTOM]->faces[0][2];

	sides[SIDE_BOTTOM]->faces[0][0] = sides[SIDE_RIGHT]->faces[2][2];
	sides[SIDE_BOTTOM]->faces[0][1] = sides[SIDE_RIGHT]->faces[1][2];
	sides[SIDE_BOTTOM]->faces[0][2] = sides[SIDE_RIGHT]->faces[0][2];

	sides[SIDE_RIGHT]->faces[0][2] = temp[0];
	sides[SIDE_RIGHT]->faces[1][2] = temp[1];
	sides[SIDE_RIGHT]->faces[2][2] = temp[2];
}

void Logic::RotateBack(){

	RotateSelectedFace(SIDE_BACK);

	face temp[3];

	temp[0] = sides[SIDE_TOP]->faces[0][0];
	temp[1] = sides[SIDE_TOP]->faces[0][1];
	temp[2] = sides[SIDE_TOP]->faces[0][2];

	sides[SIDE_TOP]->faces[0][0] = sides[SIDE_LEFT]->faces[2][0];
	sides[SIDE_TOP]->faces[0][1] = sides[SIDE_LEFT]->faces[1][0];
	sides[SIDE_TOP]->faces[0][2] = sides[SIDE_LEFT]->faces[0][0];
	
	sides[SIDE_LEFT]->faces[0][0] = sides[SIDE_BOTTOM]->faces[2][0];
	sides[SIDE_LEFT]->faces[1][0] = sides[SIDE_BOTTOM]->faces[2][1];
	sides[SIDE_LEFT]->faces[2][0] = sides[SIDE_BOTTOM]->faces[2][2];

	sides[SIDE_BOTTOM]->faces[2][0] = sides[SIDE_RIGHT]->faces[2][0];
	sides[SIDE_BOTTOM]->faces[2][1] = sides[SIDE_RIGHT]->faces[1][0];
	sides[SIDE_BOTTOM]->faces[2][2] = sides[SIDE_RIGHT]->faces[0][0];

	sides[SIDE_RIGHT]->faces[0][0] = temp[0];
	sides[SIDE_RIGHT]->faces[1][0] = temp[1];
	sides[SIDE_RIGHT]->faces[2][0] = temp[2];
}

void Logic::RotateLeft(){
	RotateSelectedFace(SIDE_LEFT);
	face temp[3];

	temp[0] = sides[SIDE_TOP]->faces[0][0];
	temp[1] = sides[SIDE_TOP]->faces[1][0];
	temp[2] = sides[SIDE_TOP]->faces[2][0];

	sides[SIDE_TOP]->faces[0][0] = sides[SIDE_BACK]->faces[0][0];
	sides[SIDE_TOP]->faces[1][0] = sides[SIDE_BACK]->faces[1][0];
	sides[SIDE_TOP]->faces[2][0] = sides[SIDE_BACK]->faces[2][0];

	sides[SIDE_BACK]->faces[0][0] = sides[SIDE_BOTTOM]->faces[0][0];
	sides[SIDE_BACK]->faces[1][0] = sides[SIDE_BOTTOM]->faces[1][0];
	sides[SIDE_BACK]->faces[2][0] = sides[SIDE_BOTTOM]->faces[2][0];

	sides[SIDE_BOTTOM]->faces[0][0] = sides[SIDE_FRONT]->faces[0][0];
	sides[SIDE_BOTTOM]->faces[1][0] = sides[SIDE_FRONT]->faces[1][0];
	sides[SIDE_BOTTOM]->faces[2][0] = sides[SIDE_FRONT]->faces[2][0];

	sides[SIDE_FRONT]->faces[0][0] = temp[0];
	sides[SIDE_FRONT]->faces[1][0] = temp[1];
	sides[SIDE_FRONT]->faces[2][0] = temp[2];
}

void Logic::RotateRight(){
	RotateSelectedFace(SIDE_RIGHT);

	face temp[3];

	temp[0] = sides[SIDE_TOP]->faces[0][2];
	temp[1] = sides[SIDE_TOP]->faces[1][2];
	temp[2] = sides[SIDE_TOP]->faces[2][2];

	sides[SIDE_TOP]->faces[0][2] = sides[SIDE_BACK]->faces[0][2];
	sides[SIDE_TOP]->faces[1][2] = sides[SIDE_BACK]->faces[1][2];
	sides[SIDE_TOP]->faces[2][2] = sides[SIDE_BACK]->faces[2][2];

	sides[SIDE_BACK]->faces[0][2] = sides[SIDE_BOTTOM]->faces[0][2];
	sides[SIDE_BACK]->faces[1][2] = sides[SIDE_BOTTOM]->faces[1][2];
	sides[SIDE_BACK]->faces[2][2] = sides[SIDE_BOTTOM]->faces[2][2];

	sides[SIDE_BOTTOM]->faces[0][2] = sides[SIDE_FRONT]->faces[0][2];
	sides[SIDE_BOTTOM]->faces[1][2] = sides[SIDE_FRONT]->faces[1][2];
	sides[SIDE_BOTTOM]->faces[2][2] = sides[SIDE_FRONT]->faces[2][2];

	sides[SIDE_FRONT]->faces[0][2] = temp[0];
	sides[SIDE_FRONT]->faces[1][2] = temp[1];
	sides[SIDE_FRONT]->faces[2][2] = temp[2];
}

void Logic::RotateVerticalCenter(){
	face temp[3];

	temp[0] = sides[SIDE_TOP]->faces[0][1];
	temp[1] = sides[SIDE_TOP]->faces[1][1];
	temp[2] = sides[SIDE_TOP]->faces[2][1];

	sides[SIDE_TOP]->faces[0][1] = sides[SIDE_BACK]->faces[0][1];
	sides[SIDE_TOP]->faces[1][1] = sides[SIDE_BACK]->faces[1][1];
	sides[SIDE_TOP]->faces[2][1] = sides[SIDE_BACK]->faces[2][1];

	sides[SIDE_BACK]->faces[0][1] = sides[SIDE_BOTTOM]->faces[0][1];
	sides[SIDE_BACK]->faces[1][1] = sides[SIDE_BOTTOM]->faces[1][1];
	sides[SIDE_BACK]->faces[2][1] = sides[SIDE_BOTTOM]->faces[2][1];

	sides[SIDE_BOTTOM]->faces[0][1] = sides[SIDE_FRONT]->faces[0][1];
	sides[SIDE_BOTTOM]->faces[1][1] = sides[SIDE_FRONT]->faces[1][1];
	sides[SIDE_BOTTOM]->faces[2][1] = sides[SIDE_FRONT]->faces[2][1];

	sides[SIDE_FRONT]->faces[0][1] = temp[0];
	sides[SIDE_FRONT]->faces[1][1] = temp[1];
	sides[SIDE_FRONT]->faces[2][1] = temp[2];
}

void Logic::RotateMidZ(){
	face temp[3];

	temp[0] = sides[SIDE_TOP]->faces[1][0];
	temp[1] = sides[SIDE_TOP]->faces[1][1];
	temp[2] = sides[SIDE_TOP]->faces[1][2];

	sides[SIDE_TOP]->faces[1][0] = sides[SIDE_LEFT]->faces[2][1];
	sides[SIDE_TOP]->faces[1][1] = sides[SIDE_LEFT]->faces[1][1];
	sides[SIDE_TOP]->faces[1][2] = sides[SIDE_LEFT]->faces[0][1];

	sides[SIDE_LEFT]->faces[0][1] = sides[SIDE_BOTTOM]->faces[1][0];
	sides[SIDE_LEFT]->faces[1][1] = sides[SIDE_BOTTOM]->faces[1][1];
	sides[SIDE_LEFT]->faces[2][1] = sides[SIDE_BOTTOM]->faces[1][2];

	sides[SIDE_BOTTOM]->faces[1][0] = sides[SIDE_RIGHT]->faces[2][1];
	sides[SIDE_BOTTOM]->faces[1][1] = sides[SIDE_RIGHT]->faces[1][1];
	sides[SIDE_BOTTOM]->faces[1][2] = sides[SIDE_RIGHT]->faces[0][1];

	sides[SIDE_RIGHT]->faces[0][1] = temp[0];
	sides[SIDE_RIGHT]->faces[1][1] = temp[1];
	sides[SIDE_RIGHT]->faces[2][1] = temp[2];
}

void Logic::RotateTop(){
	RotateSelectedFace(SIDE_TOP);

	face temp[3];

	temp[0] = sides[SIDE_FRONT]->faces[0][0];
	temp[1] = sides[SIDE_FRONT]->faces[0][1];
	temp[2] = sides[SIDE_FRONT]->faces[0][2];

	sides[SIDE_FRONT]->faces[0][0] = sides[SIDE_RIGHT]->faces[0][2];
	sides[SIDE_FRONT]->faces[0][1] = sides[SIDE_RIGHT]->faces[0][1];
	sides[SIDE_FRONT]->faces[0][2] = sides[SIDE_RIGHT]->faces[0][0];

	sides[SIDE_RIGHT]->faces[0][0] = sides[SIDE_BACK]->faces[2][0];
	sides[SIDE_RIGHT]->faces[0][1] = sides[SIDE_BACK]->faces[2][1];
	sides[SIDE_RIGHT]->faces[0][2] = sides[SIDE_BACK]->faces[2][2];
	
	sides[SIDE_BACK]->faces[2][0] = sides[SIDE_LEFT]->faces[0][2];
	sides[SIDE_BACK]->faces[2][1] = sides[SIDE_LEFT]->faces[0][1];
	sides[SIDE_BACK]->faces[2][2] = sides[SIDE_LEFT]->faces[0][0];

	sides[SIDE_LEFT]->faces[0][0] = temp[0];
	sides[SIDE_LEFT]->faces[0][1] = temp[1];
	sides[SIDE_LEFT]->faces[0][2] = temp[2];
}

void Logic::RotateBottom(){
	RotateSelectedFace(SIDE_BOTTOM);

	face temp[3];

	temp[0] = sides[SIDE_FRONT]->faces[2][0];
	temp[1] = sides[SIDE_FRONT]->faces[2][1];
	temp[2] = sides[SIDE_FRONT]->faces[2][2];

	sides[SIDE_FRONT]->faces[2][0] = sides[SIDE_RIGHT]->faces[2][2];
	sides[SIDE_FRONT]->faces[2][1] = sides[SIDE_RIGHT]->faces[2][1];
	sides[SIDE_FRONT]->faces[2][2] = sides[SIDE_RIGHT]->faces[2][0];

	sides[SIDE_RIGHT]->faces[2][0] = sides[SIDE_BACK]->faces[0][0];
	sides[SIDE_RIGHT]->faces[2][1] = sides[SIDE_BACK]->faces[0][1];
	sides[SIDE_RIGHT]->faces[2][2] = sides[SIDE_BACK]->faces[0][2];

	sides[SIDE_BACK]->faces[0][0] = sides[SIDE_LEFT]->faces[2][2];
	sides[SIDE_BACK]->faces[0][1] = sides[SIDE_LEFT]->faces[2][1];
	sides[SIDE_BACK]->faces[0][2] = sides[SIDE_LEFT]->faces[2][0];

	sides[SIDE_LEFT]->faces[2][0] = temp[0];
	sides[SIDE_LEFT]->faces[2][1] = temp[1];
	sides[SIDE_LEFT]->faces[2][2] = temp[2];												  
}
//KORREKT
void Logic::RotateHorizontalCenter(){
	face temp[3];
	//Korrekt
	temp[0] = sides[SIDE_FRONT]->faces[1][0];
	temp[1] = sides[SIDE_FRONT]->faces[1][1];
	temp[2] = sides[SIDE_FRONT]->faces[1][2];
	//Korrekt
	sides[SIDE_FRONT]->faces[1][0] = sides[SIDE_RIGHT]->faces[1][2];
	sides[SIDE_FRONT]->faces[1][1] = sides[SIDE_RIGHT]->faces[1][1];
	sides[SIDE_FRONT]->faces[1][2] = sides[SIDE_RIGHT]->faces[1][0];

	//Korrekt
	sides[SIDE_RIGHT]->faces[1][0] = sides[SIDE_BACK]->faces[1][0];
	sides[SIDE_RIGHT]->faces[1][1] = sides[SIDE_BACK]->faces[1][1];
	sides[SIDE_RIGHT]->faces[1][2] = sides[SIDE_BACK]->faces[1][2];

	//KORREKT
	sides[SIDE_BACK]->faces[1][0] = sides[SIDE_LEFT]->faces[1][2];
	sides[SIDE_BACK]->faces[1][1] = sides[SIDE_LEFT]->faces[1][1];
	sides[SIDE_BACK]->faces[1][2] = sides[SIDE_LEFT]->faces[1][0];

	//KORREKT
	sides[SIDE_LEFT]->faces[1][0] = temp[0];
	sides[SIDE_LEFT]->faces[1][1] = temp[1];
	sides[SIDE_LEFT]->faces[1][2] = temp[2]; 
}