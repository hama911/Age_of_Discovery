#pragma once
#include "Pos.h"

struct Node;

struct Path
{
	Path::Path(const int& _parentNodeID, const int& _childNodeID)
		: id(0)
		, parentNodeID(_parentNodeID)
		, childNodeID(_childNodeID)
	{}

	int		id;
	double	len;	//三次元座標上の距離
	int		parentNodeID;
	int		childNodeID;
};

struct Node
{
	Node::Node(const int& _id, const Pos& _pos)
		: id(_id)
		, joinedRegionID(-1)
		, isSea(false)
		, isOcean(false)
		, isCoast(false)
		, pos(_pos)
		, isInQueue(false)
		, isScaned(false)
		, cost(0.0)
		, fromNodeID(-1)
		, ownCityID(-1)
	{}

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

struct Route
{
	Route(const int& _id)
		: id(_id) {}

	int		id;
	String	name;
	int		destinationNodeID;
	int		originNodeID;
	Array<int>	pathIDs;
	double	totalLength;
	bool	isSeaRoute;
};