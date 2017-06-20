#pragma once
#include <Siv3D.hpp>

struct Pos;
struct City;
struct Planet
{
	Planet(const int& _size, const double& _nodeInterval);
	void	create();
	void	recreate();
	void	recreate(const int& _size, const double& _nodeInterval);
	void	update();
	void	draw() const;
	double	getHeight(const Pos& _pos) const;

	int		size;
	double	nodeInterval;
	Mat3x2	transform;
	PerlinNoise heightNoise;
	Image	mapImage;
	Texture mapTexture;
	Font	font;
};