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
		if (getHeight(pos) < 0) o = Palette::Blue;
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
			nodes.push_back(Node(Pos(Vec2(x, y))));

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
				n1.paths.push_back(Path(&n1, &n2));

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
			const double d = w.child->pos.ePos.distanceFrom(w.parent->pos.ePos);
			if (len == 0.0 || len > d) len = d;
		}
		Vec2 p = RandomVec2(Random(len / 2.0));
		p.x /= Cos(n.pos.mPos.y);
		n.pos.set(n.pos.mPos.movedBy(p));
		n.isSea = getHeight(n.pos.ePos) < 0 ? true : false;

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
		for (auto& w : n.paths)
			w.len = w.child->pos.ePos.distanceFrom(w.parent->pos.ePos);

	Window::SetTitle(L"Age of Discovery");
}

double	Planet::getHeight(const Pos& _pos) const
{
	return heightNoise.octaveNoise(_pos.ePos, 10);
}
