#include "Pos.h"

void Pos::set(const Vec3& _ePos) {
	ePos = _ePos.normalized();
	mPos = Vec2(Atan2(ePos.z, ePos.x), -Asin(ePos.y));
}
void Pos::set(const Vec2& _mPos) {
	set(Vec3(Cos(_mPos.x)*Cos(_mPos.y), -Sin(_mPos.y), Sin(_mPos.x)*Cos(_mPos.y)));
}
Pos::Pos() { set(Vec2(0.0, 0.0)); }
Pos::Pos(const Vec3& _ePos) { set(_ePos); }
Pos::Pos(const Vec2& _mPos) { set(_mPos); }