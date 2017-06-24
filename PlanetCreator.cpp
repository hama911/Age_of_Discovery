#include "Planet.h"
#include "Node.h"
#include "Pos.h"

Planet::Planet(const int& _size)
	: size(_size), mapImage(size * 2, size)
{
	selectedCityID = -1;
	loadData();
	create();
}

void	Planet::recreate()
{
	nodes.clear();
	cities.clear();
	citizens.clear();
	create();
}
void	Planet::makeMapTexture()
{
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
			Window::SetTitle(L"マップの生成中 ", (p.y*size * 2 + p.x), L"/", (size*size * 2));
			if (!System::Update()) return;
		}
	}
	mapTexture = Texture(mapImage);
}
void	Planet::create()
{
	timeSpeed = 0.01;
	font = Font(12);
	transform = Mat3x2::Identity().scale(200).translate(Window::Center());
	heightNoise = PerlinNoise(Random(UINT32_MAX - 1));

	//Nodeの生成
	readBinary(L"nodes.bin");

	//マップの生成
	makeMapTexture();

	//全Nodeの陸海設定の更新
	for (auto& n : nodes) n.isSea = getHeight(n.pos.ePos) < 0.60 ? true : false;
	for (auto& n : nodes)
	{
		n.isOcean = n.isSea;
		for (auto& w : n.paths)
		{
			auto& childNode = nodes[w.childNodeID];
			if (!n.isSea && childNode.isSea) n.isCoast = true;
			if (n.isSea && !childNode.isSea) n.isOcean = false;
			n.isOcean = getHeight(n.pos.ePos) < 0.50 ? true : false;
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
	int numCities = 50;
	for (;;)
	{
		auto& n = nodes[Random(nodes.size() - 1)];
		if (n.joinedRegionID != -1 && !regions[n.joinedRegionID].hasCity && n.isCoast)
		{
			cities.push_back(int(cities.size()));
			auto& c = cities.back();
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
		do c.joinedNodeID = Random(int(nodes.size()) - 1);
		while (nodes[c.joinedNodeID].ownCityID != -1 || nodes[c.joinedNodeID].isSea);
		nodes[c.joinedNodeID].ownCityID = c.id;
	}

	//Citizensの設定
	for (auto& t : cities)
	{
		int numCitizens = Random(100, 1000);
		for (int i = 0; i < numCitizens; i++)
		{
			t.citizens.push_back(Citizen());
			auto& s = t.citizens.back();
			s.income = int(Pow(Random(1.0), 1)*10000.0);
			s.joinedCityID = t.id;
			s.name = L"ボブ";
		}
	}

	//Cityに名前を設定
	CSVReader cityNameCSV(L"Assets/CityName.csv");
	if (cityNameCSV)
	{
		Array<String> names;
		for (int i = 0; i < cityNameCSV.rows; i++)
			names.push_back(cityNameCSV.getOr<String>(i, 0, L""));
		for (auto& c : cities)
		{
			const int i = Random(int(names.size() - 1));
			c.name = names[i];
			names.erase(names.begin() + i);
			if (names.empty()) break;
		}
	}

	//City諸設定
	for (auto& c : cities)
	{
		c.time = nodes[c.joinedNodeID].pos.mPos.x / TwoPi + 0.5;
		c.canProduceItemType = Random(int(iData.size() - 1));
	}

	//Companyの生成
	companies.clear();
	const int	numCompanies = 150;
	for (int i = 0; i < numCompanies; i++)
		companies.push_back(int(companies.size()));

	//Companyに名前を設定
	CSVReader companyNameCSV(L"Assets/CompanyName.csv");
	if (companyNameCSV)
	{
		Array<String> names;
		for (int i = 0; i < companyNameCSV.rows; i++)
			names.push_back(companyNameCSV.getOr<String>(i, 0, L""));
		for (auto& c : companies)
		{
			const int i = Random(int(names.size() - 1));
			c.name = names[i];
			names.erase(names.begin() + i);
			if (names.empty()) break;
		}
	}

	//Companyの配置
	for (auto& c : companies)
	{
		auto& t = cities[Random(int(cities.size() - 1))];
		//Storeの配置
		t.market.stores.push_back(Store());
		auto& s = t.market.stores.back();
		s.name = Format(c.name + L"-直営店");
		s.joinedCompanyID = c.id;
		s.isMunicipal = false;
		for (auto& t2 : cities)
		{
			if (t2.id != t.id && RandomBool(0.0))
			{
				t2.market.stores.push_back(Store());
				auto& s2 = t2.market.stores.back();
				s2.name = Format(c.name + L"-支店");
				s2.joinedCompanyID = c.id;
				s2.isMunicipal = false;

			}
		}

		//Vehicleの追加
		if (nodes[t.joinedNodeID].isCoast)
		{
			c.vehicles.push_back(int(c.vehicles.size()));
			auto& v = c.vehicles.back();
			v.stayedInNodeID = t.joinedNodeID;
			v.type = 2;
			v.joinedCityID = t.joinedNodeID;

			c.vehicles.push_back(int(c.vehicles.size()));
			auto& v2 = c.vehicles.back();
			v2.stayedInNodeID = t.joinedNodeID;
			v2.type = 0;
			v2.joinedCityID = t.joinedNodeID;
		}
		else
		{
			c.vehicles.push_back(int(c.vehicles.size()));
			auto& v = c.vehicles.back();
			v.stayedInNodeID = t.joinedNodeID;
			v.type = 0;
			v.joinedCityID = t.joinedNodeID;
		}
	}

	//市営商会の設置
	for (auto& t : cities)
	{
		companies.push_back(Company(int(companies.size())));
		auto& c = companies.back();
		c.name = Format(t.name, L"市営商会");
		c.isMunicipal = true;
		t.market.stores.push_back(Store());
		auto& s = t.market.stores.back();
		s.joinedCompanyID = c.id;
		s.isMunicipal = true;
		s.name = Format(L"[市営]", iData[t.canProduceItemType].name, L"問屋");

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