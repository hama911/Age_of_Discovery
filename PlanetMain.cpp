#include "Planet.h"

void	Planet::draw() const
{
	Graphics2D::SetTransform(transform);
	mapTexture.resize(Vec2(Pi * 2, Pi)).draw(Vec2(-Pi, -HalfPi));

	//regionの描画
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

	//ルートの描画
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

	//Vehicleの描画
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
						Circle(pos1.lerp(pos2, len / p->len), 0.005).draw(Palette::Yellow);
						break;
					}
					else len -= p->len;
				}
			}
		}
	}

	//都市の描画
	for (const auto& c : cities)
	{
		const auto& n = nodes[c.joinedNodeID];
		Circle(n.pos.mPos, 0.01).draw(Palette::Red);
	}


	Graphics2D::ClearTransform();

	//Cityの情報表示
	if (selectedCityID != -1)
	{
		const auto& c = cities[selectedCityID];
		Circle(nodes[c.joinedNodeID].pos.mPos, 0.02).draw(Palette::Pink);

		Vec2 pos(0, 0);
		drawInfoBox(pos, Vec2(480, 24));
		font(c.name).drawCenter(pos + Vec2(240, 12));

		pos.moveBy(0, 24);
		drawInfoBox(pos, Vec2(160, 24));
		font(L"総人口:", c.numCitizens, L"人").drawCenter(pos + Vec2(80, 12));

	}

}

void	Planet::drawInfoBox(const Vec2& _pos, const Vec2& _size) const
{
	RectF(_pos, _size).draw(Palette::Darkblue).drawFrame(1, 0, Palette::Skyblue);
}

void	Planet::update()
{
	Mouse::SetTransform(transform);

	//Cityの選択
	if (Input::MouseL.clicked)
	{
		selectedCityID = -1;
		for (const auto& c : cities)
		{
			if (Circle(nodes[c.joinedNodeID].pos.mPos, 0.01).mouseOver) selectedCityID = c.id;
		}
	}

	//Companiesの更新
	for (auto& c : companies)
	{
		for (auto& v : c.vehicles)
		{
			if (v.inProcessRouteID == -1)
			{
				bool flag = false;
				for (auto& r : routes)
					if (r.originNodeID == v.stayedInNodeID) flag = true;
				if (flag)
				{
					for (;;)
					{
						auto& r = routes[Random(int(routes.size() - 1))];
						if (r.originNodeID == v.stayedInNodeID)
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
				v.progress += 0.01;
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

	//カメラの更新
	Mouse::ClearTransform();
	transform = transform.scale(1.0 - double(Mouse::Wheel()) / 10.0, Mouse::PosF());
}

void	Planet::makeAllRoute()
{
	Array<Node*> nodeTemp;
	for (auto& c : cities)
	{
		nodeTemp.clear();

		//最初のNodeの登録
		nodeTemp.push_back(&nodes[c.joinedNodeID]);
		nodes[c.joinedNodeID].isScaned = true;

		//ルートの形成
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

		//ルートの登録
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

		//リセット
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