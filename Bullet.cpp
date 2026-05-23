#include "Bullet.hpp"

namespace {
	constexpr double kFallDecayRate = 5.0;
	constexpr double kFallSpeedDivisor = 50.0;
}

Bullet::Bullet(const Vec2& pos, const Vec2& dir, const BulletParams& params)
	: pos_(pos)
	, dir_(dir)
	, size_(params.size)
	, bulletSpeed_(params.bulletSpeed)
	, fallSpeed_(params.fallSpeed)
	, lifeSpan_(params.lifeSpan)
{
}

Bullet::Bullet(const Vec2& pos, const Vec2& target)
	: Bullet(pos, directionTo(pos, target), BulletParams{})
{
}

std::unique_ptr<Bullet> Bullet::createAimed(const Vec2& pos, const Vec2& target,
	const BulletParams& params)
{
	return std::make_unique<Bullet>(pos, directionTo(pos, target), params);
}

Vec2 Bullet::directionTo(const Vec2& from, const Vec2& to)
{
	const Vec2 delta = to - from;
	const double len = delta.length();
	if (len < 1e-9)
	{
		return Vec2{ 1.0, 0.0 };
	}
	return delta / len;
}

void Bullet::update()
{
	if (fallSpeed_ > 0.0)
	{
		fallSpeed_ -= Scene::DeltaTime() * kFallDecayRate;
	}
	dir_ += Vec2{ 0.0, fallSpeed_ / kFallSpeedDivisor };
	pos_ += dir_ * bulletSpeed_ * Scene::DeltaTime() * kBlockSize;
}
