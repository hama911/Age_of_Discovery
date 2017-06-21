#pragma once
#include<Siv3D.hpp>

struct Node;

struct Citizen
{
	String	name;
	int		joinedCityID;

};

struct City
{
	City(const int& _id);

	int		id;

	String	name;
	int		numCitizens;
	int		joinedNodeID;

	Array<Citizen>	citizens;
};