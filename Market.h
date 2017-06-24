#pragma once
#include "Item.h"

//独自の取引ログ
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

	int	getNumStock() const
	{
		int num = 0;
		for (auto& b : baskets) num += b.item.num;
		return num;
	}
	bool	buy()
	{
		if (baskets.empty()) return false;
		today.num++;
		today.sum += baskets.front().price;
		baskets.front().item.num--;
		if (baskets.front().item.num == 0) baskets.erase(baskets.begin());
		return true;
	}
	int		previousStock;	//前日最終在庫数
	int		itemType;
	Array<Basket>	baskets;

	TradeLog	previous;	//前日
	TradeLog	today;		//本日
};

struct Store
{
	bool	sellItem(const Basket& _basket)
	{
		//棚があるかどうか
		bool flag = true;
		for (const auto& h : shelves) if (h.itemType == _basket.item.type) flag = false;
		if (flag) shelves.push_back(Shelf(_basket.item.type));

		for (auto& h : shelves)
		{
			if (h.itemType == _basket.item.type)
			{
				if (h.baskets.empty())  h.baskets.push_back(_basket);
				else
				{
					for (auto i = h.baskets.begin(); ; i++)
					{
						if (i->price > _basket.price)
						{
							h.baskets.emplace(i, _basket);
							break;
						}
						else if (i == h.baskets.end())
						{
							h.baskets.push_back(_basket);
							break;
						}
					}
				}
				return true;
			}
		}
		return false;
	}

	Shelf&	getShelf(const int& _itemType)
	{
		for (auto& h : shelves) if (h.itemType == _itemType) return h;
		shelves.push_back(Shelf(_itemType));
		return shelves.back();
	}

	String	name;
	int		joinedCompanyID;
	Array<Shelf>	shelves;
	Array<Stock>	stocks;
	bool	isMunicipal;
};

struct Market
{
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
	int	getNumStock(const int& _itemType) const
	{
		int num = 0;
		for (auto& s : stores)
			for (auto& h : s.shelves)
				if (h.itemType == _itemType) for (auto& b : h.baskets) num += b.item.num;
		return num;
	}

	Array<Store> stores;
};