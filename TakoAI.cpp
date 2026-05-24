#include "TakoAI.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"

namespace {
	constexpr double kGasInterval = 5.0;
	constexpr double kDashDuration = 0.8;
	constexpr double kRestDuration = 3.0;
	constexpr double kFirstDashDelay = 1.0;
	constexpr Vec2 kDashSpeed{ 160, -4 };
	constexpr double kSpeedDamp = 0.98;
	constexpr double kFallAccel = 3.9;
	constexpr BulletParams kGasParams{
		.bulletSpeed = 1.0,
		.fallSpeed = 1.0,
		.size = 16,
		.lifeSpan = 3.0,
	};
	constexpr double kGasHorizontalSpeed = 100.0;
}

void TakoAI::tick(Tako& self, double dt)
{
	// ガス弾の生成 (一定間隔で必ず生成、プレイヤー方向に発射)
	gasTimer_ += dt;
	if (gasTimer_ > kGasInterval)
	{
		self.bullets().push_back(
			Bullet::createAimed(self.getPos(),
				self.getPlayerPos(), kGasParams));
		gasTimer_ = 0.0;
	}

	// ガス弾の進行 / 寿命 (各弾の dir に従って進行)
	for (auto& bullet : self.bullets())
	{
		bullet->addPos(bullet->getDir() * (dt * kGasHorizontalSpeed));
		bullet->decreaseLifeSpan(dt);
	}
	self.bullets().remove_if([](const std::unique_ptr<Bullet>& b) {
		return b->getLifeSpan() < 0 || b->isHit();
		});

	// 突進パターン (moveX 中で実行)
	phaseTimer_ += dt;
	if (phaseTimer_ > phaseDuration_)
	{
		phaseTimer_ = 0.0;

		if (phase_ == TakoPhase::Resting)
		{
			// 突進開始
			self.setSpeed(kDashSpeed);
			phase_ = TakoPhase::Dashing;
			phaseDuration_ = kDashDuration;
			self.setVisualPhaseRest(false);
		}
		else
		{
			// 待機へ
			phase_ = TakoPhase::Resting;
			phaseDuration_ = kRestDuration;
			self.setVisualPhaseRest(true);
		}
	}

	// 物理: 減衰 + 浮力
	Vec2 sp = self.getSpeed();
	sp.x *= kSpeedDamp;
	sp.y -= dt * kFallAccel;
	self.setSpeed(sp);
	self.setPos(self.getPos() + sp * dt);
}
