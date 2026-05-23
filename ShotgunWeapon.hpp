#pragma once
#include "IWeapon.hpp"

class ShotgunWeapon : public CooltimeWeapon
{
public:
	ShotgunWeapon();
	bool tryFire(const Vec2& origin, const Vec2& cursorWorld,
		Array<std::unique_ptr<Bullet>>& pool) override;
	bool isContinuous() const override { return false; }
};
