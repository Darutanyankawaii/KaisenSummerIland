#include "WaterGunWeapon.hpp"
#include "Bullet.hpp"

namespace {
	constexpr double kCooltime = 0.5;
	constexpr BulletParams kParams{
		.bulletSpeed = 10.0,
		.fallSpeed = 1.0,
		.size = 8,
		.lifeSpan = 3.0,
	};
}

WaterGunWeapon::WaterGunWeapon()
	: CooltimeWeapon(kCooltime, static_cast<int>(WeaponName::Water_Gun))
{
}

bool WaterGunWeapon::tryFire(const Vec2& origin, const Vec2& cursorWorld,
	Array<std::unique_ptr<Bullet>>& pool)
{
	if (!canFire()) return false;
	pool.push_back(Bullet::createAimed(origin, cursorWorld, kParams));
	resetCooltime();
	return true;
}
