#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "decl.h"
#include "map.h"

#define SIZE_OF_MONEY_BUFFER 20
#define PLAYER_TOWER_INDEXES 100
using namespace std;

class PlayerTowers
{
public:
	int playerTowerIndexes[PLAYER_TOWER_INDEXES];
	int actualPosition=0;

	PlayerTowers()
	{
		for (int i = 0; i < PLAYER_TOWER_INDEXES; i++)
			playerTowerIndexes[i] = -1;
	}

	void Add(int towerID)
	{
		if (actualPosition < PLAYER_TOWER_INDEXES)
		{
			playerTowerIndexes[actualPosition] = towerID;
			actualPosition++;
		}
		else
			actualPosition = 0;
	}

	void Remove(int towerID)
	{
		for (int i = 0; i <= actualPosition; i++)
		{
			if (playerTowerIndexes[i] == towerID)
			{
				playerTowerIndexes[i] = -1;
			}
		}
	}

	bool IsItOurTower(int towerID)
	{
		for (int i = 0; i <= actualPosition; i++)
		{
			if (playerTowerIndexes[i] == towerID)
				return true;
		}

		return false;
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

class TPlayer {

public:
    int ID;
    int seed;
    int myTime;
	MoneyBuffer playerMoneyBuffer;
	PlayerTowers playerTowers;

    Map *map = nullptr;

    TheaderIni headerIni;
    TinputData inputData;
    ToutputData outputData;

    string scriptName;

    TPlayer() :ID(0), seed(0), myTime(0) { }
    ~TPlayer() { if (map) delete this->map; }

    void init() {
        map = new Map();
        map->GenerateMap(headerIni.seed);
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



