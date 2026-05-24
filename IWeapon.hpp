#pragma once
#include "Basic.hpp"

class Bullet;

// プレイヤーの武器インターフェース。
// 各武器は弾の発射ロジック・クールタイム・連射可否を内部に閉じる。
class IWeapon
{
public:
	virtual ~IWeapon() = default;

	// 弾を撃てる状態なら撃って true を返す。クールタイム中なら何もせず false。
	// origin: 弾の発射開始位置 (ワールド座標)
	// cursorWorld: 狙う先のワールド座標
	// pool: 弾の格納先 (Player の playerBullets_)
	virtual bool tryFire(const Vec2& origin, const Vec2& cursorWorld,
		Array<std::unique_ptr<Bullet>>& pool) = 0;

	// クールタイムを deltaSeconds 進める (毎フレーム呼ぶこと)。
	virtual void tick(double deltaSeconds) = 0;

	// true: 連射武器 (MouseL.pressed 中ずっと撃つ)
	// false: 単発武器 (MouseL.down の瞬間のみ撃つ)
	virtual bool isContinuous() const = 0;

	// 武器識別子 (WeaponName enum 値)。アニメーション切替などに利用。
	virtual int weaponId() const = 0;

	// 武器のクールタイム (秒)。Player 側で aiming アニメ維持期間に利用
	virtual double cooltimeSec() const = 0;

	// クールタイム残量比 (0.0=即発射可、1.0=フルクール残)。HUD 表示用
	virtual double cooltimeRatio() const = 0;
};

// クールタイム管理を担う共通基底。
class CooltimeWeapon : public IWeapon
{
public:
	CooltimeWeapon(double cooltimeSeconds, int weaponId)
		: cooltimeSpec_(cooltimeSeconds), weaponId_(weaponId)
	{
	}

	void tick(double deltaSeconds) override
	{
		current_ -= deltaSeconds;
		if (current_ < 0.0) current_ = 0.0;
	}

	int weaponId() const override { return weaponId_; }
	double cooltimeSec() const override { return cooltimeSpec_; }
	double cooltimeRatio() const override
	{
		if (cooltimeSpec_ <= 0.0) return 0.0;
		return std::clamp(current_ / cooltimeSpec_, 0.0, 1.0);
	}

protected:
	bool canFire() const { return current_ <= 0.0; }
	void resetCooltime() { current_ = cooltimeSpec_; }

private:
	double cooltimeSpec_;
	double current_ = 0.0;
	int weaponId_;
};
