#include "Planet.h"
#include "Pos.h"

Planet::Planet(const int& _size, const double& _nodeInterval)
	: size(_size), nodeInterval(_nodeInterval), mapImage(size * 2, size)
{
	create();
}

void	Planet::recreate()
{
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
		if (stopwatch.ms() > 16) {
			stopwatch.restart();
			const Size barSize(320, 20);
			const double Progress = double(p.y*size * 2 + p.x) / double(size*size * 2);
			Rect(Window::Center() - barSize / 2, barSize).drawFrame(0, 2, Palette::White);
			Rect(Window::Center() - barSize / 2, int(barSize.x*Progress), barSize.y).draw(Palette::Gray);
			font(L"マップの生成中...").drawCenter(Window::Center());
			if (!System::Update()) return;
		}
	}
	mapTexture = Texture(mapImage);

}

double	Planet::getHeight(const Pos& _pos) const
{
	return heightNoise.octaveNoise(_pos.ePos, 10);
}
