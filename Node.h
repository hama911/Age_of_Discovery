#pragma once
#include "Pos.h"

struct Node;

struct Path
{
	Path(const int& _parentNodeID, const int& _childNodeID);
	double	len;	//�O�������W��̋���

	int		parentNodeID;
	int		childNodeID;
};

struct Node
{
	int		id;
	Node(const int& _id, const Pos& _pos);
	bool	isCoast;
	bool	isSea;
	Pos		pos;
	Array<Path> paths;

	//�o�H�T���A���S���Y���p
	bool	isInQueue;
	bool	isScaned;
	double	cost;

	int		fromNodeID;
	int		ownCityID;
};