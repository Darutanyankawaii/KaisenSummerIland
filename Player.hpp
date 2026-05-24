#pragma once
#include "Basic.hpp"
#include "Camera.hpp"
#include "PlayerAnimationSet.hpp"

class Bullet;
class Enemy;
class IWeapon;

class Player
{
public:
	Player();
	~Player();

	void updateX();
	void updateY();
	void lastUpdate(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_);
	void draw() const;

	void recoverDamage(int damage);
	void receiveDamage(int damage);
	void knockBackToEnemy(const Array<std::unique_ptr<Enemy>>& enemies);
	// 弾被弾時の処理 (無敵時間中は無効)。bulletPos からノックバック方向を決定
	void onBulletHit(const Vec2& bulletPos);

	int getHp() const { return hp_; }
	int getDir() const { return playerDir_; }

	double getPosX() const { return pos_.x; }
	double getPosY() const { return pos_.y; }
	double getSpeedX() const { return speed_.x; }
	double getSpeedY() const { return speed_.y; }
	double getSizeX() const { return SIZE.x; }
	double getSizeY() const { return SIZE.y; }

	Vec2 getPos() const { return pos_; }
	Vec2 getSize() const { return SIZE; }
	Vec2 getCenter() const { return pos_ + SIZE / 2; }

	RectF getRectF() const { return RectF{ pos_, SIZE }; }

	void setGround(bool isGround) { isGround_ = isGround; }
	void setPosX(double x) { pos_ = Vec2{ x, pos_.y }; }
	void setPosY(double y) { pos_ = Vec2{ pos_.x, y }; }
	void setSpeedX(double x) { speed_ = Vec2{ x, speed_.y }; }
	void setSpeedY(double y) { speed_ = Vec2{ speed_.x, y }; }
	void setPos(const Vec2& pos) { pos_ = pos; }

	// CSV のスポーンセル (スプライト top-left の意図) を受け取り、
	// 内部の pos_ (=hitbox top-left) へ変換して設定する
	void setSpawnAt(const Vec2& spriteTopLeft) {
		pos_ = spriteTopLeft - SPRITE_DRAW_OFFSET;
	}

	// スプライト top-left を返す (カメラ初期位置などで利用)
	Vec2 getSpritePos() const { return pos_ + SPRITE_DRAW_OFFSET; }

	// 武器設定 (Item から呼ばれる)
	void setWeapon(int weaponId);
	int weaponId() const;

private:
	using State = PlayerAnimationSet::State;

	PlayerAnimationSet animations_;
	State state_ = State::Stand;
	State prevState_ = State::Stand;

	std::unique_ptr<IWeapon> weapon_;

	void initAnimations();
	void playSound();
	void attack(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_);

public:
	// テクスチャサイズ (描画用)
	static constexpr Vec2 SPRITE_SIZE{ 64, 64 };
	// pos_ (=hitbox top-left) から sprite top-left へのオフセット。
	// スプライト 64x64 のうち、stand フレームの opaque 範囲は y=9..63 / x=10..50。
	// 体本体 (ハット除外) の典型範囲は x=18..46 (w=28)、y=9..63 (h=55)。
	// hitbox を体に合わせ、描画はスプライト全体を残す。
	static constexpr Vec2 SPRITE_DRAW_OFFSET{ -18, -9 };

private:
	static constexpr Vec2 SET_SPEED{ 0, 0 };
	static constexpr Vec2 SET_ACCEL{ 0, 0 };
	// 当たり判定サイズ。スプライトの体部分にフィット (ハット・余白を除外)
	static constexpr Vec2 SIZE{ 28, 55 };

	static constexpr float kJumpImpulse = 18.0f;
	static constexpr double kKnockBackSpeed = 5.0;
	static constexpr double kShotPostDuration = 0.5;

	Vec2 pos_{ 0, 0 };
	Vec2 speed_{ SET_SPEED };
	Vec2 accel_{ SET_ACCEL };

	int playerDir_ = 1;
	Vec2 attackDir_{ 1, 0 };
	bool isGround_ = false;

	int hp_ = 5;
	int walkSpeed_ = 5;
	float gravity_ = 1.0f;

	bool shotNow_ = false;
	double shotTime_ = 0.0;

	bool isKnockback_ = false;
	bool isInvincible_ = false;
	int knockBackDir_ = 1;

	Timer collisionalTimer_{ 0.2s, StartImmediately::No };
	Timer invincibleTimer_{ 0.7s, StartImmediately::No };
};
