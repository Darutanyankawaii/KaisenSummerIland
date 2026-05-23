#include "BucketWeapon.hpp"
#include "Bullet.hpp"

namespace {
	constexpr double kCooltime = 1.0;
	constexpr double kSpreadTheta = 50.0 * Math::Pi / 180.0;
	constexpr double kSpreadSpeed = 0.1;
	constexpr int kPellets = 5;
	constexpr BulletParams kParams{
		.bulletSpeed = 7.0,
		.fallSpeed = 0.25,
		.size = 8,
		.lifeSpan = 3.0,
	};
}

BucketWeapon::BucketWeapon()
	: CooltimeWeapon(kCooltime, static_cast<int>(WeaponName::Bucket))
{
}

bool BucketWeapon::tryFire(const Vec2& origin, const Vec2& cursorWorld,
	Array<std::unique_ptr<Bullet>>& pool)
{
	if (!canFire()) return false;

	const Vec2 baseDirection = cursorWorld - origin;
	auto applySpread = [&](const Vec2& v) -> Vec2 {
		const double theta = Random(-kSpreadTheta, kSpreadTheta);
		const double speedJitter = Random(1.0 - kSpreadSpeed, 1.0 + kSpreadSpeed);
		return Vec2(v.x * Cos(theta) - v.y * Sin(theta),
			v.x * Sin(theta) + v.y * Cos(theta)) * speedJitter;
		};

	for (int i = 0; i < kPellets; ++i)
	{
		const Vec2 spreadTarget = applySpread(baseDirection) + origin;
		pool.push_back(Bullet::createAimed(origin, spreadTarget, kParams));
	}
	resetCooltime();
	return true;
}
