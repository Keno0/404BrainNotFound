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
#define PLAN_PROFIT 1.2 // used for calculate the offer when try to rent a tower, 0.2 = 20% profit
#define PROFIT_PER_CUSTOMER 0.00001
#define PREDICT_OF_CUSTUMER_OF_A_TOWER 0.2 //20%-a district population-nek
using namespace std;

class PlayerTowers
{
public:
	int playerTowerIndexes[PLAYER_TOWER_INDEXES][5]; //[i][0]: tower index, [i][1]: max costumer, [i][2]:maxprofit, [i][3]: current customer, [i][4]: current profit
	int actualPosition=0, actualPositionOfBlackList=0;
	int playerBlackListTower[PLAYER_TOWER_INDEXES][2]; //index, leaved

	PlayerTowers()
	{
		for (int i = 0; i < PLAYER_TOWER_INDEXES; i++)
		{
			playerBlackListTower[i][0] = -1;
			playerBlackListTower[i][1] = -1;
			for (int j = 0; j < 5; j++)
			{
				playerTowerIndexes[i][j] = -1;
			}
		}			
	}


	void AddTowerToBlacklist(int towerID)
	{
		if (actualPositionOfBlackList < PLAYER_TOWER_INDEXES)
		{
			playerBlackListTower[actualPositionOfBlackList][0] = towerID;
			playerBlackListTower[actualPositionOfBlackList][1] = 1;
			actualPositionOfBlackList++;
		}
		else
			actualPositionOfBlackList = 0;
	}

	void Add(int towerID)
	{
		if (actualPosition < PLAYER_TOWER_INDEXES)
		{
			playerTowerIndexes[actualPosition][0] = towerID;
			actualPosition++;
		}
		else
			actualPosition = 0;
	}

	void Remove(int towerID)
	{
		for (int i = 0; i <= actualPosition; i++)
		{
			if (playerTowerIndexes[i][0] == towerID)
			{
				playerTowerIndexes[i][0] = -1;
				playerTowerIndexes[i][1] = -1;
				playerTowerIndexes[i][2] = -1;

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

				if (inputData.towerInf[playerTowerIndexes[i][0]].cust + CUSTOMER_CHANGES > playerTowerIndexes[i][3])
					playerTowerIndexes[i][3] = inputData.towerInf[playerTowerIndexes[i][0]].cust;
				else if (inputData.towerInf[playerTowerIndexes[i][0]].cust - CUSTOMER_CHANGES < playerTowerIndexes[i][3])
					playerTowerIndexes[i][3] = inputData.towerInf[playerTowerIndexes[i][0]].cust;

				if (TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]) > playerTowerIndexes[i][2])
					playerTowerIndexes[i][2] = TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]);
				else if (TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]) <= 0)
				{

					AddTowerToBlacklist(playerTowerIndexes[i][0]);
					Remove(playerTowerIndexes[i][0]);
				}

				playerTowerIndexes[i][4] = TowerProfit(inputData.towerInf[playerTowerIndexes[i][0]]);
			}
		}
	}

	int TowerProfit(TtowerInfRec towerInfo)
	{
		return towerInfo.cust * towerInfo.offer*0.000001 - (towerInfo.rentingCost + towerInfo.runningCost);
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
	}

	void MagicMap::giveMeMyTowerPopulation() {
		int bigmap_district_center_x = 0;
		int bigmap_district_center_y = 0;
		int tower_ID = 0;


		/* get tower ID for all districts */
		for (int magic_x = 0; magic_x < MAP_SIZE / DISTRICT_SIZE; magic_x++)
		{
			for (int magic_y = 0; magic_y < MAP_SIZE / DISTRICT_SIZE; magic_y++)
			{
				bigmap_district_center_x = (DISTRICT_SIZE / 2) + (DISTRICT_SIZE * magic_x);
				bigmap_district_center_y = (DISTRICT_SIZE / 2) + (DISTRICT_SIZE * magic_y);
				double min_distance = 10000;

				/* get nearest tower to district center */
				for (int i = 0; i < TOWER_MAX; i++)
				{
					if (min_distance > getDistance(bigmap_district_center_x, bigmap_district_center_y, towers[i][0], towers[i][1]))
					{
						min_distance = getDistance(bigmap_district_center_x, bigmap_district_center_y, towers[i][0], towers[i][1]);
						tower_ID = i;
					}
				}

				population_with_tower_id[magic_x * (MAP_SIZE / DISTRICT_SIZE) + magic_y][0] = magicMap[magic_x][magic_y]; // population in district
				population_with_tower_id[magic_x * (MAP_SIZE / DISTRICT_SIZE) + magic_y][1] = tower_ID;				      // nearest tower id
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
		if (debug) {
			cout << "DEBUG" << endl << endl << endl;
			cout << "population" << endl;
			cout << population_with_tower_id[0][0] << endl;
			cout << population_with_tower_id[1][0] << endl;
			cout << population_with_tower_id[2][0] << endl;
			cout << population_with_tower_id[3][0] << endl;
			cout << population_with_tower_id[4][0] << endl;
			cout << population_with_tower_id[5][0] << endl;
			cout << population_with_tower_id[6][0] << endl;
			cout << "DEBUG END" << endl;
		}

	}

};

class TPlayer {

public:
    int ID;
    int seed;
    int myTime;

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
			if (inputData.towerInf[i].owner == ID && inputData.towerInf[i].licit == 0)
				if (!playerTowers.IsItOurTower(i))
					playerTowers.Add(i);
		}
	}
	//[i][0]: tower index, [i][1]: max costumer, [i][2]:maxprofit, [i][3]: current customer, [i][4]: current profit
	void HandleCostumerChange()
	{
		
		for (int i = 0; i < playerTowers.actualPosition; i++)
		{
			if ((playerTowers.playerTowerIndexes[i][3] - inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust) > CUSTOMER_CHANGES)
			{
				int tempOffer = PalyerCalculateMinimumOffer(playerTowers.playerTowerIndexes[i][0]);
				if (inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*0.9 < tempOffer)
				{					
					changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0], inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
						tempOffer);
				}
				else
					changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0], inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
					inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*0.9);
			}

			else if ((inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].cust - playerTowers.playerTowerIndexes[i][3]) > CUSTOMER_CHANGES)
			{
				changeDistanceAndOffer(playerTowers.playerTowerIndexes[i][0], inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].distance,
					inputData.towerInf[playerTowers.playerTowerIndexes[i][0]].offer*1.1);
			}
		}
	}

	double PalyerCalculateMinimumOffer(int towerID)
	{
		return ((inputData.towerInf[towerID].rentingCost + inputData.towerInf[towerID].runningCost) / (inputData.towerInf[towerID].cust *PROFIT_PER_CUSTOMER));
	}

	double CalculateMaximumPriceOfRent(int customer, double distance, double offer) {

		double income = customer*PROFIT_PER_CUSTOMER*PREDICT_OF_CUSTUMER_OF_A_TOWER*offer;
		double runnungCost = distance*distance*0.04;

		return income - runnungCost;			
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



