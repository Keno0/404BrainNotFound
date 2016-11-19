#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <ctime>

#include "decl.h"
#include "map.h"

#define SIZE_OF_MONEY_BUFFER 20
#define PLAYER_TOWER_INDEXES 100
#define DISTRICT_SIZE 25  //Best around 20-25
#define CUSTOMER_CHANGES 10000
#define PLAN_PROFIT 1.4 // used for calculate the offer when try to rent a tower, 0.2 = 20% profit
#define PROFIT_PER_CUSTOMER 0.000001
#define PREDICT_OF_CUSTUMER_OF_A_TOWER 0.2 //20%-a district population-nek
#define DEFAULT_RENTING_COST 7
#define DEFAULT_POPULATION 150000 // ez alatti district population-n�l nem vesz�nk tornyot
#define SAFETY_MONEY 5000
#define MAX_CUSTOMER_AT_A_TOWER 500000
#define MIN_CUSTOMER_AT_A_TOWER 20000
#define AVERAGE_POPULATION_GROWTH 1.00015
#define MAX_TOWER_BUY 3
#define MAX_TOWER_BUY_IN_INIT_STATE 3
using namespace std;

class PlayerTowers
{
public:
	int playerTowerIndexes[PLAYER_TOWER_INDEXES][6]; //[i][0]: tower index, [i][1]: max costumer, [i][2]:maxprofit, [i][3]: current customer, [i][4]: current profit [i][5]: all earned profit
	int actualPosition=0, actualPositionOfBlackList=0, actualPositionOfLeavedTowers=0;
	int playerBlackListTower[PLAYER_TOWER_INDEXES][2]; //index, leaved
	int playerLeavedTower[PLAYER_TOWER_INDEXES][2]; //index, counter for rerent

	PlayerTowers()
	{
		for (int i = 0; i < PLAYER_TOWER_INDEXES; i++)
		{
			playerBlackListTower[i][0] = -1;
			playerBlackListTower[i][1] = -1;
			for (int j = 0; j < 6; j++)
			{
				playerTowerIndexes[i][j] = -1;
			}
		}			
	}


	void AddTowerToBlacklist(int towerID)
	{
		if (actualPositionOfBlackList < PLAYER_TOWER_INDEXES && !IsItTowerInBlackList(towerID))
		{
			playerBlackListTower[actualPositionOfBlackList][0] = towerID;
			playerBlackListTower[actualPositionOfBlackList][1] = 1;
			actualPositionOfBlackList++;
		}
		else if (actualPositionOfBlackList == PLAYER_TOWER_INDEXES)
			actualPositionOfBlackList = 0;
	}

	void Add(int towerID)
	{
		if (actualPosition < PLAYER_TOWER_INDEXES && !IsItOurTower(towerID))
		{
			playerTowerIndexes[actualPosition][0] = towerID;
			actualPosition++;
		}
		else if(actualPosition == PLAYER_TOWER_INDEXES)
			actualPosition = 0;
	}

	void AddToLeavedTowers(int towerID)
	{

		if (actualPositionOfLeavedTowers < PLAYER_TOWER_INDEXES && !IsItTowerInLeavedTowerList(towerID))
		{
			playerLeavedTower[actualPositionOfLeavedTowers][0] = towerID;
			playerLeavedTower[actualPositionOfLeavedTowers][1] = 10;
			actualPositionOfLeavedTowers++;
		}
		else if (actualPositionOfLeavedTowers == PLAYER_TOWER_INDEXES)
			actualPositionOfLeavedTowers = 0;
	}

	void UpdateLeavedTowersCounter()
	{
		for (int i = 0; i < actualPositionOfLeavedTowers; i++)
		{
			if (playerLeavedTower[i][1] != 0 && playerLeavedTower[i][0] != -1)
				playerLeavedTower[i][1]--;
			else if(playerLeavedTower[i][0] != -1)
			{
				playerLeavedTower[i][0] = -1;
				playerLeavedTower[i][1] = -1;
			}
		}
	}

	int CouterOfLeavedTower(int towerID)
	{
		for (int i = 0; i < actualPositionOfLeavedTowers; i++)
		{
			if (playerLeavedTower[i][0] == towerID)
				return playerLeavedTower[i][1];
		}
	}

	void Remove(int towerID)
	{
		for (int i = 0; i <= actualPosition; i++)
		{
			if (playerTowerIndexes[i][0] == towerID)
			{
				for(int j=0; j<6; j++)
					playerTowerIndexes[i][j] = -1;

			}
		}
	}

	void RemoveFromLeavedTower(int towerID)
	{
		for (int i = 0; i <= actualPositionOfLeavedTowers; i++)
		{
			if (playerLeavedTower[i][0] == towerID)
			{
				playerLeavedTower[i][0] = -1;
			    playerLeavedTower[i][1] = -1;

			}
		}
	}

	bool IsItOurTower(int towerID)
	{
		for (int i = 0; i <= actualPosition; i++)
		{
			if (playerTowerIndexes[i][0] == towerID)
				return true;
		}

		return false;
	}

	bool IsItTowerInBlackList(int towerID)
	{
		for (int i = 0; i <= actualPositionOfBlackList; i++)
		{
			if (playerBlackListTower[i][0] == towerID)
				return true;
		}

		return false;
	}

	bool IsItTowerInLeavedTowerList(int towerID)
	{
		for (int i = 0; i <= actualPositionOfLeavedTowers; i++)
		{
			if (playerLeavedTower[i][0] == towerID)
				return true;
		}

		return false;
	}

	void UpdateTowerData(TinputData inputData)
	{
		//[i][0]: tower index, [i][1]: max costumer, [i][2]:maxprofit, [i][3]: current customer, [i][4]: current profit
		for (int i = 0; i < actualPosition; i++)
		{
			if (playerTowerIndexes[i][0] != -1)
			{
				if (inputData.towerInf[playerTowerIndexes[i][0]].cust > playerTowerIndexes[i][1])
				{
					playerTowerIndexes[i][1] = inputData.towerInf[playerTowerIndexes[i][0]].cust;
				}

				//Ha nagyobb a v�ltoz�s mint a CUSTOMER CHANGES, akkor updatelj�k a playerTower customert
				if (inputData.towerInf[playerTowerIndexes[i][0]].cust + CUSTOMER_CHANGES > playerTowerIndexes[i][3])
					playerTowerIndexes[i][3] = inputData.towerInf[playerTowerIndexes[i][0]].cust;
				else if (inputData.towerInf[playerTowerIndexes[i][0]].cust - CUSTOMER_CHANGES < playerTowerIndexes[i][3])
					playerTowerIndexes[i][3] = inputData.towerInf[playerTowerIndexes[i][0]].cust;

				playerTowerIndexes[i][4] = TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]);
				if(playerTowerIndexes[i][5] <= 200)
					playerTowerIndexes[i][5] += TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]);
				else if(TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]) <= 0)
					playerTowerIndexes[i][5] += TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]);
				if (TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]) > playerTowerIndexes[i][2])
					playerTowerIndexes[i][2] = TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]);
				
				if (playerTowerIndexes[i][5] <= -100)
				{

					AddTowerToBlacklist(playerTowerIndexes[i][0]);
					Remove(playerTowerIndexes[i][0]);
				}

				
			}
		}

		UpdateLeavedTowersCounter();
	}

	int TowerProfit(TtowerInfRec towerInfo)
	{
		return towerInfo.cust * towerInfo.offer*PROFIT_PER_CUSTOMER - (towerInfo.rentingCost + towerInfo.runningCost);
	}
	
};

class MoneyBuffer
{
public:
	double moneyInLastMonths[SIZE_OF_MONEY_BUFFER];
	int lastIndex = 0;

	MoneyBuffer()
	{
		for (int i = 0; i < SIZE_OF_MONEY_BUFFER; i++)
		{
			moneyInLastMonths[i] = 0;
		}
	}

	void Add(int money)
	{
		moneyInLastMonths[lastIndex] = money;
		if (lastIndex >= SIZE_OF_MONEY_BUFFER-1)
			lastIndex = 0;
		else
			lastIndex++;
	}

	double AvarageOfLastFiveMonth()
	{
		double temp = 0;
		int divisor = 0;
		for (int i = 0; i < SIZE_OF_MONEY_BUFFER; i++)
		{
			if (moneyInLastMonths[i] != 0)
			{
				temp += moneyInLastMonths[i];
				divisor++;
			}
		}

		if (temp == 0 || divisor == 0)
			return 0;
		else
			return (double)(temp / divisor);
	}
};

class MagicMap {
public:
	int population[MAP_SIZE][MAP_SIZE];
	int towers[TOWER_MAX][2];
	int magicMap[MAP_SIZE / DISTRICT_SIZE][MAP_SIZE / DISTRICT_SIZE];
	int population_with_tower_id[(MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE)][2];
	bool debug = false;

	MagicMap()
	{
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				population[i][j] = 0;
			}
		}

		for (int i = 0; i < MAP_SIZE/ DISTRICT_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE/ DISTRICT_SIZE; j++)
			{
				magicMap[i][j] = 0;
			}
		}

		for (int i = 0; i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE); i++)
		{
			for (int j = 0; j < 2; j++)
			{
				population_with_tower_id[i][j] = 0;
			}
		}

		for (int i = 0; i < TOWER_MAX; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				towers[i][j] = 0;
			}
		}
	}

	double MagicMap::getDistance(int x, int y, int j, int k) {
		return std::sqrt(std::pow((x - j), 2) + std::pow(y - k, 2));
	}

	void MagicMap::setValues(int pop[MAP_SIZE][MAP_SIZE], int tow[TOWER_MAX][2]) {
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				//population[i][j] = pop[i][j];
				population[i][j] = pop[j][i];
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

	void MagicMap::giveMeMyMagicMap() {

		for (int magic_x = 0; magic_x < MAP_SIZE / DISTRICT_SIZE; magic_x++)
		{			
			for (int magic_y = 0; magic_y < MAP_SIZE / DISTRICT_SIZE; magic_y++)
			{
				for (int pop_x = magic_x * DISTRICT_SIZE; pop_x < magic_x * DISTRICT_SIZE + DISTRICT_SIZE; pop_x++)
				{
					for (int pop_y = magic_y * DISTRICT_SIZE; pop_y < magic_y * DISTRICT_SIZE + DISTRICT_SIZE; pop_y++)
					{
						magicMap[magic_x][magic_y] += (int)population[pop_x][pop_y];
					}

					if (debug)
						cout << "MagicMap population at x: " << magic_x << " , at y: " << magic_y << "is : " << magicMap[magic_x][magic_y] << endl;
				}
			}

		}
		cout << "end of giveMeMyMagicMap" << endl;
	}

	void MagicMap::giveMeMyTowerPopulation() {
		int bigmap_district_center_x = 0;
		int bigmap_district_center_y = 0;
		


		/* get tower ID for all districts */
		for (int magic_x = 0; magic_x < MAP_SIZE / DISTRICT_SIZE; magic_x++)
		{
			for (int magic_y = 0; magic_y < MAP_SIZE / DISTRICT_SIZE; magic_y++)
			{
				bigmap_district_center_x = (DISTRICT_SIZE / 2) + (DISTRICT_SIZE * magic_x);
				bigmap_district_center_y = (DISTRICT_SIZE / 2) + (DISTRICT_SIZE * magic_y);
				double min_distance = getDistance(DISTRICT_SIZE, DISTRICT_SIZE, DISTRICT_SIZE/2, DISTRICT_SIZE / 2);

				int tower_ID = -1;
				/* get nearest tower to district center */
				for (int i = 0; i < TOWER_MAX; i++)
				{
					if (min_distance > getDistance(bigmap_district_center_x, bigmap_district_center_y, towers[i][0], towers[i][1]))
					{
						min_distance = getDistance(bigmap_district_center_x, bigmap_district_center_y, towers[i][0], towers[i][1]);
						tower_ID = i;
					}
				}

				if (tower_ID != -1)
				{
					population_with_tower_id[magic_x * (MAP_SIZE / DISTRICT_SIZE) + magic_y][0] = magicMap[magic_x][magic_y]; // population in district
					population_with_tower_id[magic_x * (MAP_SIZE / DISTRICT_SIZE) + magic_y][1] = tower_ID;				      // nearest tower id
				}
				else
				{
					population_with_tower_id[magic_x * (MAP_SIZE / DISTRICT_SIZE) + magic_y][0] = 0; // population in district
					population_with_tower_id[magic_x * (MAP_SIZE / DISTRICT_SIZE) + magic_y][1] = tower_ID;				      // nearest tower id
				}
			}
		}

		/* order towers by population */
		for (int i = 1; i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE); i++)
		{
			int j = i - 1;
			int temp_pop = population_with_tower_id[i][0];
			int temp_tower_id = population_with_tower_id[i][1];
			while ( j>=0 && population_with_tower_id[j][0] > temp_pop )
			{
				population_with_tower_id[j + 1][0] = population_with_tower_id[j][0];
				population_with_tower_id[j + 1][1] = population_with_tower_id[j][1];
				j--;
			}

			population_with_tower_id[j + 1][0] = temp_pop;
			population_with_tower_id[j + 1][1] = temp_tower_id;
		}

		/* reverse the array - biggest population first */
		std::reverse(std::begin(population_with_tower_id), std::end(population_with_tower_id));
	}

	void MagicMap::simulatePopulationChange() {
		for (int i = 1; i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE); i++)
		{
			population_with_tower_id[i][0] = population_with_tower_id[i][0] * AVERAGE_POPULATION_GROWTH;
		}
	}

};

class TPlayer {

public:
    int ID;
    int seed;
    int myTime;
	int money;

	MoneyBuffer playerMoneyBuffer;
	PlayerTowers playerTowers;

    Map *map = nullptr;

	MagicMap magicMap;

    TheaderIni headerIni;
    TinputData inputData;
    ToutputData outputData;

    string scriptName;

    TPlayer() :ID(0), seed(0), myTime(0) { }
    ~TPlayer() { if (map) delete this->map; }

    void init() {
		time_t initTime = time(0);
        map = new Map();
        map->GenerateMap(headerIni.seed);
		magicMap.setValues(map->pop, map->towers);
		magicMap.giveMeMyMagicMap();
		magicMap.giveMeMyTowerPopulation();

		initTime = time(0) - initTime;
		cout << "initTime: " << initTime << " seconds" << endl << endl;
    }



    void loadScript() {
        if (scriptName.size() == 0) return;
        OMax = 0; OMut = 1;
        std::ifstream infile1("data\\" + scriptName + ".txt");
        if (infile1) {
            std::string line;
            while (std::getline(infile1, line))
            {
                int n;
                std::istringstream iss(line);
                OMax++;
                iss >> Times[OMax] >> OScript[OMax].towerID >> OScript[OMax].rentingCost >> n >> OScript[OMax].distance >> OScript[OMax].offer;
                OScript[OMax].leave = n > 0;
            }
            infile1.close();
        }

        for (int i = 0; i <= TIME_MAX; i++) IScript[i] = 0;

        std::ifstream infile2("data\\" + scriptName + ".inv");
        if (infile2) {
            int n = 0;
            std::string line;
            while (std::getline(infile2, line) && n < TIME_MAX)
            {
                std::istringstream iss(line);
                n++;
                iss >> IScript[n];
            }
            infile2.close();
        }
    }

    void makeMove();

	void AddTowersIfItsOur()
	{
		for (int i = 0; i < TOWER_MAX; i++)
		{
			if (inputData.towerInf[i].owner == ID && inputData.towerInf[i].licit == 0 && !playerTowers.IsItTowerInLeavedTowerList(i))
				if (!playerTowers.IsItOurTower(i))
					playerTowers.Add(i);
		}
	}
	//[i][0]: tower index, [i][1]: max costumer, [i][2]:maxprofit, [i][3]: current customer, [i][4]: current profit
	void HandleCostumerChange()
	{
		int tempOffer = 1;
		for (int i = 0; i < playerTowers.actualPosition; i++)
		{
			if (playerTowers.playerTowerIndexes[i][0] != -1)
			{
				tempOffer = PalyerCalculateMinimumOffer(playerTowers.playerTowerIndexes[i][0]);

				if ((playerTowers.playerTowerIndexes[i][3] - inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust) > CUSTOMER_CHANGES)
				{

					if (inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*0.8 < tempOffer)
					{
						changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0],
							inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
							tempOffer);
					}
					else
					{
						changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0],
							inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
							inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*0.9);
					}
				}

				else if ((inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust - playerTowers.playerTowerIndexes[i][3]) > CUSTOMER_CHANGES)
				{
					changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0],
						inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
						inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*1.1);
				}
				else if (inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust > MAX_CUSTOMER_AT_A_TOWER)
				{
					changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0],
						inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
						inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*1.1);
				}
				else if (inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust < MIN_CUSTOMER_AT_A_TOWER && inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust > 0)
				{
					changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0],
						inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
						inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*0.9);
				}
			}
		}
	}

	void HandleTowerDistance()
	{
		for (int i = 0; i < playerTowers.actualPosition; i++)
		{
			if(inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance < 15)
				changeDistanceAndOffer(	playerTowers.playerTowerIndexes[i][0],
										15,
										inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer);
		}
	}

	double PalyerCalculateMinimumOffer(int towerID)
	{
		if (inputData.towerInf[towerID].cust > 0)
			return ((inputData.towerInf[towerID].rentingCost + inputData.towerInf[towerID].runningCost) / (inputData.towerInf[towerID].cust *PROFIT_PER_CUSTOMER));
		else
			return 5;
	}

	double CalculateMaximumPriceOfRent(int customer, double distance, double offer) {
		cout << "MAX fv, customer: " << customer << "distance: " << distance << "offer: " << offer << endl;
		double income = customer*PROFIT_PER_CUSTOMER*PREDICT_OF_CUSTUMER_OF_A_TOWER*offer;
		double runnungCost = distance*distance*0.04;
		cout << "MAX fv, income: " << income << "runnungCost: " << runnungCost << endl;
		return income - runnungCost;			
	}

	void GrowthStateLevel1()
	{
		int i = 0;
		int distance = 0;
		int maxTowerBuy = 0, maxTowerLicit=0;
		int currentRentingCost = 0;
		while (money > inputData.header.money*0.95 && (i < (MAP_SIZE / DISTRICT_SIZE)*(MAP_SIZE / DISTRICT_SIZE)) && inputData.header.money > SAFETY_MONEY)
		{
			distance = getDistanceForRent(magicMap.population_with_tower_id[i][0]);
			// rent free towers
			if ((inputData.towerInf[magicMap.population_with_tower_id[i][1]].owner == 0) && 
				magicMap.population_with_tower_id[i][0]>DEFAULT_POPULATION && 
				inputData.header.time % 5 == 0 && 
				maxTowerBuy <= MAX_TOWER_BUY && 
				!playerTowers.IsItTowerInLeavedTowerList(magicMap.population_with_tower_id[i][1]))
			{
				rentTower(magicMap.population_with_tower_id[i][1], DEFAULT_RENTING_COST, distance,
					CalculateOffer(10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[i][0], DEFAULT_RENTING_COST, magicMap.population_with_tower_id[i][0]));
				money -= DEFAULT_RENTING_COST;
				cout << "tower pop: " << magicMap.population_with_tower_id[i][0] << endl;
				maxTowerBuy++;
			}
			//licit to another player's tower
			else if (inputData.towerInf[magicMap.population_with_tower_id[i][1]].owner != ID &&
				inputData.towerInf[magicMap.population_with_tower_id[i][1]].licitID != ID && 
				magicMap.population_with_tower_id[i][0]>DEFAULT_POPULATION && money > inputData.header.money*0.95 &&
				inputData.header.time >100 && maxTowerLicit <= MAX_TOWER_BUY) //not our tower
			{
				//worth to overlicit?
				if (inputData.towerInf[magicMap.population_with_tower_id[i][1]].licit > 0)
					currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[i][1]].licit;
				else
					currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[i][1]].rentingCost;

				double offer = CalculateOffer(10 + DISTRICT_SIZE*0.0000015* magicMap.population_with_tower_id[i][0], currentRentingCost, magicMap.population_with_tower_id[i][0]);
				double maximumRentingCost = CalculateMaximumPriceOfRent(magicMap.population_with_tower_id[i][0], distance, offer);
				double ourOfferForRenting = currentRentingCost + (maximumRentingCost - currentRentingCost) *0.25;
				//cout << "maximumRentingCost: " << maximumRentingCost << endl;
				if (maximumRentingCost > ourOfferForRenting) // skip towers which are not profitable
				{
					//cout << "licit tower ID: " << magicMap.population_with_tower_id[i][1] << endl;
					rentTower(magicMap.population_with_tower_id[i][1], ourOfferForRenting, distance, offer);
					money -= ourOfferForRenting;
					maxTowerLicit++;
				}
			}
			// ha esetleg a mi tornyainkat licit�lj�k
			else if (inputData.towerInf[magicMap.population_with_tower_id[i][1]].owner == ID &&
				inputData.towerInf[magicMap.population_with_tower_id[i][1]].licitID != 0 &&
				inputData.towerInf[magicMap.population_with_tower_id[i][1]].licitDelay < 3)
			{
				if (inputData.towerInf[magicMap.population_with_tower_id[i][1]].licit > 0)
					currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[i][1]].licit+1;
				else
					currentRentingCost = inputData.towerInf[magicMap.population_with_tower_id[i][1]].rentingCost + 1;

				double offer = CalculateOffer(	10 + DISTRICT_SIZE*0.00000015* magicMap.population_with_tower_id[i][0],
												currentRentingCost, magicMap.population_with_tower_id[i][0]);

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
			outputData.invest = playerMoneyBuffer.AvarageOfLastFiveMonth()*0.15;
	}

	double getDistanceForRent(int population) {
		return 10 + DISTRICT_SIZE*0.000001* population;
	}

	double CalculateOffer(int distance, int rentingCost, int disctrictCustomer)
	{
		return ((1 + PLAN_PROFIT)*(distance*distance*0.04 + rentingCost) / (disctrictCustomer*PROFIT_PER_CUSTOMER*PREDICT_OF_CUSTUMER_OF_A_TOWER));
	}

	void LeaveTowersFromBlackList()
	{
		for (int i = 0; i < playerTowers.actualPositionOfBlackList; i++)
		{
			if (playerTowers.playerBlackListTower[i][0] != -1)
			{
				leaveTower(playerTowers.playerBlackListTower[i][0]);
				playerTowers.AddToLeavedTowers(playerTowers.playerBlackListTower[i][0]);
				playerTowers.playerBlackListTower[i][0] = -1; // Remove
				
			}
		}
	}

protected:
    void rentTower(short ID, float rentingCost, short dist, float offer)
    {
        if (outputData.numOrders < ORDER_MAX)
        {
            outputData.orders[outputData.numOrders].towerID = ID;
            outputData.orders[outputData.numOrders].rentingCost = rentingCost;
            outputData.orders[outputData.numOrders].distance = dist;
            outputData.orders[outputData.numOrders].offer = offer;
            outputData.orders[outputData.numOrders].leave = 0;
            outputData.numOrders++;
        }
    }


    void leaveTower(short ID)
    {
        if (outputData.numOrders < ORDER_MAX)
        {
            outputData.orders[outputData.numOrders].towerID = ID;
            outputData.orders[outputData.numOrders].leave = 1;
            outputData.orders[outputData.numOrders].rentingCost = 0;
            outputData.orders[outputData.numOrders].distance = 0;
            outputData.orders[outputData.numOrders].offer = 0;
            outputData.numOrders++;
        }

    }

    void changeDistanceAndOffer(short ID, short dist, float offer)
    {
        if (outputData.numOrders < ORDER_MAX)
        {
            outputData.orders[outputData.numOrders].towerID = ID;
            outputData.orders[outputData.numOrders].distance = dist;
            outputData.orders[outputData.numOrders].offer = offer;
            outputData.orders[outputData.numOrders].leave = 0;
            outputData.numOrders++;
        }
    }

private:
    int OMax;
    int OMut;
    TtowerOrderRec OScript[ORDER_MAX + 1];   // order
    int Times[ORDER_MAX + 1];
    float IScript[TIME_MAX + 1];             // invest

};



