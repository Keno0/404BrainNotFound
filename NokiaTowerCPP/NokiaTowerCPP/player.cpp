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


class MagicMap {
	int population[MAP_SIZE][MAP_SIZE];
	int towers[TOWER_MAX][2];
public:
	void set_values(int [MAP_SIZE][MAP_SIZE], int [TOWER_MAX][2]);
	void give_me_my_magic_map(int *[MAP_SIZE][MAP_SIZE]);
	void give_me_my_tower_population(int *[MAP_SIZE/30][MAP_SIZE/30], int *[][2], int *[(MAP_SIZE / 30)*(MAP_SIZE / 30)][2]);

};

void MagicMap::set_values(int pop[MAP_SIZE][MAP_SIZE], int tow[TOWER_MAX][2]) {
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			population[i][j] = pop[i][j];
		}
	}

	for (int i = 0; i < TOWER_MAX; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			towers[i][j] = tow[i][j];
		}
	}
}

void MagicMap::give_me_my_magic_map(int *population[MAP_SIZE][MAP_SIZE]) {

	int magicMap[MAP_SIZE / 30][MAP_SIZE / 30];


	for (int magic_x = 0; magic_x < MAP_SIZE/30; magic_x++)
	{
		for (int magic_y = 0; magic_y < MAP_SIZE / 30; magic_y++)
		{	
			for (int pop_x = magic_x *30; pop_x < magic_x *30 + 30; pop_x++)
			{
				for (int pop_y = magic_y *30; pop_y < magic_y *30 + 30; pop_y++)
				{
					magicMap[magic_x][magic_y] += (int) population[pop_x][pop_y];
				}
			}
		}
		
	}
}

void MagicMap::give_me_my_tower_population(int *magicMap[MAP_SIZE/30][MAP_SIZE/30], int *towers[][2], int *population_with_tower_id[(MAP_SIZE/30)*(MAP_SIZE/30)][2]) {
	int bigmap_district_center_x = 0;
	int bigmap_district_center_y = 0;
	int tower_ID = 0;


	/* get tower ID for all districts */
	for (int magic_x = 0; magic_x < MAP_SIZE/30 ; magic_x++)
	{
		for (int magic_y = 0; magic_y < MAP_SIZE/30; magic_y++)
		{
			bigmap_district_center_x = 15 + 30 * magic_x;
			bigmap_district_center_y = 15 + 30 * magic_y;
			double min_distance = 10000;

			/* get nearest tower to district center */
			for (int i = 0; i < TOWER_MAX; i++)
			{
				if (min_distance > getDistance(bigmap_district_center_x, bigmap_district_center_y, *towers[i][0], *towers[i][1]))
				{
					min_distance = getDistance(bigmap_district_center_x, bigmap_district_center_y, *towers[i][0], *towers[i][1]);
					tower_ID = i;
				}
			}

			*population_with_tower_id[magic_x * (MAP_SIZE / 30) + magic_y][0] = *magicMap[magic_x][magic_y]; // population in district
			*population_with_tower_id[magic_x * (MAP_SIZE / 30) + magic_y][1] = tower_ID;					 // nearest tower id
		}
	}
}


