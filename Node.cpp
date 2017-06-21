#include "Node.h"

Node::Node(const int& _id, const Pos& _pos)
	: id(_id)
{
	isSea = false;
	isCoast = false;
	pos = _pos;
	paths.clear();

	isInQueue = false;
	isScaned = false;
	cost = 0.0;
	fromNodeID = -1;
	ownCityID = -1;
}

Path::Path(const int& _parentNodeID, const int& _childNodeID)
	: parentNodeID(_parentNodeID), childNodeID(_childNodeID) {}