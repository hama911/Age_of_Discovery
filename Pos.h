#pragma once
#include <Siv3D.hpp>

struct Pos
{
	void set(const Vec3& _ePos);
	void set(const Vec2& _mPos);

	Pos();
	Pos(const Vec3& _ePos);
	Pos(const Vec2& _mPos);

	Vec3 ePos;
	Vec2 mPos;
};