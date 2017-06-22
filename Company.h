#pragma once
#include <Siv3D.hpp>
#include "Node.h"

struct Mission
{

};

struct VehicleD
{
	int		id;
	String	name;
	double	range;
	double	space;
	bool	isShip;
	
};

struct Vehicle
{
	Vehicle(const int& _id);
	int		id;
	double	progress;

	int		inProcessRouteID;
	int		stayedInNodeID;
};

struct Company
{
	Company(const int& _id);
	int		id;
	String	name;

	Array<Vehicle> vehicles;
};