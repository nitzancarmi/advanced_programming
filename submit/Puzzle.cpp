#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <assert.h> 
#include "Puzzle.h"
#include <algorithm>

using std::getline;
using std::string;
using std::endl;
using std::pair;



Puzzle::Puzzle() 
{
	m_vParts = NULL;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				for (int s = -1; s < 2; s++)
				{
					m_mPartMap[make_pair(i, j)][make_pair(k, s)] = new list<Part>();
				}
			}
		}
	}      
}

Puzzle::~Puzzle()
{
	if (m_vParts)
		delete m_vParts;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				for (int s = -1; s < 2; s++)
				{
					delete m_mPartMap[make_pair(i, j)][make_pair(k, s)];
				}
			}
		}
	}
}

int Puzzle::init(std::string path)
{
	int rc = 0;
	size_t pos;
	string line, token;
	std::ifstream input;
	std::vector<string> wrongIds, missingIds;
	std::vector<pair<int, string>> wrongFormats;
	input.open(path);
	if (!input.is_open()) {
		*fout << "Failed to open input file" << endl;
		return -EINVAL;
	}

	getline(input, line);
	pos = line.find("=");
	if (pos != string::npos) {
		token = line.substr(0, pos);
		if (token.length() < 11 ||
			token.compare(0, 11, "NumElements")) {
			*fout << "Invalid first line in input file" << endl;
			rc = -EINVAL;
		}
		else
			line.erase(0, pos + 1);

		token = line.substr(line.find_first_not_of(" "), string::npos);
		std::istringstream buf(token);

		buf >> m_iNumOfElements;
		if (m_iNumOfElements < 1) {
			*fout << "Invalid NumOfElements" << endl;
		}
	}
	std::vector<Part> *Parts = new std::vector<Part>(m_iNumOfElements);

	while (input && getline(input, line) && line.length()!=0) {
		int id, left, up, right, down, eol;
		std::istringstream iss;
		iss.str(line);

		iss >> id;
		if (iss.fail()) {
			wrongIds.emplace_back(std::to_string(id));
			rc = -1;
			continue;
		}

		if (id <= 0 || id > (int)m_iNumOfElements) {
			wrongIds.emplace_back(std::to_string(id));
			rc = -1;
			continue;
		}


		iss >> left;
		if (iss.fail() || abs(left)>1 ) {
			wrongFormats.emplace_back(make_pair(id, line));
			rc = -1;
			continue;
		}
	
		iss >> up;
		if(iss.fail() || abs(up)>1) {
			wrongFormats.emplace_back(make_pair(id, line));
			rc = -1;
			continue;
		}
	
		iss >> right;
		if(iss.fail() || abs(right)>1) {
			wrongFormats.emplace_back(make_pair(id, line));
			rc = -1;
			continue;
		}
	
		iss >> down;
		if(iss.fail() || abs(down)>1) {
			wrongFormats.emplace_back(make_pair(id, line));
			rc = -1;
			continue;
		}
	
		iss >> eol;
		if(!iss.fail() || !iss.eof()) {
			wrongFormats.emplace_back(make_pair(id, line));
			rc = -1;
			continue;
		}

		Parts->at(id-1) = Part(id, left, up, right, down);


	}

	for (int i = 0; i < (int)Parts->size(); i++) {
		Part& p = Parts->at(i);
		if(!p.getId()) { //only missing, not wrong ID
			missingIds.emplace_back(std::to_string(i+1));
			rc = -1;
		}
	}


	if ((int)missingIds.size() > 0)
	{
		bool printEndl = false; 
		bool firstRound = true;
		for (int i = 0; i < (int)missingIds.size(); i++)
		{
			string deli = 
				(i == (int)missingIds.size() - 1) ? "" : ", ";
			//print only if part is not a wrong-format ID
			auto itr = find_if(wrongFormats.begin(), wrongFormats.end(),
				[id = missingIds.at(i)](const auto& format) {
				return std::to_string(format.first) == id;
			});
			if (itr == wrongFormats.end())
			{
				if (firstRound)
				{
					*fout << "Missing puzzle element(s) with the following IDs: ";
					firstRound = false;
				}
				*fout << missingIds[i] << deli;
				printEndl = true;
			}	
		}
		if(printEndl)
			*fout << endl;
	}

	if ((int)wrongIds.size() > 0)
	{
		*fout << "Puzzle of size " << m_iNumOfElements << " cannot have the following IDs: ";
		for (int i = 0; i < (int)wrongIds.size(); i++)
		{
			string deli =
				(i == (int)wrongIds.size() - 1) ? "" : ", ";
			*fout << wrongIds[i] << deli;
		}
		*fout << endl;
	}

	for (const pair<int, string> &line : wrongFormats)
	{
		*fout << "Puzzle ID " << line.first << " has wrong data: " << line.second << endl;
	}
	m_vParts = Parts;
	return rc;
}


bool Puzzle::isValidStraightEdges(int sizei, int sizej)
{
	int leftStraight = 0;
	int topStraight = 0;
	for (size_t i = 0; i < m_iNumOfElements; i++)
	{		
		int left = (*m_vParts)[i].getLeft();
		int top = (*m_vParts)[i].getTop();

		if (left == 0)
			leftStraight++;
		if (top == 0)
			topStraight++;
	}
	if (leftStraight < sizei || topStraight < sizej)
		return false;
	return true;
}


int Puzzle::solveRec(size_t i, size_t j, Table& tab)
{
	int** table = tab.getTable(); 
	
	int leftpeek, toppeek, rightpeek, bottompeek;
	rightpeek = bottompeek = -2; //there is no part to the right/bottom
	leftpeek  = toppeek    = 0; //there is a frame part to the left/top
	
	if (j > 0 && table[i][j - 1] >= 0)
		leftpeek = 0-((*m_vParts)[table[i][j - 1] - 1].getRight());
	if (i > 0 && table[i - 1][j] >= 0)
		toppeek =  0-((*m_vParts) [table[i-1][j]-1].getBottom());
	if(j < (size_t)tab.getCols() - 1 && table[i][j + 1] != 0) //not empty
		rightpeek = 0;
	if (i < (size_t)tab.getRows() - 1 && table[i+1][j] != 0) //not empty
		bottompeek = 0;

	map<pair<int, int>, list<Part>*> currentMap= m_mPartMap[make_pair(leftpeek, toppeek)];
	
	//We always check the top-left directions - 
	//solve the puzzle from top-left to bottom-right
	for (auto const &m : currentMap)
	{
		//no parts of this left-top edges are avilable
		if (m.second->empty())
			continue;
		
		//this is not the apropriate right-edge list of parts 
		if (rightpeek > -2 && rightpeek != m.first.first)
			continue;

		//this is not the apropriate bottom-edge list of parts 
		if (bottompeek > -2 && bottompeek != m.first.second)
			continue;

		//if we got so far, we have match in this list - continue checking this part
		Part current = m.second->front();
		m.second->pop_front();
		table[i][j] = current.getId();
		
		//End of table
		if ((i == (size_t)tab.getRows() - 1) && (j == (size_t)tab.getCols() - 1))
			return 0; //solve succeeded

		//End of line
		if (j == (size_t)tab.getCols() - 1)
		{
			if (solveRec(i + 1, 0, tab) == 0) //move to the next line, and first column
				return 0;
			else
			{
				table[i][j] = 0;
				m.second->push_back(current);
				continue;
			}
		}
		else // "middle" cell in line
		{
			if (solveRec(i, j + 1, tab) == 0) //continue solving along the current line
				return 0;
			else
			{
				table[i][j] = 0;
				m.second->push_back(current);
				continue;
			}
		}
	}
	return -1;
}

Table Puzzle::Solve()
{
	unsigned int i;
	int ret;
	unsigned int size = m_iNumOfElements;

	for(i = 1; i <= size; i++) {
		if (size % i == 0) {
			if (!isValidStraightEdges(i, size / i))
				continue;
				
			Table table(i, size/i); 
			ret = solveRec(0, 0, table);
			if (ret == 0)
				return table;
		}
	}
	
	*fout << "Cannot solve puzzle : it seems that there is no proper solution" << endl;
	
	return Table();
}

bool Puzzle::cornerCheck(bool &tr, bool &tl, bool &br, bool &bl)
{
	//the easy case - only one element
	if (m_iNumOfElements == 1) {
		Part& p = (*m_vParts)[0];
		tr = p.getTop() == 0 && p.getRight() == 0;
		tl = p.getTop() == 0 && p.getLeft() == 0;
		br = p.getBottom() == 0 && p.getRight() == 0;
		bl = p.getBottom() == 0 && p.getLeft() == 0;
		return (tr && tl && br && bl);
	}

	//the regular case - different 4 corner parts
	for (size_t i = 0; i < m_iNumOfElements; i++)
	{
		Part& pi = (*m_vParts)[i];
		if (pi.getTop() == 0 && pi.getRight() == 0)
		{
			pi.setCorner(true);
			for (size_t j = 0; j < m_iNumOfElements; j++)
			{
				Part& pj = (*m_vParts)[j];
				if (!pj.isCorner() && pj.getTop() == 0 && pj.getLeft() == 0)
				{
					pj.setCorner(true);
					for (size_t k = 0; k < m_iNumOfElements; k++)
					{
						Part& pk = (*m_vParts)[k];
						if (!pk.isCorner() && pk.getBottom() == 0 && pk.getRight() == 0)
						{
							pk.setCorner(true);
							for (size_t m = 0; m < m_iNumOfElements; m++)
							{
								Part& pm = (*m_vParts)[m];
								if (!pm.isCorner() && pm.getBottom() == 0 && pm.getLeft() == 0)
								{
									pm.setCorner(true);
									return true;
								}
							}
							pk.setCorner(false);
						}
					}
					pj.setCorner(false);
				}
			}
			pi.setCorner(false);
		}
	}

	//the row case - 2 corner parts
	for (size_t i = 0; i < m_iNumOfElements; i++)
		{
			Part& pi = (*m_vParts)[i];

			//if a single part isn't straight on top/bottom, it will fail
			if (pi.getTop() != 0 || pi.getBottom() != 0)
				break;

			if (pi.getRight() == 0)
			{
				pi.setCorner(true);
				for (size_t j = 0; j < m_iNumOfElements; j++)
				{
					Part& pj = (*m_vParts)[j];
					if(!pj.isCorner() && pj.getLeft() == 0)
					{
						pj.setCorner(true);
						return true;
					}
				}
				pi.setCorner(false);
			}
		}

	//the col case - 2 corner parts
	for (size_t i = 0; i < m_iNumOfElements; i++)
		{
			Part& pi = (*m_vParts)[i];

			//if a single part isn't straight on right/left, it will fail
			if (pi.getLeft() != 0 || pi.getRight() != 0)
				break;

			if (pi.getTop() == 0)
			{
				pi.setCorner(true);
				for (size_t j = 0; j < m_iNumOfElements; j++)
				{
					Part& pj = (*m_vParts)[j];
					if (!pj.isCorner() && pj.getBottom() == 0)
					{
						pj.setCorner(true);
						return true;
					}
				}
				pi.setCorner(false);
			}
		}

	// if we got so far, it means there is no available solution (not enough corners)!
	for (size_t i = 0; i < m_iNumOfElements; i++)
	{
		Part& p = (*m_vParts)[i];
		if (p.getTop() == 0 && p.getRight() == 0) {
			tr = true;
			continue;
		}
		if (p.getTop() == 0 && p.getLeft() == 0) {
			tl = true;
			continue;
		}
		if (p.getBottom() == 0 && p.getRight() == 0) {
			br = true;
			continue;
		}
		if (p.getBottom() == 0 && p.getLeft() == 0) {
			bl = true;
			continue;
		}
	}
	return false;
}


int Puzzle::preProcess()
{
	int sum = 0;
	int topStraight = 0;
	int bottomStraight = 0;
	int leftStraight = 0;
	int rightStraight = 0;
	bool tr = false;
	bool tl = false;
	bool br = false;
	bool bl = false;
	int ret = 0;

	for (size_t i = 0; i < m_iNumOfElements; i++)
	{
		int left = (*m_vParts)[i].getLeft();
		int top = (*m_vParts)[i].getTop();
		int right = (*m_vParts)[i].getRight();
		int bottom = (*m_vParts)[i].getBottom();

		if (left == 0)
			leftStraight++;
		if (top == 0)
			topStraight++;
		if (right == 0)
			rightStraight++;
		if (bottom == 0)
			bottomStraight++;

		sum += (*m_vParts)[i].getLeft();
		sum += (*m_vParts)[i].getTop();
		sum += (*m_vParts)[i].getRight();
		sum += (*m_vParts)[i].getBottom();
	}

	if (!((topStraight == bottomStraight && topStraight != 0) && (leftStraight == rightStraight && leftStraight != 0)))
	{
		*fout << "Cannot solve puzzle: wrong number of straight edges" << endl;
		ret = -1;
	}

	if (Puzzle::cornerCheck(tr, tl, br, bl) == false) 
	{
		ret = -1;
		if (tl == false)
			*fout << "Cannot solve puzzle: missing corner element: TL" << endl;
		if (tr == false)
			*fout << "Cannot solve puzzle: missing corner element: TR" << endl;
		if (bl == false)
			*fout << "Cannot solve puzzle: missing corner element: BL" << endl;
		if (br == false)
			*fout << "Cannot solve puzzle: missing corner element: BR" << endl;
	}

	if (sum != 0)
	{
		*fout << "Cannot solve puzzle : sum of edges is not zero" << endl;
		ret = -1;
	}

	return ret;
}


void Puzzle::initPartsMap()
{
	int l, t, r, b;
	for (size_t i = 0; i < m_iNumOfElements; i++)
	{
		Part& p = m_vParts->at(i);
		l = p.getLeft();
		t = p.getTop();
		r = p.getRight();
		b = p.getBottom();

		m_mPartMap[make_pair(l,t)][make_pair(r,b)]->push_back(p);
	}
}
