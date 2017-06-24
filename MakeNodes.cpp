#include "Planet.h"

void	Planet::loadData()
{
	iData.clear();
	vData.clear();
	JSONReader json(L"Assets/ItemData.json");
	for (auto& item : json[L"ItemData"].getArray())
		iData.push_back(item);
	for (auto& vehicle : json[L"VehicleData"].getArray())
		vData.push_back(vehicle);
}
bool	Planet::readBinary(const String& _filePath)
{
	nodes.clear();
	paths.clear();
	BinaryReader reader(_filePath);
	int	nodesSize;
	int pathsSize;
	reader.read(nodesSize);
	for (int i = 0; i < nodesSize; i++)
	{
		Vec3 ePos;
		reader.read(ePos);
		nodes.push_back(Node(i, Pos(ePos)));
	}
	reader.read(pathsSize);
	for (int i = 0; i < pathsSize; i++)
	{
		int parentNodeID;
		int childNodeID;
		reader.read(parentNodeID);
		reader.read(childNodeID);
		nodes[parentNodeID].paths.push_back(Path(parentNodeID, childNodeID));
	}
	for (auto& n : nodes)
	{
		for (auto& p : n.paths)
		{
			p.id = int(paths.size());
			p.len = nodes[p.childNodeID].pos.ePos.distanceFrom(n.pos.ePos);
			paths.push_back(&p);
		}
	}
	return true;
}

void	Planet::makeNodeData(const double& _interval, const String& _outputFilePath)
{
	Stopwatch stopwatch(true);
	//Nodeの配置
	for (double y = -HalfPi + _interval; y < HalfPi; y += _interval)
	{
		const int num = int(TwoPi / (_interval / Cos(y)));
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
			if (&n1 != &n2 && n1.pos.ePos.distanceFrom(n2.pos.ePos) < _interval*1.41)
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

		//ロードエフェクト
		if (stopwatch.ms() > 16)
		{
			stopwatch.restart();
			Window::SetTitle(L"ノードのランダム移動中", int(100.0*double(i) / double(nodes.size())), L"%");
			if (!System::Update()) return;
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

	//データの保存、読み込み
	{
		BinaryWriter writer(_outputFilePath);
		writer.write(int(nodes.size()));
		for (auto& n : nodes)
			writer.write(n.pos.ePos);
		writer.write(int(paths.size()));
		for (auto& p : paths)
		{
			writer.write(p->parentNodeID);
			writer.write(p->childNodeID);
		}
		writer.close();
	}
}
