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

double CalculateOffer(int distance,int rentingCost, int disctrictCustomer)
{
	return ((1 + PLAN_PROFIT)*(distance*distance*0.04 + rentingCost) / (disctrictCustomer*PROFIT_PER_CUSTOMER*PREDICT_OF_CUSTUMER_OF_A_TOWER));
}



double getDistanceForRent(int population) {
	return 10 + DISTRICT_SIZE*0.000001* population;
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

	state=determinateCurrentState(inputData, playerMoneyBuffer);

	AddTowersIfItsOur();
	

	int tempPop = 150000;
	int money = inputData.header.money;
	int rentingCost = 7;
	int i = 0;
	int randomTowerID = 0;

	//for (int i = 0; i < playerTowers.actualPositionOfBlackList; i++)
	//{
	//	if (playerTowers.playerBlackListTower[i][1] == 1)
	//	{
	//		leaveTower(playerTowers.playerBlackListTower[i][0]);
	//		playerTowers.playerBlackListTower[i][1] = 0;
	//	}
	//}



	playerMoneyBuffer.Add(inputData.header.money);
		switch (state)
		{
		case initState:	
			
			while (money > (inputData.header.money*0.95) && (i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE)))
			{			
				randomTowerID = magicMap.population_with_tower_id[i][1] + 1;
				if ((inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].owner == 0) && magicMap.population_with_tower_id[randomTowerID][0]>tempPop)
				{
					rentTower(magicMap.population_with_tower_id[randomTowerID][1], rentingCost, 10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[randomTowerID][0],
						CalculateOffer(10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[randomTowerID][0],rentingCost, magicMap.population_with_tower_id[randomTowerID][0]));
					money -= rentingCost;
					inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].owner = ID;
				}
				i++;
				
				
			}
			break;
		case growth:

			while (money > inputData.header.money*0.95 && (i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE)))
			{
				randomTowerID = i;
				double distance = getDistanceForRent(magicMap.population_with_tower_id[randomTowerID][0]);

				if ((inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].owner == 0) && magicMap.population_with_tower_id[randomTowerID][0]>tempPop)
				{
					rentTower(magicMap.population_with_tower_id[randomTowerID][1], rentingCost, distance,
						CalculateOffer(10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[randomTowerID][0], rentingCost, magicMap.population_with_tower_id[randomTowerID][0]));
					money -= rentingCost;
					cout << "tower pop: " << magicMap.population_with_tower_id[randomTowerID][0] << endl;
					inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].owner = ID;
				}
				else if (inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].licitID != ID  && magicMap.population_with_tower_id[randomTowerID][0]>tempPop) //not our tower
				{
					//worth to overlicit?
					double currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].licit;
					if (currentRentingCost < 7)
					{
						currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[randomTowerID][1]].rentingCost;
					}
					
					double offer = CalculateOffer(10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[randomTowerID][0], rentingCost, magicMap.population_with_tower_id[randomTowerID][0]);
					double maximumRentingCost = 100;/*CalculateMaximumPriceOfRent(magicMap.population_with_tower_id[randomTowerID][1], distance, offer);*/
					double ourOfferForRenting = currentRentingCost + 1;

					cout << "ourOfferForRenting" << ourOfferForRenting << endl;
					if (200 > currentRentingCost) // skip towers which are not profitable
					{
						cout << "licit, tower ID: " << magicMap.population_with_tower_id[randomTowerID][1] << endl;
						rentTower(magicMap.population_with_tower_id[randomTowerID][1], ourOfferForRenting, distance, offer);
						money -= ourOfferForRenting*3;
					}				
				}
				i++;
			}

			if(inputData.towerInf[playerTowers.playerTowerIndexes[playerTowers.actualPosition][0]].techLevel < 5)
				outputData.invest = playerMoneyBuffer.AvarageOfLastFiveMonth()*0.15;
			break;
		case stagnation:
			if (inputData.towerInf[playerTowers.playerTowerIndexes[playerTowers.actualPosition][0]].techLevel < 5)
				outputData.invest = playerMoneyBuffer.AvarageOfLastFiveMonth()*0.05;
			break;
		case decrease:
			break;
		default:
			break;
		}

		HandleCostumerChange();
		playerTowers.UpdateTowerData(inputData);
        //if (inputData.header.time == 140) leaveTower(124);
    
}



