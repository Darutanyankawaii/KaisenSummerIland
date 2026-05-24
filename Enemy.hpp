#pragma once
#include "Basic.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"

class MaguroAI;
class TakoAI;

class Enemy
{
public:
	explicit Enemy(const Vec2& pos);
	Enemy(const Vec2& pos, const Vec2& size, int hp, int id, bool loop);
	virtual ~Enemy();

	virtual void update();
	virtual void draw() const = 0;
	virtual void moveX();
	virtual void moveY();

	// ---- 位置 / 速度 / 加速度 (カプセル化) ----
	const Vec2& getPos() const { return pos_; }
	double getPosX() const { return pos_.x; }
	double getPosY() const { return pos_.y; }
	void setPos(const Vec2& p) { pos_ = p; }
	void setPosX(double x) { pos_.x = x; }
	void setPosY(double y) { pos_.y = y; }

	const Vec2& getSpeed() const { return speed_; }
	double getSpeedX() const { return speed_.x; }
	double getSpeedY() const { return speed_.y; }
	void setSpeed(const Vec2& s) { speed_ = s; }
	void setSpeedX(double x) { speed_.x = x; }
	void setSpeedY(double y) { speed_.y = y; }

	const Vec2& getAccel() const { return accel_; }
	void setAccel(const Vec2& a) { accel_ = a; }
	void setAccelY(double y) { accel_.y = y; }

	const Vec2& getSize() const { return SIZE_; }
	double getSizeX() const { return SIZE_.x; }
	double getSizeY() const { return SIZE_.y; }
	void setSize(const Vec2& s) { SIZE_ = s; }

	RectF getRectF() const { return RectF(pos_, SIZE_); }

	// ---- ステータス ----
	int getHp() const { return hp_; }
	void setHp(int hp) { hp_ = hp; }
	void takeDamage(int amount) { hp_ -= amount; }

	int getID() const { return ID_; }
	bool isBoss() const { return ID_ == -1; }

	bool hasHitbox() const { return hitbox_; }
	void setHitbox(bool h) { hitbox_ = h; }

	bool isFacingRight() const { return dir_; }
	void setFacingRight(bool right) { dir_ = right; }

	float getGravity() const { return gravity_; }
	void setGravity(float g) { gravity_ = g; }

	int getWalkSpeed() const { return walkSpeed_; }
	void setWalkSpeed(int s) { walkSpeed_ = s; }

	// ---- 弾配列 (敵が所有) ----
	Array<std::unique_ptr<Bullet>>& bullets() { return bullets_; }
	const Array<std::unique_ptr<Bullet>>& bullets() const { return bullets_; }

	// ---- プレイヤー情報 (collision からの一方向通知) ----
	void setPlayerDir(int playerDir) { playerDir_ = playerDir; }
	void setPlayerPos(const Vec2& p) { playerPos_ = p; }
	void setPlayerRect(const RectF& r) { playerRect_ = r; }
	int getPlayerDir() const { return playerDir_; }
	const Vec2& getPlayerPos() const { return playerPos_; }

	// ---- ボスエリア (Maguro が利用) ----
	void getArea(Array<Vec2>& outBossArea) const { outBossArea = bossArea_; }
	void setBossArea(const Array<Vec2>& area) { bossArea_ = area; }
	const Array<Vec2>& getBossArea() const { return bossArea_; }

	// ---- 旧 API 互換 (削除予定。Collision.cpp が直接書換していたメンバへのアクセス用) ----
	int returnBulletSize() const { return static_cast<int>(bullets_.size()); }

protected:
	Animation loadAnimation(const String& pathDir, bool loop = true);

	float gravity_ = 0.1f;
	Vec2 pos_{ 0, 0 };
	Vec2 speed_{ 0, 0 };
	Vec2 accel_{ 0, gravity_ };
	Vec2 SIZE_{ 48, 48 };
	// スプライト top-left の pos_ からのオフセット。
	// pos_ = hitbox top-left, スプライト描画位置 = pos_ + spriteDrawOffset_
	Vec2 spriteDrawOffset_{ 0, 0 };
	// スプライトを描画する位置 (各派生クラスの draw で利用)
	Vec2 getSpriteDrawPos() const { return pos_ + spriteDrawOffset_; }
	bool dir_ = false;
	int walkSpeed_ = 3;
	int hp_ = 1;
	Array<std::unique_ptr<Bullet>> bullets_;
	int ID_ = 0;
	bool hitbox_ = true;
	bool loop_ = true;

	int playerDir_ = 0;
	Vec2 playerPos_{ 0, 0 };
	RectF playerRect_;
	Array<Vec2> bossArea_;
};

class Curage : public Enemy
{
public:
	explicit Curage(const Vec2& pos);
	void update() override;
	void draw() const override;

private:
	Animation animation_;
};

class Kani : public Enemy
{
public:
	explicit Kani(const Vec2& pos);
	void update() override;
	void moveX() override;
	void draw() const override;

private:
	Animation animation_;
};

class Tako : public Enemy
{
public:
	explicit Tako(const Vec2& pos);
	~Tako() override;

	void moveX() override;
	void update() override;
	void draw() const override;

	// AI から状態を制御するための内部アクセス
	void setVisualPhaseRest(bool rest) { restingPhase_ = rest; }
	bool isRestingPhase() const { return restingPhase_; }

private:
	std::unique_ptr<TakoAI> ai_;
	bool restingPhase_ = true; // 旧 flag_ (true=待機, false=突進)
};

class Maguro : public Enemy
{
public:
	explicit Maguro(const Vec2& pos);
	~Maguro() override;

	void update() override;
	void moveX() override;
	void moveY() override;
	void draw() const override;

	// AI から呼ぶ内部アクセス
	Array<Animation>& animations() { return animations_; }
	void setAnimeFlag(bool f) { animeFlag_ = f; }
	bool getAnimeFlag() const { return animeFlag_; }
	void setAnimeDir(int d) { animeDir_ = d; }
	int getAnimeDir() const { return animeDir_; }

	static constexpr int IMAGE_NUM = 2;
	static constexpr int BOSS_HP = 30;
	static constexpr int BOSS_ID = -1;

private:
	std::unique_ptr<MaguroAI> ai_;
	Array<Animation> animations_;
	bool animeFlag_ = false;
	int animeDir_ = 0;
};
