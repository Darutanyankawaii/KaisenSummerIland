#pragma once
#include "Basic.hpp"
#include "Player.hpp"
#include "Bullet.hpp"

class Enemy
{
public:

	Vec2 pos_;//敵の座標
	Vec2 speed_ = { 0, 0 };//敵の速さ
	Vec2 accel_ = { 0,gravity };//敵の加速度
	Vec2 SIZE{ 48,48 };//敵のサイズ
	float gravity = 0.1;//落下速度
	bool dir = false;//敵の向いている方向

	int WALK_SPEED = 3;//敵の動く速さ
	int hp_ = 1;//敵のHP

	Array<std::unique_ptr<Bullet>> bullets;

	int ID = 0;

	bool hitbox = true;
	bool loop = true;

	RectF getRectF() const { return RectF(pos_, SIZE); }

	Enemy(const Vec2& pos);//主に初期化する内容に使う
	Enemy(const Vec2& pos, const Vec2 size, int hp, const int id, const bool loop);//主に初期化する内容に使う

	virtual void update();//アニメーションのアップデート等
	virtual void draw() const = 0;//主に描画したい内容に使う
	virtual void moveX();
	virtual void moveY();

	void getArea(Array<Vec2>& bossArea);

	int returnBullet_size()
	{
		return bullets.size();
	}
	Array<std::unique_ptr<Bullet>>& returnBullet()
	{
		return bullets;
	}

	void setPDir(int playerDir) { playerDir_ = playerDir; };
	void setPPos(Vec2 playerPos) { playerPos_ = playerPos;  };
	void setPRect(RectF playerRect) { playerRect_ = playerRect; };

protected:
	int playerDir_;
	Vec2 playerPos_;
	RectF playerRect_;
	Array<Vec2> bossArea;
	Animation loadAnimation(String);
private:
};

class Curage :public Enemy {
public:
	Curage(const Vec2& _pos);
	void update() override;
	void draw() const override;
private:
	Animation animation;
};

class Kani :public Enemy {
public:
	Kani(const Vec2& _pos);
	void update() override;
	void moveX() override;
	void draw() const override;
private:
	Animation animation;
};

class Tako:public Enemy {
public:
	Tako(const Vec2& _pos);

	void moveX() override;

	// 出現間隔（秒）
	double spawnTime = 1.0;
	double spawnTime_iPi = 5.0;

	// 蓄積された時間（秒）
	double accumulator = 0;
	double accumulator_iPi = 0;

	//　機能に関するフラグ
	bool flag_;

	//Array<BGas> bullets;

	void update() override;
	void draw() const override;
private:
};

class Maguro :public Enemy {
public:
	Maguro(const Vec2& _pos);
	void update() override;
	void moveX() override;
	void moveY() override;
	void draw() const override;

private:
	//bossステータス
	const int IMAGE_NUM = 2;
	const int BOSS_HP = 30;
	const int BOSS_ID = -1;

	//行動機能
	int num = 0;//次移動する場所の記録
	Vec2 goPos = { 0, 0 };//敵の行き先

	//攻撃機能管理
	const double spawnTime = 0.5;// 攻撃機能発動の間隔（秒）
	double accumulator = 0;// 蓄積された時間（秒）
	bool endedAct = true;//攻撃行動を終了したか確認

	Vec2 setDir(const Vec2&, const Vec2&);
	Vec2 setAngle(const Vec2&, const double, const bool);

	//拡散弾攻撃機能
	bool bulletFlag = false;//拡散弾攻撃機能のフラグ
	double diffusionAccumulator = 0;// 拡散弾攻撃の蓄積された時間（秒）
	const double SPAWN_DIFFUSION = 1.0;// 拡散弾攻撃の出現間隔（秒）
	void spreadAttack();//拡散弾攻撃機能の初期化関数

	//突進機能
	bool rushFlag = false;//突進機能のフラグ
	double rushAccumulator = 0;// 突進の蓄積された時間（秒）
	const double SPAWN_RUSH = 1.0;// 突進の出現間隔（秒）
	void moveAttack();//突進機能の初期化関数

	//アニメ機能
	Array<Animation> animation;//アニメ－ションの変数
	bool animeFlag = false;//アニメーション開始のフラグ
	bool animeEnd = false;//アニメーション終了のフラグ
	int animeDir = 0;//アニメーション描画の方向
};
