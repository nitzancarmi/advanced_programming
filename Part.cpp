#include "Part.h"


/////delete!!!!!
#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;


Part::Part(int id, int left, int top, int right, int bottom)
{
	m_iId = id;
	m_iLeft = left; 
	m_iTop = top;
	m_iRight = right;
	m_iBottom = bottom;
	m_iRow = m_iCol = -1;
	taken = false;
}

bool Part::isConnectedTo(Part& other, direction dir)
{
	int sum = -2;
	int currentEdge = -2;
	int otherEdge = -2;

	switch (dir)
	{
	case(TOP):
		currentEdge = this->getTop();
		otherEdge   = other.getBottom();
		cout << "this top: " << this->getTop() << endl;
		cout << "other bottom: " << other.getBottom() << endl;
		break;
	case(BOTTOM):
		currentEdge = this->getBottom();
		otherEdge   = other.getTop();
		break;
	case(LEFT):
		currentEdge = this->getLeft();
		otherEdge   = other.getRight();
		break;
	case(RIGHT):
		currentEdge = this->getRight();
		otherEdge   = other.getLeft();
		break;
	}

	sum = currentEdge + otherEdge;
	if(sum == 0)
		return true;
	return false;
}
