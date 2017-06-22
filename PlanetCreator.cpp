#include "Planet.h"
#include "Node.h"
#include "Pos.h"

Planet::Planet(const int& _size, const double& _nodeInterval)
	: size(_size), nodeInterval(_nodeInterval), mapImage(size * 2, size)
{
	selectedCityID = -1;
	create();
}

void	Planet::recreate()
{
	nodes.clear();
	cities.clear();
	citizens.clear();
	create();
}

void	Planet::recreate(const int& _size, const double& _nodeInterval)
{
	size = _size;
	nodeInterval = _nodeInterval;
	recreate();
}

VehicleD::VehicleD(const int& _id)
	: id(_id) {}
void	Planet::create()
{
	font = Font(12);
	transform = Mat3x2::Identity().scale(200).translate(Window::Center());
	heightNoise = PerlinNoise(Random(UINT32_MAX - 1));

	//Itemデータの読み込み
	JSONReader json(L"Assets/ItemData.json");
	for (auto& item : json[L"ItemData"].getArray())
	{
		itemD.push_back(ItemD());
		auto& i = itemD.back();
		i.name = item[L"Name"].getOr<String>(L"none");
		i.description = item[L"Description"].getOr<String>(L"noData");
		i.volume = item[L"volume"].getOr<double>(0.01);
	}
	for (auto& vehicle : json[L"VehicleData"].getArray())
	{
		vehicleD.push_back(VehicleD(int(vehicleD.size())));
		auto& v = vehicleD.back();
		v.name = vehicle[L"Name"].getOr<String>(L"none");
		v.description = vehicle[L"Description"].getOr<String>(L"noData");
		v.speed = vehicle[L"Speed"].getOr<double>(0.01);
		v.range = vehicle[L"Range"].getOr<double>(1000);
		v.space = vehicle[L"Space"].getOr<double>(1000);
		v.isShip = vehicle[L"IsShip"].getOr<bool>(true);
	}

	//マップの生成
	Stopwatch stopwatch(true);
	for (const auto& p : step(Size(size * 2, size)))
	{
		const Pos pos(Vec2(p.x - size, p.y - size / 2)*Pi / double(size));
		Color& o = mapImage[p.y][p.x];
		if (getHeight(pos) < 0.60) o = Palette::Blue;
		else o = Palette::Green;

		//ロードエフェクト
		if (stopwatch.ms() > 16)
		{
			stopwatch.restart();
			Window::SetTitle(L"マップの生成中", int(100.0*double(p.y*size * 2 + p.x) / double(size*size * 2)), L"%");
			if (!System::Update()) return;
		}
	}
	mapTexture = Texture(mapImage);

	//Nodeの配置
	for (double y = -HalfPi + nodeInterval; y < HalfPi; y += nodeInterval)
	{
		const int num = int(TwoPi / (nodeInterval / Cos(y)));
		const double d = TwoPi / double(num);
		const double a = Random(0.0, d);
		for (int i = 0; i < num; i++)
		{
			const double x = d*i - Pi + a;
			nodes.push_back(Node(int(nodes.size()), Pos(Vec2(x, y))));

			//ロードエフェクト
			if (stopwatch.ms() > 16)
			{
				stopwatch.restart();
				Window::SetTitle(L"ノードの配置中", int(100.0*double(i) / double(num)), L"%");
				if (!System::Update()) return;
			}
		}
	}

	//Nodeの相互接続
	for (int i = 0; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes.size(); j++)
		{
			auto& n1 = nodes[i];
			auto& n2 = nodes[j];
			if (&n1 != &n2 && n1.pos.ePos.distanceFrom(n2.pos.ePos) < nodeInterval*1.41)
				n1.paths.push_back(Path(i, j));

			//ロードエフェクト
			if (stopwatch.ms() > 16) {
				stopwatch.restart();
				Window::SetTitle(L"ノードの接続中", int(100 * double(i*nodes.size() + j) / double(nodes.size()*nodes.size())), L"%");
				if (!System::Update()) return;
			}
		}
	}

	//Nodeのランダム移動
	for (int i = 0; i < nodes.size(); i++) {
		auto& n = nodes[i];
		double len = 0.0;
		for (auto& w : n.paths) {
			const double d = nodes[w.childNodeID].pos.ePos.distanceFrom(nodes[w.parentNodeID].pos.ePos);
			if (len == 0.0 || len > d) len = d;
		}
		Vec2 p = RandomVec2(Random(len / 2.0));
		p.x /= Cos(n.pos.mPos.y);
		n.pos.set(n.pos.mPos.movedBy(p));
		n.isSea = getHeight(n.pos.ePos) < 0.60 ? true : false;

		//ロードエフェクト
		if (stopwatch.ms() > 16)
		{
			stopwatch.restart();
			Window::SetTitle(L"ノードのランダム移動中", int(100.0*double(i) / double(nodes.size())), L"%");
			if (!System::Update()) return;
		}
	}

	//全Pathの距離の登録
	for (auto& n : nodes)
	{
		for (auto& w : n.paths)
		{
			auto& childNode = nodes[w.childNodeID];
			if (!n.isSea && childNode.isSea) n.isCoast = true;
			w.len = childNode.pos.ePos.distanceFrom(n.pos.ePos);
		}
	}

	//Pathsに登録
	paths.clear();
	for (auto& n : nodes)
	{
		for (auto& p : n.paths)
		{
			p.id = int(paths.size());
			paths.push_back(&p);
		}
	}

	//regionの設定
	regions.clear();
	for (auto& n : nodes)
	{
		if (!n.isSea && n.joinedRegionID == -1)
		{
			regions.push_back(int(regions.size()));
			auto& r = regions.back();
			n.joinedRegionID = r.id;
			r.numNodes++;
			Array<Node*> nodeTemp;
			nodeTemp.push_back(&n);
			for (int i = 0; i < nodeTemp.size(); i++)
			{
				for (auto& p : nodeTemp[i]->paths)
				{
					auto& n2 = nodes[p.childNodeID];
					if (!n2.isSea && n2.joinedRegionID == -1)
					{
						n2.joinedRegionID = r.id;
						r.numNodes++;
						nodeTemp.push_back(&n2);
					}
				}
			}
		}
	}

	//Cityの配置
	int numCities = 100;
	for (;;)
	{
		auto& n = nodes[Random(nodes.size() - 1)];
		if (n.joinedRegionID != -1 && !regions[n.joinedRegionID].hasCity && n.isCoast)
		{
			cities.push_back(int(cities.size()));
			auto& c = cities.back();
			c.numCitizens = 100;
			c.joinedNodeID = n.id;
			nodes[c.joinedNodeID].ownCityID = c.id;
			regions[n.joinedRegionID].hasCity = true;
			numCities--;

			bool flag = true;
			for (const auto& nt : nodes)
			{
				if (nt.joinedRegionID != -1 && !regions[nt.joinedRegionID].hasCity) flag = false;
			}
			if (flag) break;
		}
	}

	for (int i = 0; i < numCities; i++) {
		cities.push_back(int(cities.size()));
		auto& c = cities.back();
		c.numCitizens = 100;
		do c.joinedNodeID = Random(int(nodes.size()) - 1);
		while (nodes[c.joinedNodeID].ownCityID != -1 || nodes[c.joinedNodeID].isSea);
		nodes[c.joinedNodeID].ownCityID = c.id;
	}

	//Cityに名前を設定
	CSVReader csv(L"Assets/CityName.csv");
	if (csv)
	{
		Array<String> names;
		for (int i = 0; i < csv.rows; i++)
			names.push_back(csv.getOr<String>(i, 0, L""));
		for (auto& c : cities)
		{
			const int i = Random(int(names.size() - 1));
			c.name = names[i];
			names.erase(names.begin() + i);
			if (names.empty()) break;
		}
	}

	//Companyの配置
	int	numCompanies = 20;
	for (int i = 0; i < numCompanies; i++)
	{
		companies.push_back(int(companies.size()));
		auto& c = companies.back();
		for (;;)
		{
			auto& t = cities[Random(int(cities.size() - 1))];
			if (nodes[t.joinedNodeID].isCoast)
			{
				int numVehicles = Random(3, 10);
				for (int j = 0; j < numVehicles; j++)
				{
					c.vehicles.push_back(int(c.vehicles.size()));
					auto& v = c.vehicles.back();
					v.stayedInNodeID = t.joinedNodeID;
					v.type = Random(int(vehicleD.size() - 1));
				}
				break;
			}
		}
	}

	//全ルートの生成
	routes.clear();
	makeAllRoute();

	Window::SetTitle(L"Age of Discovery");
}

double	Planet::getHeight(const Pos& _pos) const
{
	return (heightNoise.octaveNoise(_pos.ePos, 10) + 1.0)*0.5;
}

Region::Region(const int& _id)
	:id(_id), name(L""), numNodes(0), hasCity(false) {}