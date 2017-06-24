#pragma once
#include <Siv3D.hpp>

struct IData
{
	IData(const JSONValue& _json)
	{
		name = _json[L"Name"].getOr<String>(L"none");
		description = _json[L"Description"].getOr<String>(L"noData");
		volume = _json[L"volume"].getOr<double>(0.01);
	}
	String	name;
	String	description;
	double	volume;
};

struct Item
{
	Item() : type(-1), num(0) {}
	Item(const int& _type, const int& _num) : type(_type), num(_num) {}

	int		type;
	int		num;
};
struct Stock
{
	Stock(const Item& _item, const bool& _sellEnabled = false, const int& _day = 0)
		: item(_item)
		, day(_day)
		, sellEnabled(_sellEnabled)
	{}

	Item	item;
	int		day;			//îÑÇÈÇÃÇ…Ç©ÇØÇÈì˙êî
	bool	sellEnabled;	//îÑÇÈÇ©Ç«Ç§Ç©
};
