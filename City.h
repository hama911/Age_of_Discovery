#pragma once
#include<Siv3D.hpp>
#include "Market.h"

struct Node;

struct Citizen
{
	String	name;
	int		joinedCityID;

	int		income;
};

struct City
{
	City(const int& _id);

	int		id;

	int		money;

	Market	market;

	String	name;
	int		joinedNodeID;

	Array<Citizen>	citizens;
};