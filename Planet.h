#pragma once
#include <Siv3D.hpp>
#include "Node.h"
#include "City.h"
#include "Route.h"

struct Pos;

struct Planet
{
	Planet(const int& _size, const double& _nodeInterval);
	void	create();
	void	recreate();
	void	recreate(const int& _size, const double& _nodeInterval);
	void	update();
	void	draw() const;
	void	makeAllRoute();
	double	getHeight(const Pos& _pos) const;

	Array<Node>		nodes;
	Array<City>		cities;
	Array<Citizen>	citizens;
	Array<Route>	routes;
	int		size;
	double	nodeInterval;
	Mat3x2	transform;
	PerlinNoise heightNoise;
	Image	mapImage;
	Texture mapTexture;
	Font	font;
};