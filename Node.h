#pragma once
#include "Pos.h"

struct Node;

struct Path
{
	Path(const int& _parentNodeID, const int& _childNodeID);

	int		id;
	double	len;	//三次元座標上の距離
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

	//経路探索アルゴリズム用
	bool	isInQueue;
	bool	isScaned;
	double	cost;

	int		fromNodeID;
	int		ownCityID;
};