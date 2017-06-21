#include "Planet.h"

void	Planet::draw() const
{
	Graphics2D::SetTransform(transform);
	mapTexture.resize(Vec2(Pi * 2, Pi)).draw(Vec2(-Pi, -HalfPi));

	//都市の描画
	for (const auto& c : cities)
	{
		const auto& n = nodes[c.joinedNodeID];
		Circle(n.pos.mPos, 0.01).draw(Palette::Red);
	}

	//ルートの描画
	for (const auto& r : routes)
	{
		for (const auto& p : r.paths)
		{
			const auto& n1 = nodes[p->childNodeID];
			const auto& n2 = nodes[p->parentNodeID];
			if (Abs(n1.pos.mPos.x - n2.pos.mPos.x) > Pi)
			{
				if (n1.pos.mPos.x < 0) Line(n1.pos.mPos, n2.pos.mPos.movedBy(-TwoPi, 0)).draw(0.002);
				else Line(n1.pos.mPos.movedBy(-TwoPi, 0), n2.pos.mPos).draw(0.002);
			}
			else Line(n1.pos.mPos, n2.pos.mPos).draw(0.002);
		}
	}


	Graphics2D::ClearTransform();
}

void	Planet::update()
{
	Mouse::SetTransform(transform);


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
			n1->isInQueue = false;
			for (auto& p : n1->paths)
			{
				auto& n2 = nodes[p.childNodeID];

				if (n1->ownCityID != c.id && n2.ownCityID == -1 && (n1->isSea != n2.isSea)) continue;

				if (!n2.isInQueue && (!n2.isScaned || n2.cost > n1->cost + p.len))
				{
					n2.isScaned = true;
					n2.isInQueue = true;
					n2.cost = n1->cost + p.len;
					n2.fromNodeID = n1->id;
					nodeTemp.push_back(&n2);
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
				r.destinationNodeID = c.joinedNodeID;
				r.originNodeID = ct.joinedNodeID;

				auto* n = &nodes[ct.joinedNodeID];
				for (;;)
				{
					for (auto& p : n->paths)
					{
						if (n->fromNodeID == p.childNodeID)
						{
							r.paths.push_back(&p);
							r.totalLength += p.len;
							n = &nodes[n->fromNodeID];
							break;
						}
					}
					if (n->id == c.joinedNodeID) break;
				}
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