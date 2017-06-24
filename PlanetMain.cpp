#include "Planet.h"

void	Planet::draw() const
{
	Graphics2D::SetTransform(transform);
	mapTexture.resize(Vec2(Pi * 2, Pi)).draw(Vec2(-Pi, -HalfPi));

	//regionÇÃï`âÊ
	if (Input::KeyT.pressed)
	{
		for (const auto& n : nodes)
		{
			if (n.joinedRegionID != -1)
			{
				const Color color = HSV(n.joinedRegionID * 30);
				Circle(n.pos.mPos, 0.004).draw(color);
				for (const auto& p : n.paths)
				{
					//Circle(n.pos.mPos,p.len).draw(color);
					const auto& n1 = nodes[p.parentNodeID];
					const auto& n2 = nodes[p.childNodeID];
					if (n2.joinedRegionID == n.joinedRegionID)
					{
						if (Abs(n1.pos.mPos.x - n2.pos.mPos.x) > Pi)
						{
							if (n1.pos.mPos.x < 0) Line(n1.pos.mPos, n2.pos.mPos.movedBy(-TwoPi, 0)).draw(0.004, color);
							else Line(n1.pos.mPos.movedBy(-TwoPi, 0), n2.pos.mPos).draw(0.004, color);
						}
						else Line(n1.pos.mPos, n2.pos.mPos).draw(0.004, color);
					}
				}
			}
		}
	}

	//OceanÇÃï`âÊ
	if (Input::KeyU.pressed)
	{
		for (const auto& n : nodes)
		{
			if (!n.isOcean && n.isSea)
			{
				for (const auto& p : n.paths)
				{
					const auto& childNode = nodes[p.childNodeID];
					if (!childNode.isOcean && childNode.isSea)
					{
						const auto& n1 = n;
						const auto& n2 = childNode;
						const auto& color = Palette::Green;
						if (n2.joinedRegionID == n.joinedRegionID)
						{
							if (Abs(n1.pos.mPos.x - n2.pos.mPos.x) > Pi)
							{
								if (n1.pos.mPos.x < 0) Line(n1.pos.mPos, n2.pos.mPos.movedBy(-TwoPi, 0)).draw(0.004, color);
								else Line(n1.pos.mPos.movedBy(-TwoPi, 0), n2.pos.mPos).draw(0.004, color);
							}
							else Line(n1.pos.mPos, n2.pos.mPos).draw(0.004, color);
						}
					}
				}
			}
		}
	}

	//ÉãÅ[ÉgÇÃï`âÊ
	if (Input::KeyY.pressed)
	{
		for (const auto& r : routes)
		{
			for (const auto& pID : r.pathIDs)
			{
				const auto& p = paths[pID];
				const auto& n1 = nodes[p->childNodeID];
				const auto& n2 = nodes[p->parentNodeID];
				const auto& color = r.isSeaRoute ? Palette::Gray : Palette::White;
				if (Abs(n1.pos.mPos.x - n2.pos.mPos.x) > Pi)
				{
					if (n1.pos.mPos.x < 0) Line(n1.pos.mPos, n2.pos.mPos.movedBy(-TwoPi, 0)).draw(0.002, color);
					else Line(n1.pos.mPos.movedBy(-TwoPi, 0), n2.pos.mPos).draw(0.002, color);
				}
				else Line(n1.pos.mPos, n2.pos.mPos).draw(0.002, color);
			}
		}
	}

	//VehicleÇÃï`âÊ
	for (const auto& c : companies)
	{
		for (const auto& v : c.vehicles)
		{
			if (v.inProcessRouteID != -1)
			{
				double len = v.progress;
				const auto& r = routes[v.inProcessRouteID];
				for (const auto& pID : r.pathIDs)
				{
					const auto& p = paths[pID];
					if (len < p->len)
					{
						auto pos1 = nodes[p->parentNodeID].pos.mPos;
						auto pos2 = nodes[p->childNodeID].pos.mPos;
						//const Color color = HSV(vData[v.type].id * 72);
						const Color color = v.stock.item.num == 0 ? Palette::Black : HSV(v.stock.item.type * 30);
						if (Abs(pos1.x - pos2.x) > Pi)
						{
							if (pos1.x < 0) pos2.moveBy(-TwoPi, 0);
							else pos1.moveBy(-TwoPi, 0);
						}
						const double theta = Atan2((pos2 - pos1).y, (pos2 - pos1).x);
						Triangle(0.02).rotated(theta + HalfPi).movedBy(pos1.lerp(pos2, len / p->len)).drawFrame(0.01, Palette::Black).draw(color);
						//Circle(pos1.lerp(pos2, len / p->len), 0.005).draw(Palette::Yellow);
						break;
					}
					else len -= p->len;
				}
			}
		}
	}

	//ìsésÇÃï`âÊ
	for (const auto& c : cities)
	{
		const auto& n = nodes[c.joinedNodeID];
		const double& s = c.getDrawSize();
		const Color& color = HSV(c.canProduceItemType * 30);
		if (n.isCoast)
		{
			Circle(n.pos.mPos, s).drawFrame(s*0.25, 0.0, color);
			Circle(n.pos.mPos, s*0.5).draw(color);
		}
		else Circle(n.pos.mPos, s).draw(color);
	}


	Graphics2D::ClearTransform();

	//CityÇÃèÓïÒï\é¶
	if (selectedCityID != -1)
	{
		const auto& c = cities[selectedCityID];
		Circle(nodes[c.joinedNodeID].pos.mPos, 0.02).draw(Palette::Pink);

		Vec2 pos(0, 0);
		drawInfoBox(pos, Vec2(480, 24));
		font(c.name).drawCenter(pos + Vec2(240, 12));
		pos.moveBy(0, 24);

		drawInfoBox(pos, Vec2(160, 24));
		font(L"ëçêlå˚:", c.citizens.size(), L"êl").drawCenter(pos + Vec2(80, 12));
		pos.moveBy(0, 24);

		drawInfoBox(pos, Vec2(160, 24));
		font(int(c.time * 24), L":", int((c.time*24.0 - int(c.time*24.0))*60.0)).draw(pos + Vec2(24, 0));
		pos.moveBy(0, 24);

		drawInfoBox(pos, Vec2(160, 48));
		font(L"ê∂éYï®:", iData[c.canProduceItemType].name).draw(pos + Vec2(12, 0));

		TradeLog tradeLog = c.market.getTradeRatePrevious(c.canProduceItemType);
		if (tradeLog.num > 0) font(L"ïΩãœâøäi:", tradeLog.sum / tradeLog.num).draw(pos + Vec2(12, 24));
		else font(L"ïΩãœâøäi:").draw(pos + Vec2(12, 24));
		pos.moveBy(0, 48);

		drawInfoBox(pos, Vec2(160, 12));
		pos.moveBy(0, 12);

		for (int i = 0; i < iData.size(); i++)
		{
			drawInfoBox(pos, Vec2(160, 48));
			font(iData[i].name, L" ç›å…", c.market.getNumStock(i)).draw(pos + Vec2(12, 0));
			TradeLog tradeLog2 = c.market.getTradeRatePrevious(i);
			if (tradeLog2.num > 0) font(L"ïΩãœâøäi:", tradeLog2.sum / tradeLog2.num).draw(pos + Vec2(12, 24));
			else font(L"ïΩãœâøäi:").draw(pos + Vec2(12, 24));
			pos.moveBy(0, 48);
		}

		//è§ìXÇÃèÓïÒ
		pos = Vec2(160, 24);
		drawInfoBox(pos, Vec2(320, 24));
		font(L"è§ìX").drawCenter(pos + Vec2(160, 12));
		pos = Vec2(160, 48);
		for (auto& s : c.market.stores)
		{
			drawInfoBox(pos, Vec2(320, 24));
			font(s.name).draw(pos + Vec2(12, 0));
			pos.moveBy(0, 24);
		}
		pos = Vec2(160, 48);
		for (const auto& s : c.market.stores)
		{
			if (RectF(pos, Vec2(320, 24)).mouseOver)
			{
				pos = Mouse::PosF();
				drawInfoBox(Mouse::PosF(), Vec2(240, 24 * s.shelves.size()));
				for (const auto& h : s.shelves)
				{
					font(iData[h.itemType].name, h.getNumStock(), L"å¬ ", h.baskets.front().price).draw(pos + Vec2(12, 0));
					pos.moveBy(0, 24);
				}
				break;
			}
			pos.moveBy(0, 24);
		}

	}

}

void	Planet::drawInfoBox(const Vec2& _pos, const Vec2& _size) const
{
	RectF(_pos, _size).draw(Palette::Darkblue).drawFrame(1, 0, Palette::Skyblue);
}

void	Planet::update()
{
	if (Input::KeyF2.clicked) timeSpeed = Min(timeSpeed * 2.0, 1.0);
	if (Input::KeyF1.clicked) timeSpeed = Max(timeSpeed / 2.0, 0.01);
	Mouse::SetTransform(transform);

	//CityÇÃëIë
	if (Input::MouseL.clicked)
	{
		selectedCityID = -1;
		for (const auto& c : cities)
		{
			if (Circle(nodes[c.joinedNodeID].pos.mPos, c.getDrawSize()).mouseOver) selectedCityID = c.id;
		}
	}

	//CompaniesÇÃçXêV
	for (auto& c : companies)
	{
		for (auto& v : c.vehicles)
		{
			if (v.inProcessRouteID == -1)
			{
				bool flag = false;
				for (auto& r : routes)
				{
					if (v.joinedNodeID != v.stayedInNodeID && r.destinationNodeID != v.joinedNodeID) continue;
					if (r.originNodeID == v.stayedInNodeID && r.isSeaRoute == vData[v.type].isShip && r.totalLength < vData[v.type].range) flag = true;
				}
				if (flag)
				{
					for (;;)
					{
						auto& r = routes[Random(int(routes.size() - 1))];
						if (v.joinedNodeID != v.stayedInNodeID && r.destinationNodeID != v.joinedNodeID) continue;
						if (r.originNodeID == v.stayedInNodeID && r.isSeaRoute == vData[v.type].isShip && r.totalLength < vData[v.type].range)
						{
							v.inProcessRouteID = r.id;
							v.progress = 0.0;
							break;
						}
					}
				}
			}
			else
			{
				v.progress += vData[v.type].speed*timeSpeed;
				auto& r = routes[v.inProcessRouteID];
				if (v.progress >= r.totalLength)
				{
					v.inProcessRouteID = -1;
					v.stayedInNodeID = r.destinationNodeID;
					v.progress = 0.0;

					auto& t = cities[nodes[v.stayedInNodeID].ownCityID];
					if (v.joinedNodeID == v.stayedInNodeID)	//â∆Ç…íÖÇ¢ÇΩÇÁ
					{
						if (v.stock.item.num > 0)
						{
							for (auto& s : t.market.stores)
							{
								if (c.id == s.joinedCompanyID)
								{
									s.stocks.push_back(v.stock);
								}
							}
							v.stock = Stock(Item());
						}
					}
					else	//çwì¸èàóù
					{
						for (auto& s : t.market.stores)
						{
							if (s.isMunicipal && !s.shelves.empty())
							{

								auto& h = s.shelves.front();
								for (int i = 0;; i++)
								{
									if (!h.baskets.empty() && i < 1000)
									{
										h.buy();
									}
									else
									{
										v.stock.day = int(r.totalLength / vData[v.type].speed) + 1;
										v.stock.sellEnabled = true;
										v.stock.item = Item(h.itemType, i);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//CityÇÃçXêV
	for (auto& t : cities)
	{
		t.time += timeSpeed;
		if (t.time >= 1.0)
		{
			t.time -= 1.0;
			//MarketÇÃê¥éZ
			for (auto& s : t.market.stores)
			{
				for (auto& h : s.shelves)
				{
					h.previousStock = h.getNumStock();
					h.previous.num = h.today.num;
					h.previous.sum = h.today.sum;
					h.today.num = 0;
					h.today.sum = 0;
				}
			}

			//StoreÇÃâøäiçXêV & è§ïií«â¡
			for (auto& s : t.market.stores)
			{
				for (auto& b : s.stocks)
				{
					if (b.sellEnabled)
					{
						auto numSell = b.item.num / b.day;
						b.item.num -= numSell;

						if (numSell > 0)
						{
							auto& h = s.getShelf(b.item.type);
							const int price = int((h.previous.sum + 1000000) / (h.previous.num + 100));
							s.sellItem(Basket(Item(b.item.type, numSell), price));
						}
						b.day--;
					}
				}
				Erase_if(s.stocks, [](Stock& b) {return b.item.num <= 0; });
			}

			//ê∂éYï®
			auto i = Item();
			i.num = t.market.getNumStock(t.canProduceItemType) > 5000 ? 0 : int(t.citizens.size() / 2);
			i.type = t.canProduceItemType;
			if (i.num > 0)
			{
				//MarketÇÃItemí«â¡
				for (auto& s : t.market.stores)
				{
					if (companies[s.joinedCompanyID].isMunicipal)
					{
						auto& h = s.getShelf(i.type);
						const int price = int((h.previous.sum + 1000000) / (h.previous.num + 100));
						s.sellItem(Basket(i, price));
					}
				}
			}
		}


		for (auto& s : t.market.stores)
		{
			for (auto& h : s.shelves)
			{
				for (auto& b : h.baskets)
				{
					b.price = Max(b.price - 20, 0);
				}
			}
		}

		//CitizensÇÃçXêV
		for (auto& u : t.citizens)
		{
			u.timer += timeSpeed*double(cities[u.joinedCityID].market.stores.size());
			for (;;)
			{
				if (u.timer < 1.0) break;
				u.timer -= 1.0;

				for (int i = 0; i < iData.size(); i++)
				{
					//çwì¸ÇµÇ…çsÇ≠è§ìX
					if (!t.market.stores.empty())
					{
						auto& s = t.market.stores[Random(int(t.market.stores.size() - 1))];

						for (auto& h : s.shelves)
						{
							if (h.itemType == i && !h.baskets.empty() && h.baskets.front().price < u.income)
							{
								//çwì¸
								h.buy();
								break;
							}
						}
					}
				}
			}
		}

	}


	//ÉJÉÅÉâÇÃçXêV
	Mouse::ClearTransform();
	transform = transform.scale(1.0 - double(Mouse::Wheel()) / 10.0, Mouse::PosF());
}

void	Planet::makeAllRoute()
{
	Array<Node*> nodeTemp;
	for (auto& c : cities)
	{
		nodeTemp.clear();

		//ç≈èâÇÃNodeÇÃìoò^
		nodeTemp.push_back(&nodes[c.joinedNodeID]);
		nodes[c.joinedNodeID].isScaned = true;

		//ÉãÅ[ÉgÇÃå`ê¨
		for (int i = 0; i < nodeTemp.size(); i++)
		{
			auto& n1 = nodeTemp[i];

			if (n1->fromNodeID != -1 && !n1->isSea && nodes[n1->fromNodeID].isSea) continue;

			n1->isInQueue = false;
			for (auto& p : n1->paths)
			{
				auto& n2 = nodes[p.childNodeID];

				if (n1->ownCityID != c.id && n2.ownCityID == -1 && (n1->isSea != n2.isSea)) continue;

				if (!n2.isScaned || n2.cost > n1->cost + p.len)
				{
					if (!n2.isInQueue) nodeTemp.push_back(&n2);
					n2.isScaned = true;
					n2.isInQueue = true;
					n2.cost = n1->cost + p.len;
					n2.fromNodeID = n1->id;
				}
			}
		}

		//ÉãÅ[ÉgÇÃìoò^
		for (auto& ct : cities)
		{
			if (&c != &ct && nodes[ct.joinedNodeID].fromNodeID != -1)
			{
				routes.push_back(int(routes.size()));
				auto& r = routes.back();

				auto* n = &nodes[ct.joinedNodeID];
				for (;;)
				{
					for (auto& p : n->paths)
					{
						if (n->fromNodeID == p.childNodeID)
						{
							r.pathIDs.push_back(p.id);
							r.totalLength += p.len;
							n = &nodes[n->fromNodeID];
							break;
						}
					}
					if (n->id == c.joinedNodeID) break;
				}

				r.destinationNodeID = c.joinedNodeID;
				r.originNodeID = ct.joinedNodeID;
				r.isSeaRoute = nodes[paths[r.pathIDs.front()]->childNodeID].isSea;
			}
		}

		//ÉäÉZÉbÉg
		for (auto& n : nodeTemp)
		{
			n->cost = 0.0;
			n->isInQueue = false;
			n->isScaned = false;
			n->fromNodeID = -1;
		}
	}
}
