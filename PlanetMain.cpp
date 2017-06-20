#include "Planet.h"

void	Planet::draw() const
{
	Graphics2D::SetTransform(transform);
	mapTexture.resize(Vec2(Pi * 2, Pi)).draw(Vec2(-Pi, -HalfPi));

	Graphics2D::ClearTransform();
}

void	Planet::update()
{
	Mouse::SetTransform(transform);


	//ƒJƒƒ‰‚ÌXV
	Mouse::ClearTransform();
	transform = transform.scale(1.0 - double(Mouse::Wheel()) / 10.0, Mouse::PosF());
}