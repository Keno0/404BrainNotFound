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
	int maxPop = 0;
	for (i = 0; i < MAP_SIZE; i++)
	{
		for (j = 0; j < MAP_SIZE; j++)
		{
			if (maxPop < pop[i][j] && (i != y && j != x))
			{
				maxPop = pop[i][j];
				x = j;
				y = i;
			}
		}
	}

	return maxPop;
}

double getDistance(int x1, int y1, int x2, int y2) {
	return std::sqrt(std::pow((x1 - x2), 2) + std::pow(y1 - y2, 2));
}

int maxPopTower(int towers[][2], bool towerMap[MAP_SIZE][MAP_SIZE], int &x, int &y) {

	int ID = 0;
	double minDistance = 100000000;

	for(int i = 0; i < TOWER_MAX; i++)
	{
		if (minDistance > getDistance(x, y, towers[i][0], towers[i][1]))
		{
			minDistance = getDistance(x, y, towers[i][0], towers[i][1]);
			ID = i;
		}		
	}
	return ID;
}

States determinateCurrentState(TinputData inputData, MoneyBuffer moneyBuffer)
{
	States state = initState;
	double economyChange = 0;	

	if (inputData.header.time == 0)
	{
		return state;
	}

	economyChange = (inputData.header.money / moneyBuffer.AvarageOfLastFiveMonth()) * 100;
	cout << "-----------------------------------------" << endl;
	cout << "current MONEY: " << inputData.header.money << endl;
	cout << "average of last 5 months: " << moneyBuffer.AvarageOfLastFiveMonth() << endl;
	cout << "economyChange " << economyChange << " %" << endl << endl;
	cout << "-----------------------------------------" << endl;
	if (economyChange > 101.5)
		return growth;
	if (economyChange <= 101.5 && economyChange >= 98.5)
		return stagnation;
	else
		return decrease;

	return state;
}


void TPlayer::makeMove() {

	int maxPopLocationX = 0;
	int maxPopLocationY = 0;
	States state = initState;
	int leave = 0;

    map->MapNextTime(); // next population state

    cout << "time: " << inputData.header.time << " total population:" << map->totalPop << endl;
	cout << "Tower 93 Cust: " << inputData.towerInf[93].cust << endl;

    myTime++;
    outputData.invest = 0;
    outputData.numOrders = 0;

	

	AddTowersIfItsOur();
	money = inputData.header.money;
 
	int i = 0;
	double currentRentingCost = 0;
	double distance = 0;
	int maxTowerBuyInInitState = 0;

	state = determinateCurrentState(inputData, playerMoneyBuffer);	//ezt a kettot nem kéne felcserelni?
	playerMoneyBuffer.Add(inputData.header.money);					//???
	switch (state)
	{
		case initState:
			while ((money > (inputData.header.money*0.95)) && (i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE)) && (maxTowerBuyInInitState< MAX_TOWER_BUY_IN_INIT_STATE))
			{			
				if ((inputData.towerInf[magicMap.population_with_tower_id[i][1]].owner == 0) && magicMap.population_with_tower_id[i][0]>DEFAULT_POPULATION)
				{
					rentTower(	magicMap.population_with_tower_id[i][1],
								DEFAULT_RENTING_COST,
								10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[i][0],
								30
							 );
					maxTowerBuyInInitState++;
					money -= DEFAULT_RENTING_COST;
				}
				i++;				
			}
			break;

		case growth:

			GrowthStateLevel1();
			

			break;

		case stagnation:
			/*while (	money > inputData.header.money*0.95 &&
					(i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE)) &&
					inputData.header.money > SAFETY_MONEY)
			{
				if (inputData.towerInf[magicMap.population_with_tower_id[i][1]].owner == ID &&
					inputData.towerInf[magicMap.population_with_tower_id[i][1]].licitID != 0 &&
					inputData.towerInf[magicMap.population_with_tower_id[i][1]].licitDelay < 3)
				{
					if (inputData.towerInf[magicMap.population_with_tower_id[i][1]].licit > 0)
						currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[i][1]].licit + 1;
					else
						currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[i][1]].rentingCost + 1;

					double offer = CalculateOffer(	10 + DISTRICT_SIZE*0.00000015* magicMap.population_with_tower_id[i][0],
													currentRentingCost,
													magicMap.population_with_tower_id[i][0]);

					double maximumRentingCost = CalculateMaximumPriceOfRent(magicMap.population_with_tower_id[i][0],
																			distance,
																			offer);
					if (maximumRentingCost > currentRentingCost) // skip towers which are not profitable
					{
						rentTower(	magicMap.population_with_tower_id[i][1],
									currentRentingCost,
									distance,
									offer);
						money -= currentRentingCost;
					}
				}
				i++;
			}
			if (inputData.towerInf[playerTowers.playerTowerIndexes[playerTowers.actualPosition][0]].techLevel < 5)
				outputData.invest = playerMoneyBuffer.AvarageOfLastFiveMonth()*0.05;*/
			break;
		case decrease:
			break;
		default:
			break;
	}

	playerTowers.UpdateTowerData(inputData);
	LeaveTowersFromBlackList();
		HandleCostumerChange();
		HandleTowerDistance();
		  
}



