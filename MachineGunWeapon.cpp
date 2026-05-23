#include "MachineGunWeapon.hpp"
#include "Bullet.hpp"

namespace {
	constexpr double kCooltime = 0.2;
	constexpr BulletParams kParams{
		.bulletSpeed = 15.0,
		.fallSpeed = 1.0,
		.size = 8,
		.lifeSpan = 3.0,
	};
}

MachineGunWeapon::MachineGunWeapon()
	: CooltimeWeapon(kCooltime, static_cast<int>(WeaponName::Machine_Gun))
{
}

bool MachineGunWeapon::tryFire(const Vec2& origin, const Vec2& cursorWorld,
	Array<std::unique_ptr<Bullet>>& pool)
{
	if (!canFire()) return false;
	pool.push_back(Bullet::createAimed(origin, cursorWorld, kParams));
	resetCooltime();
	return true;
}
