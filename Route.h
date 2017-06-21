#pragma once
#include"Node.h"

struct Route
{
	Route(const int& _id);
	int		id;

	String	name;
	int		destinationNodeID;
	int		originNodeID;
	Array<Path*>	paths;
	double	totalLength;
	bool	isSeaRoute;
};