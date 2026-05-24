#include "MaguroAI.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"
#include "SoundSystem.hpp"

namespace {
	constexpr double kIdleInterval = 0.5;        // 攻撃選択までの間隔
	constexpr double kChargeDuration = 1.0;      // 突進前モーション時間
	constexpr double kSpreadDuration = 1.0;      // 拡散弾後の硬直時間 (SPAWN_DIFFUSION)
	constexpr double kRushTimeout = 2.5;         // 突進の最大持続時間 (壁スタックの保険)
	constexpr double kRushSpeed = 720.0; // units/sec (60fps 換算で 12/frame 相当)
	constexpr double kGoalCheckRadius = 10.0;
	constexpr double kSelfCheckRadius = 5.0;
	constexpr BulletParams kSpreadBulletParams{};

	// HomingShot 関連
	constexpr double kHomingTrackDuration = 2.0; // 追尾補正がかかる時間
	constexpr double kHomingPhaseDuration = 2.5; // フェーズ全体の長さ
	constexpr double kHomingTurnRate = 0.04;     // 1 フレームあたりの方向補正係数
	constexpr BulletParams kHomingBulletParams{
		.bulletSpeed = 8.0, .fallSpeed = 1.0, .size = 12, .lifeSpan = 4.0,
	};

	// BurstShot 関連
	constexpr double kBurstInterval = 0.08; // 連射間隔
	constexpr int    kBurstCount = 6;       // 1 セットあたりの弾数
	constexpr double kBurstStiffness = 0.5; // 連射後の硬直
	constexpr double kBurstSpreadDeg = 15.0; // 散らばり角度 (±)
	constexpr BulletParams kBurstBulletParams{
		.bulletSpeed = 12.0, .fallSpeed = 1.0, .size = 8, .lifeSpan = 2.5,
	};
}

void MaguroAI::tick(Maguro& self, double dt)
{
	switch (phase_)
	{
	case MaguroPhase::Idle:
	{
		idleTimer_ += dt;
		if (idleTimer_ > kIdleInterval)
		{
			idleTimer_ = 0.0;
			switch (Random(3))
			{
			case 0: enterChargingRush(self); break;
			case 1: enterSpreading(self); break;
			case 2: enterHomingShot(self); break;
			case 3: enterBurstShot(self); break;
			default: break;
			}
		}
		break;
	}
	case MaguroPhase::ChargingRush:
	{
		// アニメーション再生中
		auto& anims = self.animations();
		if (self.getAnimeDir() < static_cast<int>(anims.size()))
		{
			anims[self.getAnimeDir()].update();
		}
		chargeTimer_ += dt;
		if (chargeTimer_ > kChargeDuration)
		{
			enterRushing(self);
		}
		break;
	}
	case MaguroPhase::Rushing:
	{
		rushTimer_ += dt;

		// 突進中はプレイヤーをゆるく追尾 (速度方向を毎フレーム lerp で補正)
		const Vec2 toPlayer = self.getPlayerPos() - self.getPos();
		const double len = toPlayer.length();
		if (len > 1e-6)
		{
			const Vec2 desired = (toPlayer / len) * kRushSpeed;
			constexpr double kTrackBlend = 0.06; // 0..1 が大きいほど強追尾
			self.setSpeed(self.getSpeed().lerp(desired, kTrackBlend));
		}

		// 突進中: goPos に近づいたら Idle に戻す
		const Vec2 center = self.getPos() + self.getSize() / 2;
		const Vec2 targetCenter = rushTarget_ + self.getSize() / 2;
		const bool reachedTarget = Circle{ targetCenter, kGoalCheckRadius }
			.intersects(Circle{ center, kSelfCheckRadius });
		const bool timedOut = rushTimer_ > kRushTimeout;

		if (reachedTarget || timedOut)
		{
			if (reachedTarget) self.setPosY(self.getPosY() - 5.0); // 着地補正
			self.setTerrainCollision(true);
			self.setGravity(0.1f);
			self.setSpeed({ 0, 0 });
			auto& anims = self.animations();
			if (self.getAnimeDir() < static_cast<int>(anims.size()))
			{
				anims[self.getAnimeDir()].restart();
			}
			enterIdle(self);
		}
		break;
	}
	case MaguroPhase::Spreading:
	{
		spreadTimer_ += dt;
		if (spreadTimer_ > kSpreadDuration)
		{
			self.bullets().clear();
			spreadTimer_ = 0.0;
			enterIdle(self);
		}
		break;
	}
	case MaguroPhase::HomingShot:
	{
		homingTimer_ += dt;

		// 一発だけ撃つ (フェーズ先頭)
		if (!homingFired_)
		{
			const Vec2 startPos = self.getPos() + self.getSize() / 2;
			const Vec2 dir = unitDirection(startPos, self.getPlayerPos());
			self.bullets().push_back(std::make_unique<Bullet>(startPos, dir, kHomingBulletParams));
			Sound::play(Sound::SE::EnemyShot);
			homingFired_ = true;
		}

		// 追尾期間中、bullets_ 全ての方向をプレイヤー方向にゆるく補正
		if (homingTimer_ < kHomingTrackDuration)
		{
			for (auto& b : self.bullets())
			{
				const Vec2 cur = b->getDir();
				const Vec2 desired = unitDirection(b->getPos(), self.getPlayerPos());
				const Vec2 nd = (cur + (desired - cur) * kHomingTurnRate).normalized();
				b->addDir(nd - cur);
			}
		}

		if (homingTimer_ > kHomingPhaseDuration)
		{
			homingTimer_ = 0.0;
			homingFired_ = false;
			enterIdle(self);
		}
		break;
	}
	case MaguroPhase::BurstShot:
	{
		burstTimer_ += dt;

		// kBurstInterval ごとに 1 発、kBurstCount に達するまで
		if (burstShotsFired_ < kBurstCount
			&& burstTimer_ >= kBurstInterval * burstShotsFired_)
		{
			const Vec2 startPos = self.getPos() + self.getSize() / 2;
			const Vec2 baseDir = unitDirection(startPos, self.getPlayerPos());
			const double off = Random(-kBurstSpreadDeg, kBurstSpreadDeg) * 1_deg;
			const double cs = std::cos(off), sn = std::sin(off);
			const Vec2 dir{ baseDir.x * cs - baseDir.y * sn, baseDir.x * sn + baseDir.y * cs };
			self.bullets().push_back(std::make_unique<Bullet>(startPos, dir, kBurstBulletParams));
			Sound::play(Sound::SE::EnemyShot);
			++burstShotsFired_;
		}

		if (burstShotsFired_ >= kBurstCount
			&& burstTimer_ > kBurstInterval * kBurstCount + kBurstStiffness)
		{
			burstTimer_ = 0.0;
			burstShotsFired_ = 0;
			enterIdle(self);
		}
		break;
	}
	}

	// 向きを毎フレーム更新
	self.setFacingRight(self.getPlayerPos().x < self.getPosX());
	// 突進中のみ X が動く。speed_.x は units/sec なので dt を掛ける。
	if (phase_ == MaguroPhase::Rushing)
	{
		self.setPosX(self.getPosX() + self.getSpeedX() * dt);
	}
}

void MaguroAI::enterIdle(Maguro& /*self*/)
{
	phase_ = MaguroPhase::Idle;
	idleTimer_ = 0.0;
}

void MaguroAI::enterChargingRush(Maguro& self)
{
	selectRushTarget(self);
	phase_ = MaguroPhase::ChargingRush;
	chargeTimer_ = 0.0;
	self.setAnimeFlag(true);
	self.setAnimeDir(self.isFacingRight() ? 0 : 1);
	self.setGravity(0.0f);
}

void MaguroAI::enterRushing(Maguro& self)
{
	phase_ = MaguroPhase::Rushing;
	chargeTimer_ = 0.0;
	rushTimer_ = 0.0;
	const Vec2 dir = unitDirection(self.getPos(), rushTarget_);
	self.setSpeed(dir * kRushSpeed);
	self.setTerrainCollision(false);
	self.setAnimeFlag(false);
}

void MaguroAI::enterSpreading(Maguro& self)
{
	phase_ = MaguroPhase::Spreading;
	spreadTimer_ = 0.0;
	emitSpread(self);
}

void MaguroAI::enterHomingShot(Maguro& /*self*/)
{
	phase_ = MaguroPhase::HomingShot;
	homingTimer_ = 0.0;
	homingFired_ = false;
}

void MaguroAI::enterBurstShot(Maguro& /*self*/)
{
	phase_ = MaguroPhase::BurstShot;
	burstTimer_ = 0.0;
	burstShotsFired_ = 0;
}

void MaguroAI::selectRushTarget(Maguro& self)
{
	const auto& area = self.getBossArea();
	if (area.isEmpty()) return;

	if (area.size() == 1)
	{
		lastTargetIndex_ = 0;
		rushTarget_ = area[0];
		return;
	}

	int next;
	do
	{
		next = Random(static_cast<int>(area.size()) - 1);
	} while (next == lastTargetIndex_);
	lastTargetIndex_ = next;
	rushTarget_ = area[next];
}

void MaguroAI::emitSpread(Maguro& self)
{
	const Vec2 startPos = self.getPos() + self.getSize() / 2;

	auto spawn = [&](double angle, bool dir)
		{
			self.bullets().push_back(std::make_unique<Bullet>(
				startPos,
				angledDirection(angle, dir),
				kSpreadBulletParams));
		};

	spawn(15, false);
	spawn(-15, false);
	spawn(0, false);
	spawn(15, true);
	spawn(-15, true);
	spawn(0, true);
	Sound::play(Sound::SE::EnemyShot);
}

Vec2 MaguroAI::unitDirection(const Vec2& from, const Vec2& to)
{
	const Vec2 delta = to - from;
	const double len = delta.length();
	if (len < 1e-9) return Vec2{ 1.0, 0.0 };
	return delta / len;
}

Vec2 MaguroAI::angledDirection(double angleDeg, bool dir)
{
	if (angleDeg == 0)
	{
		return Vec2{ dir ? 1.0 : -1.0, 0.0 };
	}
	if (angleDeg == 90)  return Vec2{ 0.0, 1.0 };
	if (angleDeg == -90) return Vec2{ 0.0, -1.0 };

	const double radians = dir ? ((90 - angleDeg) * 1_deg) : ((angleDeg - 90) * 1_deg);
	const Vec2 offset = OffsetCircular(Vec2{ 0, 0 }, 1, radians);
	return offset;
}
