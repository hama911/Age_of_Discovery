#pragma once
#include <Siv3D.hpp>

struct IData
{
	String	name;
	String	description;
	double	volume;
};

struct Item
{
	int		type;
	int		num;
};