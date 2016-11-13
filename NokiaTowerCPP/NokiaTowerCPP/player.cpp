#include "player.h"

using namespace std;

//-------------------------------------------------------------------
// Write your own makeMove function.
//-------------------------------------------------------------------


int maxPop(int pop[MAP_SIZE][MAP_SIZE], int &x, int &y)
{
	int i = 0;
	int j = 0;
	int temp = 0;
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

int maxPopTower(int towers[][2], bool towerMap[MAP_SIZE][MAP_SIZE], int &x, int &y) {

	int ID = 0;
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


void TPlayer::makeMove() {

	int maxPopLocationX = 0;
	int maxPopLocationY = 0;

    map->MapNextTime(); // next population state

    cout << "time: " << inputData.header.time << " total pop:" << map->totalPop << endl;

    myTime++;
    outputData.invest = 0;
    outputData.numOrders = 0;

    if (scriptName.size() > 0) {
        outputData.invest = IScript[myTime];
        outputData.numOrders = 0;
        while ((OMut <= OMax) && (myTime >= Times[OMut])) {
            if (myTime == Times[OMut]) {
                outputData.orders[outputData.numOrders].towerID = OScript[OMut].towerID;
                outputData.orders[outputData.numOrders].rentingCost = OScript[OMut].rentingCost;
                outputData.orders[outputData.numOrders].distance = OScript[OMut].distance;
                outputData.orders[outputData.numOrders].offer = OScript[OMut].offer;
                outputData.orders[outputData.numOrders].leave = OScript[OMut].leave;
                outputData.numOrders++;
            }
            OMut++;
        }
    }
    else {

		cout << "money:" << inputData.header.money << endl;
		if (inputData.header.time == 1) {
			rentTower(27,7.1, 3, 1);
			rentTower(28, 8.01, 4, 1);
			rentTower(29, 9.001, 5, 1);
			rentTower(30, 10.0001, 6, 1);
			
		}
        if (inputData.header.time == 5) changeDistanceAndOffer(124, 20, 110);
        if (inputData.header.time > 10 && inputData.header.time < 220) outputData.invest = inputData.header.money * 0.08;
		if (inputData.header.time == 10) 
		{
			cout << "time: " << inputData.header.time << " max pop:" << maxPop(map->pop,maxPopLocationX, maxPopLocationY ) << endl;
			cout << " maxPopLocationX: " << maxPopLocationX << " maxPopLocationY: " << maxPopLocationY;
			rentTower(maxPopTower(map->towers,map->towerMap, maxPopLocationX, maxPopLocationY), 10, 35, 100);

		}

		if (inputData.header.time == 20)
		{
			cout << "time: " << inputData.header.time << " max pop:" << maxPop(map->pop, maxPopLocationX, maxPopLocationY) << endl;
			cout << " maxPopLocationX: " << maxPopLocationX << " maxPopLocationY: " << maxPopLocationY;
			rentTower(maxPopTower(map->towers, map->towerMap, maxPopLocationX, maxPopLocationY), 10, 35, 100);

		}
		
        //if (inputData.header.time == 140) leaveTower(124);
    }
}



