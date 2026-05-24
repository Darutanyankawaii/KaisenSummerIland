#include "Player.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "IWeapon.hpp"
#include "WeaponFactory.hpp"
#include "AssetIDs.hpp"

namespace {
	// 発射位置: スプライト中心 (pos_ + (14, 23)) を基準に、向きに応じて手側にずらす
	constexpr Vec2 kBulletOffsetCenter{ 14, 23 };
	constexpr double kHandOffsetX = 8.0;
}

Player::Player()
{
	weapon_ = WeaponFactory::create(WeaponName::Water_Gun);
	initAnimations();
	attackDir_ = (Cursor::Pos() - pos_).normalized();
}

Player::~Player()
{
	AudioAsset(GameAssets::Audio::Walk).stop();
}

void Player::setWeapon(int weaponId)
{
	weapon_ = WeaponFactory::create(weaponId);
}

int Player::weaponId() const
{
	return weapon_ ? weapon_->weaponId() : 0;
}

void Player::updateX()
{
	if (shotNow_)
	{
		shotTime_ += Scene::DeltaTime();
		// 狙うアニメ維持期間は武器のクールタイムに合わせる (最低 kShotPostDuration 0.3s)
		const double aimHold = weapon_ ? Max(weapon_->cooltimeSec(), kShotPostDuration) : kShotPostDuration;
		if (shotTime_ > aimHold)
		{
			shotTime_ = 0;
			shotNow_ = false;
			state_ = State::Stand;
		}
		else
		{
			state_ = State::Aiming;
		}
	}

	if (state_ == State::Aiming)
	{
		if ((KeyD | KeyRight | KeyA | KeyLeft).down())
			state_ = State::Walk;
	}
	else
	{
		if (isGround_)
			state_ = State::Stand;

		if ((KeyD | KeyRight).pressed())
		{
			playerDir_ = 1;
			speed_.x = playerDir_ * walkSpeed_;
			state_ = State::Walk;
		}
		else if ((KeyA | KeyLeft).pressed())
		{
			playerDir_ = -1;
			speed_.x = playerDir_ * walkSpeed_;
			state_ = State::Walk;
		}
		else
		{
			speed_.x = 0.0;
		}
	}

	if (isKnockback_)
	{
		pos_.x += knockBackDir_ * kKnockBackSpeed;
		if (collisionalTimer_.reachedZero())
		{
			isKnockback_ = false;
		}
	}
	else
	{
		pos_.x += speed_.x;
	}

	if (invincibleTimer_.reachedZero())
	{
		isInvincible_ = false;
	}
}

void Player::updateY()
{
	accel_.y = gravity_;

	if (KeySpace.down() && isGround_)
	{
		accel_.y -= kJumpImpulse;
		state_ = State::Jump;
	}

	speed_.y += accel_.y;
	pos_.y += speed_.y;

	if (!isGround_ && state_ != State::Aiming)
	{
		state_ = State::Fall;
	}
}

void Player::lastUpdate(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_)
{
	attack(camera, playerBullets_);

	// Animation 更新
	animations_.tick(state_, weaponId(), attackDir_);

	playSound();
	prevState_ = state_;
}

void Player::draw() const
{
	const Texture tex = animations_.currentTexture();
	if (!tex) return;

	// pos_ は hitbox top-left なので、スプライトは SPRITE_DRAW_OFFSET だけ寄せて描画
	const Vec2 drawAt = pos_ + SPRITE_DRAW_OFFSET;

	if (state_ == State::Aiming)
	{
		tex.draw(drawAt);
	}
	else
	{
		if (playerDir_ == 1) tex.draw(drawAt);
		else if (playerDir_ == -1) tex.mirrored().draw(drawAt);
	}
}

void Player::receiveDamage(int damage)
{
	hp_ -= damage;
}

void Player::applyHitFrom(const Vec2& sourcePos)
{
	if (isInvincible_) return;
	hp_ -= 1;
	collisionalTimer_.restart();
	invincibleTimer_.restart();
	isKnockback_ = true;
	isInvincible_ = true;
	knockBackDir_ = (pos_.x < sourcePos.x) ? -1 : 1;
}

void Player::knockBackToEnemy(const Array<std::unique_ptr<Enemy>>& enemies)
{
	for (const auto& enemy : enemies)
	{
		if (this->getRectF().intersects(enemy->getRectF()))
		{
			applyHitFrom(enemy->getPos());
		}
	}
}

void Player::onBulletHit(const Vec2& bulletPos)
{
	applyHitFrom(bulletPos);
}

void Player::recoverDamage(int damage)
{
	hp_ += damage;
}

void Player::initAnimations()
{
	animations_.load();
}

void Player::playSound()
{
	if (prevState_ == state_) return;

	if (prevState_ == State::Walk)
	{
		AudioAsset(GameAssets::Audio::Walk).stop();
	}

	if (state_ == State::Walk && isGround_)
	{
		AudioAsset(GameAssets::Audio::Walk).setLoop(true);
		AudioAsset(GameAssets::Audio::Walk).play();
	}
}

void Player::attack(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_)
{
	if (!weapon_) return;

	// クールタイムは常に進行
	weapon_->tick(Scene::DeltaTime());

	const bool isFirstClick = MouseL.down();
	const bool isHolding = MouseL.pressed();

	const auto t = camera.createTransformer();
	const Vec2 startPos = pos_ + kBulletOffsetCenter
		+ Vec2((playerDir_ == -1 ? -kHandOffsetX : kHandOffsetX), 0);
	const Vec2 cursorWorld = Cursor::Pos();

	bool fired = false;

	if (isFirstClick)
	{
		// 任意の武器: クリックの瞬間に aiming へ (移動は止めない)
		attackDir_ = (Cursor::Pos() - getCenter()).normalize();
		state_ = State::Aiming;
		shotNow_ = true;
		// 単発・連射いずれも同フレームで発射 (連射は次フレーム以降も継続)
		fired = weapon_->tryFire(startPos, cursorWorld, playerBullets_);
	}
	else if (isHolding && weapon_->isContinuous())
	{
		// 連射武器: hold 中は state に関係なく発射継続 (狙いも追従更新)
		attackDir_ = (Cursor::Pos() - getCenter()).normalize();
		state_ = State::Aiming;
		shotNow_ = true;
		shotTime_ = 0.0; // hold 中は aiming タイマーをリセットし続け、状態維持
		fired = weapon_->tryFire(startPos, cursorWorld, playerBullets_);
	}

	if (fired)
	{
		AudioAsset(GameAssets::Audio::Shot).playOneShot();
	}
}
