#pragma once
#include "Item.h"

//�Ǝ��̎�����O
struct TradeLog
{
	TradeLog() : itemType(0), num(0), sum(0) {}
	TradeLog(const int& _itemType) : itemType(_itemType), num(0), sum(0) {}
	int	itemType;
	int	num;
	int	sum;
};

struct Basket
{
	Basket(const Item& _item, const int& _price)
		: price(_price), item(_item) {}
	int		price;
	Item	item;
};

struct Shelf
{
	Shelf(const int& _itemType)
		: itemType(_itemType) {}

	int		itemType;
	Array<Basket>	baskets;

	TradeLog	previous;	//�O��
	TradeLog	today;		//�{��
};

struct Store
{
	String	name;
	int		joinedCompanyID;
	Array<Shelf>	shelves;
	Array<Item>		items;
};

struct Market
{
	//�O���̎�����i���擾
	TradeLog	getTradeRatePrevious(const int& _itemType) const
	{
		TradeLog tradeLog(_itemType);
		for (auto& s : stores)
		{
			for (auto& h : s.shelves)
			{
				if (h.itemType == _itemType)
				{
					tradeLog.num += h.previous.num;
					tradeLog.sum += h.previous.sum;
					break;
				}
			}
		}
		return tradeLog;
	}

	String	name;
	Array<Store> stores;
};