#pragma once
#include<Siv3D.hpp>
#include "Market.h"

struct Node;

struct Citizen
{
	Citizen() : timer(Random(1.0)) {}

	String	name;
	int		joinedCityID;
	double	timer;
	int		income;
};

struct City
{
	City(const int& _id) : id(_id), money(0) {}

	double	getDrawSize() const { return Log2(citizens.size())*0.0025; }


	double	time;
	int		id;
	int		canProduceItemType;
	int		money;
	Market	market;
	String	name;
	int		joinedNodeID;
	Array<Citizen>	citizens;
};