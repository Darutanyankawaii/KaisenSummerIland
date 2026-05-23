#include "StarfishWeapon.hpp"
#include "Bullet.hpp"

namespace {
	constexpr double kCooltime = 1.0;
	constexpr BulletParams kParams{
		.bulletSpeed = 7.0,
		.fallSpeed = 0.5,
		.size = 8,
		.lifeSpan = 3.0,
	};
}

StarfishWeapon::StarfishWeapon()
	: CooltimeWeapon(kCooltime, static_cast<int>(WeaponName::Starfish))
{
}

bool StarfishWeapon::tryFire(const Vec2& origin, const Vec2& cursorWorld,
	Array<std::unique_ptr<Bullet>>& pool)
{
	if (!canFire()) return false;
	pool.push_back(Bullet::createAimed(origin, cursorWorld, kParams));
	resetCooltime();
	return true;
}
