#pragma once
#include "IWeapon.hpp"

class BucketWeapon : public CooltimeWeapon
{
public:
	BucketWeapon();
	bool tryFire(const Vec2& origin, const Vec2& cursorWorld,
		Array<std::unique_ptr<Bullet>>& pool) override;
	bool isContinuous() const override { return false; }
};
