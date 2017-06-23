#pragma once
#include <Siv3D.hpp>
#include "Node.h"
#include "City.h"
#include "Route.h"
#include "Company.h"
#include "Item.h"

struct Pos;

struct Region
{
	Region(const int& _id);
	int		id;
	String	name;
	int		numNodes;
	bool	hasCity;
};

struct Planet
{
	Planet(const int& _size, const double& _nodeInterval);
	void	create();
	void	recreate();
	void	recreate(const int& _size, const double& _nodeInterval);
	void	update();
	void	draw() const;
	void	makeAllRoute();
	void	drawInfoBox(const Vec2& _pos, const Vec2& _size) const;
	double	getHeight(const Pos& _pos) const;

	Array<IData>	iData;
	Array<VData>	vData;
	Array<Company>	companies;
	Array<Region>	regions;
	Array<Node>		nodes;
	Array<City>		cities;
	Array<Citizen>	citizens;
	Array<Route>	routes;
	Array<Path*>	paths;
	int		selectedCityID;
	int		size;
	double	nodeInterval;
	Mat3x2	transform;
	PerlinNoise heightNoise;
	Image	mapImage;
	Texture mapTexture;
	Font	font;
};