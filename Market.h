#pragma once
#include "Item.h"

struct Basket
{
	int		price;
	int		num;
};

struct Shelf
{
	int		itemType;
	Array<Basket>	baskets;

};

struct Store
{
	String	name;
	int		joinedCompanyID;
	Array<Shelf> shelves;
};

struct Market
{
	String	name;
	Array<Store> stores;
};