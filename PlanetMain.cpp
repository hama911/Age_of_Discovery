#include "Planet.h"

void	Planet::draw() const
{
	Graphics2D::SetTransform(transform);
	mapTexture.resize(Vec2(Pi * 2, Pi)).draw(Vec2(-Pi, -HalfPi));

	//region‚Ì•`‰æ
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

	//Ocean‚Ì•`‰æ
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

	//ƒ‹[ƒg‚Ì•`‰æ
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

	//Vehicle‚Ì•`‰æ
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
						if (Abs(pos1.x - pos2.x) > Pi)
						{
							if (pos1.x < 0) pos2.moveBy(-TwoPi, 0);
							else pos1.moveBy(-TwoPi, 0);
						}
						const double theta = Atan2((pos2 - pos1).y, (pos2 - pos1).x);
						Triangle(0.02).rotated(theta + HalfPi).movedBy(pos1.lerp(pos2, len / p->len)).drawFrame(0.01, Palette::Black).draw(HSV(vData[v.type].id * 72));
						//Circle(pos1.lerp(pos2, len / p->len), 0.005).draw(Palette::Yellow);
						break;
					}
					else len -= p->len;
				}
			}
		}
	}

	//“ss‚Ì•`‰æ
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

	//City‚Ìî•ñ•\¦
	if (selectedCityID != -1)
	{
		const auto& c = cities[selectedCityID];
		Circle(nodes[c.joinedNodeID].pos.mPos, 0.02).draw(Palette::Pink);

		Vec2 pos(0, 0);
		drawInfoBox(pos, Vec2(480, 24));
		font(c.name).drawCenter(pos + Vec2(240, 12));
		pos.moveBy(0, 24);

		drawInfoBox(pos, Vec2(160, 24));
		font(L"‘lŒû:", c.citizens.size(), L"l").drawCenter(pos + Vec2(80, 12));
		pos.moveBy(0, 24);

		drawInfoBox(pos, Vec2(160, 48));
		font(L"¶Y•¨:", iData[c.canProduceItemType].name).draw(pos + Vec2(12, 0));

		TradeLog tradeLog = c.market.getTradeRatePrevious(c.canProduceItemType);
		if (tradeLog.num > 0) font(L"•½‹Ï‰¿Ši:", tradeLog.sum / tradeLog.num).draw(pos + Vec2(12, 24));
		else font(L"•½‹Ï‰¿Ši:").draw(pos + Vec2(12, 24));
		pos.moveBy(0, 48);

		//¤“X‚Ìî•ñ
		pos = Vec2(160, 24);
		drawInfoBox(pos, Vec2(320, 24));
		font(L"¤“X").drawCenter(pos + Vec2(160, 12));
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
				drawInfoBox(Mouse::PosF(), Vec2(160, 24 * s.shelves.size()));
				for (const auto& h : s.shelves)
				{
					if (h.previous.num > 0)
					{
						font(iData[h.itemType].name, h.previous.sum / h.previous.num).draw(pos + Vec2(12, 0));
					}
					else font(iData[h.itemType].name).draw(pos + Vec2(12, 0));
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

	Mouse::SetTransform(transform);

	//City‚Ì‘I‘ğ
	if (Input::MouseL.clicked)
	{
		selectedCityID = -1;
		for (const auto& c : cities)
		{
			if (Circle(nodes[c.joinedNodeID].pos.mPos, c.getDrawSize()).mouseOver) selectedCityID = c.id;
		}
	}

	//Companies‚ÌXV
	for (auto& c : companies)
	{
		for (auto& v : c.vehicles)
		{
			if (v.inProcessRouteID == -1)
			{
				bool flag = false;
				for (auto& r : routes)
					if (r.originNodeID == v.stayedInNodeID && r.isSeaRoute == vData[v.type].isShip && r.totalLength < vData[v.type].range) flag = true;
				if (flag)
				{
					for (;;)
					{
						auto& r = routes[Random(int(routes.size() - 1))];
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
				v.progress += vData[v.type].speed;
				auto& r = routes[v.inProcessRouteID];
				if (v.progress >= r.totalLength)
				{
					v.inProcessRouteID = -1;
					v.stayedInNodeID = r.destinationNodeID;
					v.progress = 0.0;
				}
			}
		}
	}

	//City‚ÌXV
	for (auto& t : cities)
	{
		t.time += timeSpeed;
		if (t.time >= 1.0)
		{
			t.time -= 1.0;
			//Market‚Ì´Z
			for (auto& s : t.market.stores)
			{
				for (auto& h : s.shelves)
				{
					h.previous.num = h.today.num;
					h.previous.sum = h.today.sum;
					h.today.num = 0;
					h.today.sum = 0;
				}
			}
		}
		//Citizens‚ÌXV
		for (auto& u : t.citizens)
		{
			//w“ü‚·‚é¤•i‚Ìí—Ş
			auto i = Random(int(iData.size() - 1));

			//w“ü‚µ‚És‚­¤“X
			if (!t.market.stores.empty())
			{
				auto& s = t.market.stores[Random(int(t.market.stores.size() - 1))];

				for (auto& h : s.shelves)
				{
					if (h.itemType == i && !h.baskets.empty() && h.baskets.front().price < u.income)
					{
						//w“ü
						h.today.num++;
						h.today.sum += h.baskets.front().price;
						h.baskets.erase(h.baskets.begin());
						break;
					}
				}
			}
		}

		//Store‚Ì‰¿ŠiXV
		for (auto& s : t.market.stores)
		{
			for (auto& h : s.shelves)
			{
				for (auto& b : h.baskets)
				{
					b.price = Max(b.price - 10, 0);
				}
			}
		}

		//¶Y•¨
		auto i = Item();
		i.num = 5;
		i.type = t.canProduceItemType;
		//Market‚ÌItem’Ç‰Á
		for (auto& s : t.market.stores)
		{
			bool flag = true;
			for (const auto& h : s.shelves) if (h.itemType == i.type) flag = false;
			if (flag) s.shelves.push_back(Shelf(i.type));

			for (auto& h : s.shelves)
			{
				if (h.itemType == i.type)
				{
					if (h.previous.num > 0) h.baskets.push_back(Basket(i, Random(h.previous.sum / h.previous.num, 10000)));
					else h.baskets.push_back(Basket(i, 10000));
				}
			}
		}
	}


	//ƒJƒƒ‰‚ÌXV
	Mouse::ClearTransform();
	transform = transform.scale(1.0 - double(Mouse::Wheel()) / 10.0, Mouse::PosF());
}

void	Planet::makeAllRoute()
{
	Array<Node*> nodeTemp;
	for (auto& c : cities)
	{
		nodeTemp.clear();

		//Å‰‚ÌNode‚Ì“o˜^
		nodeTemp.push_back(&nodes[c.joinedNodeID]);
		nodes[c.joinedNodeID].isScaned = true;

		//ƒ‹[ƒg‚ÌŒ`¬
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

		//ƒ‹[ƒg‚Ì“o˜^
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

		//ƒŠƒZƒbƒg
		for (auto& n : nodeTemp)
		{
			n->cost = 0.0;
			n->isInQueue = false;
			n->isScaned = false;
			n->fromNodeID = -1;
		}
	}
}

Vehicle::Vehicle(const int& _id)
	: id(_id), progress(0.0), inProcessRouteID(-1), stayedInNodeID(-1) {}
Company::Company(const int& _id)
	: id(_id), name(L"") {}