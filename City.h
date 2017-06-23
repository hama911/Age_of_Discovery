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
	double	getDrawSize() const { return Log2(citizens.size())*0.0025; }

	/*
	時間のシステム
	0.0から1.0まで
	*/
	double	time;

	int		id;
	int		canProduceItemType;
	int		money;
	Market	market;
	String	name;
	int		joinedNodeID;
	Array<Citizen>	citizens;
};