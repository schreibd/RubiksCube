#include "Side.h"
#include <iostream>

	//Erste Dimension = Zeile
	//Zweite Dimension = Sp
	Side::Side(){
	}

	Side::Side(int value, Color c){
		int counter = 0;

		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				faces[i][j].order = counter++;
				faces[i][j].color = c;
			}
		}

		logicalSide = value;
	}


	void Side::print(){
		std::cout << "\n--------------------------";
    for(int i = 0; i<3;i++){
		std::cout<<"\n";
        for(int j = 0; j<3;j++){                
            std::cout << faces[i][j].color;
        }
        std::cout<<"\n";
    }
    std::cout << "--------------------------";
	}

	uint32_t Side::getColorValue(Color c)
	{
		switch(c)
		{
			case RED: return 0xFF561743;
				break;
			case GREEN: return 0xFF65A826;
				break;
			case BLUE: return 0xFF689DFF;
				break;
			case PINK: return 0xFFFF8F8F;
				break;
			case YELLOW: return 0xFFFFFF00;
				break;
			case WHITE: return 0xFFFFFFFF;
				break;
			default:
				return 0x00000000;
		}
	}
