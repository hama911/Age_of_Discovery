#pragma once
#include <Siv3D.hpp>
#include "Node.h"
#include "Item.h"

struct VData
{
	VData(const JSONValue& _json)
	{
		name = _json[L"Name"].getOr<String>(L"none");
		description = _json[L"Description"].getOr<String>(L"noData");
		speed = _json[L"Speed"].getOr<double>(0.01);
		range = _json[L"Range"].getOr<double>(1000);
		space = _json[L"Space"].getOr<double>(1000);
		isShip = _json[L"IsShip"].getOr<bool>(true);
	}

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
	Vehicle(const int& _type, const int& _stayedInNodeID)
		: progress(0.0)
		, inProcessRouteID(-1)
		, stayedInNodeID(_stayedInNodeID)
		, joinedNodeID(_stayedInNodeID)
		, type(_type)
		, stock(Item())
	{}


	int		type;
	double	progress;
	int		inProcessRouteID;
	int		stayedInNodeID;
	int		joinedNodeID;
	Stock	stock;
};

struct Company
{
	Company(const int& _id)
		: id(_id)
		, name(L"")
		, isMunicipal(false)
	{}

	int		id;
	String	name;
	int		money;
	bool	isMunicipal;
	Array<Vehicle> vehicles;
};