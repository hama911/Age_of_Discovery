#include "Planet.h"
#include "Node.h"
#include "Pos.h"

Planet::Planet(const int& _size, const double& _nodeInterval)
	: size(_size), nodeInterval(_nodeInterval), mapImage(size * 2, size)
{
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

void	Planet::create()
{
	font = Font(12);
	transform = Mat3x2::Identity().scale(200).translate(Window::Center());
	heightNoise = PerlinNoise(Random(UINT32_MAX - 1));

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

	//Cityの配置
	const int numCities = 50;
	for (int i = 0; i < numCities; i++) {
		cities.push_back(int(cities.size()));
		auto& c = cities.back();
		c.name = L"ミュンヘン";
		c.numCitizens = 100;
		do c.joinedNodeID = Random(int(nodes.size()) - 1);
		while (nodes[c.joinedNodeID].ownCityID != -1 || nodes[c.joinedNodeID].isSea);
		nodes[c.joinedNodeID].ownCityID = c.id;
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
