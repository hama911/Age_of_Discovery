#include "Node.h"

Node::Node(const Pos& _pos)
{
	isSea = false;
	pos = _pos;
	paths.clear();

	isInQueue = false;
	isScaned = false;
	cost = 0.0;
	from = NULL;
}

Path::Path(Node* _parent, Node* _child)
	: parent(_parent), child(_child) {}