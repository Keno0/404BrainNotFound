#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "decl.h"
#include "map.h"

using namespace std;


class MagicMap {
public:
	int population[MAP_SIZE][MAP_SIZE];
	int towers[TOWER_MAX][2];

	double MagicMap::getDistance(int x, int y, int j, int k) {
		return std::sqrt(std::pow((x - j), 2) + std::pow(y - k, 2));
	}

	void MagicMap::setValues(int pop[MAP_SIZE][MAP_SIZE], int tow[TOWER_MAX][2]) {
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

	void MagicMap::giveMeMyMagicMap(int population[MAP_SIZE][MAP_SIZE], int magicMap[MAP_SIZE / 30][MAP_SIZE / 30]) {
		
		for (int magic_x = 0; magic_x < MAP_SIZE / 30; magic_x++)
		{
			for (int magic_y = 0; magic_y < MAP_SIZE / 30; magic_y++)
			{
				for (int pop_x = magic_x * 30; pop_x < magic_x * 30 + 30; pop_x++)
				{
					for (int pop_y = magic_y * 30; pop_y < magic_y * 30 + 30; pop_y++)
					{
						magicMap[magic_x][magic_y] += (int)population[pop_x][pop_y];
					}
				}
			}

		}
	}

	void MagicMap::giveMeMyTowerPopulation(int magicMap[MAP_SIZE / 30][MAP_SIZE / 30], int towers[][2], int population_with_tower_id[(MAP_SIZE / 30)*(MAP_SIZE / 30)][2]) {
		int bigmap_district_center_x = 0;
		int bigmap_district_center_y = 0;
		int tower_ID = 0;


		/* get tower ID for all districts */
		for (int magic_x = 0; magic_x < MAP_SIZE / 30; magic_x++)
		{
			for (int magic_y = 0; magic_y < MAP_SIZE / 30; magic_y++)
			{
				bigmap_district_center_x = 15 + 30 * magic_x;
				bigmap_district_center_y = 15 + 30 * magic_y;
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

				population_with_tower_id[magic_x * (MAP_SIZE / 30) + magic_y][0] = magicMap[magic_x][magic_y]; // population in district
				population_with_tower_id[magic_x * (MAP_SIZE / 30) + magic_y][1] = tower_ID;					 // nearest tower id
			}
		}
	}

};


class TPlayer {

public:
    int ID;
    int seed;
    int myTime;
	int magicMapLoaded[MAP_SIZE / 30][MAP_SIZE / 30];
	int magicMapTowerWithPop[(MAP_SIZE / 30)*(MAP_SIZE / 30)][2];

    Map *map = nullptr;

	MagicMap magicMap;


    TheaderIni headerIni;
    TinputData inputData;
    ToutputData outputData;

    string scriptName;

    TPlayer() :ID(0), seed(0), myTime(0) { }
    ~TPlayer() { if (map) delete this->map; }

    void init() {
        map = new Map();
        map->GenerateMap(headerIni.seed);
		magicMap.setValues(map->pop, map->towers);
		

		int population[MAP_SIZE][MAP_SIZE];
		for (int i = 0; i < MAP_SIZE; i++)
		{
			for (int j = 0; j < MAP_SIZE; j++)
			{
				population[i][j] = map->pop[i][j];
			}
		}
		
		magicMap.giveMeMyMagicMap(population, magicMapLoaded);
		magicMap.giveMeMyTowerPopulation(magicMapLoaded, map->towers, magicMapTowerWithPop);
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
