#pragma once
#include "Basic.hpp"

class Maguro;

enum class MaguroPhase
{
	Idle,            // 攻撃選択待ち
	ChargingRush,    // 突進前モーション (1 秒)
	Rushing,         // 突進中 (hitbox 無し)
	Spreading,       // 拡散弾発射→硬直 (SPAWN_DIFFUSION 秒)
};

class MaguroAI
{
public:
	void tick(Maguro& self, double dt);
	MaguroPhase getPhase() const { return phase_; }

private:
	MaguroPhase phase_ = MaguroPhase::Idle;
	double idleTimer_ = 0.0;
	double chargeTimer_ = 0.0;
	double rushTimer_ = 0.0;
	double spreadTimer_ = 0.0;
	int lastTargetIndex_ = 0;
	Vec2 rushTarget_{ 0, 0 };

	void enterChargingRush(Maguro& self);
	void enterSpreading(Maguro& self);
	void enterRushing(Maguro& self);
	void enterIdle(Maguro& self);

	void selectRushTarget(Maguro& self);
	void emitSpread(Maguro& self);

	static Vec2 unitDirection(const Vec2& from, const Vec2& to);
	static Vec2 angledDirection(double angleDeg, bool dir);
};
