#pragma once
#include<Siv3D.hpp>
#include "Market.h"

struct Node;

struct Citizen
{
	Citizen(const int& _joinedCityID, const int& _income)
		: joinedCityID(_joinedCityID)
		, income(_income)
	{
		timer = Random(1.0);
	}

	int		joinedCityID;
	double	timer;
	int		income;
};

struct City
{
	City(const int& _id, const int& _joinedNodeID)
		: id(_id)
		, joinedNodeID(_joinedNodeID)
		, money(0) {}

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