#pragma once
#include <Siv3D.hpp>
#include "Node.h"

struct Mission
{

};

struct VehicleD
{
	VehicleD(const int& _id);
	int		id;
	String	name;
	String	description;
	double	range;
	double	space;
	double	speed;
	bool	isShip;
	
};

struct Vehicle
{
	Vehicle(const int& _id);
	int		type;
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