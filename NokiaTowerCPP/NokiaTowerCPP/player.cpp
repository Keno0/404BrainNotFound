#include "player.h"

using namespace std;

//-------------------------------------------------------------------
// Write your own makeMove function.
//-------------------------------------------------------------------

enum States
{
	initState,
	growth,
	stagnation,
	decrease
};


int maxPop(int pop[MAP_SIZE][MAP_SIZE], int &x, int &y)
{
	int i = 0;
	int j = 0;
	double temp = 0;
	for (i = 0; i < MAP_SIZE; i++)
	{
		for (j = 0; j < MAP_SIZE; j++)
		{
			if (temp < pop[i][j] && (i != y && j != x))
			{
				temp = pop[i][j];
				x = j;
				y = i;
			}
		}
	}

	return temp;
}

double getDistance(int x, int y, int j, int k) {
	return std::sqrt(std::pow((x - j), 2) + std::pow(y - k, 2));
}

double maxPopTower(int towers[][2], bool towerMap[MAP_SIZE][MAP_SIZE], int &x, int &y) {

	int temp_x, temp_y = 0, ID = 0;
	double min_distance = 100000000;


	

	for(int i = 0; i < TOWER_MAX; i++)
	{
		if (min_distance > getDistance(x, y, towers[i][0], towers[i][1]))
		{
			min_distance = getDistance(x, y, towers[i][0], towers[i][1]);
			ID = i;
		}
		
	}

	cout << " min dist: " << min_distance << endl;

	return ID;
}

States determinateCurrentState(TinputData inputData, MoneyBuffer moneyBuffer)
{
	States state = initState;
	double economyChange = 0;

	cout << "inputData.header.money: " << inputData.header.money << endl;
	cout << "Last five month: " << moneyBuffer.AvarageOfLastFiveMonth() << endl;

	if (moneyBuffer.AvarageOfLastFiveMonth() == 0)
	{
		return state;
	}

	economyChange = (inputData.header.money / moneyBuffer.AvarageOfLastFiveMonth()) * 100;
	cout << "economyChange " << economyChange << endl;
	if (economyChange > 101.5)
		return growth;
	if (economyChange <= 101.5 || economyChange >= 98.5)
		return stagnation;
	if (economyChange < 98.5)
		return decrease;

	return state;
}


void TPlayer::makeMove() {

	int maxPopLocationX = 0;
	int maxPopLocationY = 0;
	States state = initState;

    map->MapNextTime(); // next population state

    cout << "time: " << inputData.header.time << " total pop:" << map->totalPop << endl;

    myTime++;
    outputData.invest = 0;
    outputData.numOrders = 0;

	playerMoneyBuffer.Add(inputData.header.money);
	if(inputData.header.time< 2)
	{
		state = initState;
	}
	else
	{
		state = determinateCurrentState(inputData, playerMoneyBuffer);
	}
		cout << " current state : " << state << endl;
		switch (state)
		{
		case initState:	
			maxPop(map->pop, maxPopLocationX, maxPopLocationY);
			rentTower(maxPopTower(map->towers, map->towerMap, maxPopLocationX, maxPopLocationY), 10, 35, 100);
			rentTower(124, 10, 35, 100);
			break;
		case growth:			
			outputData.invest = inputData.header.time + 100;
			break;
		case stagnation:
			outputData.invest = inputData.header.time;
			break;
		case decrease:
			leaveTower(124);
			break;
		default:
			break;
		}


		
        //if (inputData.header.time == 140) leaveTower(124);
    
}


