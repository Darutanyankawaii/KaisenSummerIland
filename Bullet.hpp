#pragma once
# include "Basic.hpp"

enum class WeaponName //武器番号
{
	Water_Gun,   //0
	Starfish,    //1
	Shotgun,     //2
	Machine_Gun, //3
	Bucket,      //4
};

class Bullet
{
public:
	Bullet() = default;
	Bullet(const Vec2&, const Vec2&);
	Bullet(const Vec2&, const Vec2&, const int, const int);
	Bullet(const Vec2&, const Vec2&, const int, const double);
	virtual ~Bullet() = default;

	virtual void update();

	void setPos(Vec2& pos) { pos_ = pos; };
	void setDir(Vec2& dir) { dir_ = dir; };
	void setFallSpeed(double fallSpeed) { fallSpeed_ = fallSpeed; };

	void additionPos(Vec2 pos) { pos_ += pos; };
	void additionDir(Vec2 dir) { dir_ += dir; };
	void additionFallSpeed(double fallSpeed) { fallSpeed_ += fallSpeed; };

	Vec2 getPos() const { return pos_; };
	Vec2 getDir() const { return dir_; };
	double getFallSpeed() const { return fallSpeed_; };
	Circle getCircle() const { return Circle(pos_, size_); }

	double LifeSpan = 3.0;
	bool hit = false;
protected:
	Vec2 pos_;		// 弾の座標
	Vec2 dir_;		// 弾の進む方向
	int32 size_ = 8;	// 弾のサイズ(半径)
	double bulletSpeed_;
	double fallSpeed_ = 1; // 落下開始までの時間
private:
	Vec2 getDir(const Vec2&, const Vec2&);
};

//各武器の弾は、dirに方向ではなくそのまま速度ベクトルを入れています
//水鉄砲の弾
class BWater_Gun : public Bullet
{
public:
	BWater_Gun(const Vec2&, const Vec2&);
private:
	int SET_BULLET_SPEED = 10;
	const double SET_FALL_SPEED = 1.0;
};

//ヒトデの弾
class BStarfish_Gun : public Bullet
{
public:
	BStarfish_Gun(const Vec2&, const Vec2&);
private:
	const int32 SET_BULLET_SPEED = 7;
	const double SET_FALL_SPEED = 0.5;
};

//散弾銃の弾
class BShot_Gun : public Bullet
{
public:
	BShot_Gun(const Vec2&, const Vec2&);
private:
	const int32 SET_BULLET_SPEED = 10;
	const double SET_FALL_SPEED = 1.0;
};

//機関銃の弾
class BMachine_Gun : public Bullet {
public:
	BMachine_Gun(const Vec2&, const Vec2&);
private:
	const int32 SET_BULLET_SPEED = 15;
	const double SET_FALL_SPEED = 1.0;
};

//バケツの弾
class BBucket_Gun : public Bullet {
public:
	BBucket_Gun(const Vec2&, const Vec2&);
private:
	const int32 SET_BULLET_SPEED = 7;
	const double SET_FALL_SPEED = 0.25;
};

class BGas : public Bullet {
public:
	BGas(const Vec2&, const Vec2&);

private:
	int SET_SIZE = 30;
	int SET_BULLET_SPEED = 1;
};
