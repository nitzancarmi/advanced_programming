#ifndef NONROTATEPUZZLE_H
#define NONROTATEPUZZLE_H

#include "Part.h"
#include "Table.h"
#include "Puzzle.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include<list>
using namespace std;

typedef map<pair<int, int>, map<pair<int, int>, list<Part*>*>> partMapNonRotate_t;

class nonRotatePuzzle : public Puzzle
{
	//partMapNonRotate_t m_mPartMap;

public:
	

	/*Checking 3 possible problems that end the game..
	* 1- Have we got 4 corners?
	* 2- Wrong number of straight edges? (left = right , bottom = top)
	* 3- Is the sum of edges equal to 0 ?
	*
	*returns 0 on success
	*/
	virtual int preProcess();

	
	virtual void createDataBase(vector<Part>& parts, common_match_t& cm, full_match_t& fm);

private:
	void ComputeCommonDataBase(partMapNonRotate_t& partMap, common_match_t& cm);
	void ComputeFullDataBase(partMapNonRotate_t& partMap, full_match_t& fm);
	void initiatePartMap(partMapNonRotate_t& pmap);

	virtual bool isValidStraightEdges(int sizei, int sizej);
	virtual bool cornerCheck(bool &tr, bool &tl, bool &br, bool &bl);
	virtual list<pair<list<Part*>*, list<int>>> getMatches(int left, int top, int right, int bottom, partMapNonRotate_t& partMap);

	/*
	* A data structure which contains:
	* 1- a map of <left, top> keys, the value is:
	* 2- a map of <right, bottom> keys, the value is:
	* 3- list of parts that has the edges of the keys indexes
	*
	*/
	partMapNonRotate_t setPartsMap(vector<Part>& parts);
};

#endif
