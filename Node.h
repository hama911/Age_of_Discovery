#pragma once
#include "Pos.h"

struct Node;

struct Path
{
	Path(const int& _parentNodeID, const int& _childNodeID);

	int		id;
	double	len;	//�O�������W��̋���
	int		parentNodeID;
	int		childNodeID;
};

struct Node
{
	Node(const int& _id, const Pos& _pos);
	int		joinedRegionID;
	int		id;
	bool	isCoast;
	bool	isSea;
	bool	isOcean;
	Pos		pos;
	Array<Path> paths;

	//�o�H�T���A���S���Y���p
	bool	isInQueue;
	bool	isScaned;
	double	cost;

	int		fromNodeID;
	int		ownCityID;
};