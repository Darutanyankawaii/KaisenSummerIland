#pragma once
#include "IWeapon.hpp"

class WaterGunWeapon : public CooltimeWeapon
{
public:
	WaterGunWeapon();
	bool tryFire(const Vec2& origin, const Vec2& cursorWorld,
		Array<std::unique_ptr<Bullet>>& pool) override;
	bool isContinuous() const override { return false; }
};
