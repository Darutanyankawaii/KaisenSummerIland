#include "Bullet.hpp"

Bullet::Bullet(const Vec2& pos, const Vec2& cur) :
	pos_(pos),
	dir_(getDir(pos, cur)),
	bulletSpeed_(10),
	fallSpeed_(1)
{
}

Bullet::Bullet(const Vec2& pos, const Vec2& cur, const int bulletSpeed, const int size) :
	pos_(pos),
	dir_(getDir(pos, cur)),
	bulletSpeed_(bulletSpeed),
	size_(size)
{
}

Bullet::Bullet(const Vec2& pos, const Vec2& cur, const int bulletSpeed, const double fallSpeed) :
	pos_(pos),
	dir_(getDir(pos, cur)),
	bulletSpeed_(bulletSpeed),
	fallSpeed_(fallSpeed)
{
}

void Bullet::update()
{
	//用調整
	//落下判定
	if (fallSpeed_ > 0.0)
	{
		fallSpeed_ -= Scene::DeltaTime() * 5;
	}
	//移動
	dir_ += Vec2{ 0.0, fallSpeed_ / 50};
	pos_ += dir_ * bulletSpeed_ * Scene::DeltaTime() * blockSize;
}

Vec2 Bullet::getDir(const Vec2& heroPos, const Vec2& cur)
{
	// 三角関数を利用して進行方向を調整
	int x = (cur.x > heroPos.x ? 1 : -1);
	int y = (cur.y > heroPos.y ? 1 : -1);

	double tryZ = sqrt(std::pow(Abs(cur.y - heroPos.y), 2.0) + std::pow(Abs(cur.x - heroPos.x), 2.0));
	double tryX = x * sqrt(std::pow(Abs(cur.x - heroPos.x), 2.0)) / tryZ;
	double tryY = y * sqrt(std::pow(Abs(cur.y - heroPos.y), 2.0)) / tryZ;

	return Vec2{ tryX, tryY };
}

BWater_Gun::BWater_Gun(const Vec2& pos, const Vec2& cur) :
	Bullet(pos, cur, 10, 1)
{

}

BStarfish_Gun::BStarfish_Gun(const Vec2& pos, const Vec2& cur) :
	Bullet(pos, cur, 7, 0.5)
{
}

BShot_Gun::BShot_Gun(const Vec2& pos, const Vec2& cur) :
	Bullet(pos, cur, 10, 1.0)
{
}


BMachine_Gun::BMachine_Gun(const Vec2& pos, const Vec2& cur) :
	Bullet(pos, cur, 15, 1.0)
{
}


BBucket_Gun::BBucket_Gun(const Vec2& pos, const Vec2& cur) :
	Bullet(pos, cur, 7, 0.25)
{
}

BGas::BGas(const Vec2& pos, const Vec2& cur) :
	Bullet(pos, cur, SET_BULLET_SPEED, SET_SIZE)
{
}

