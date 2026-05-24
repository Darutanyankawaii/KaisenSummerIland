#include "Player.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "IWeapon.hpp"
#include "WeaponFactory.hpp"
#include "AssetIDs.hpp"

namespace {
	constexpr Vec2 kBulletOffset{ 32, 32 };
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
		if (shotTime_ > kShotPostDuration)
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

		if ((KeyD | KeyRight).pressed() && !aimFlag_)
		{
			playerDir_ = 1;
			speed_.x = playerDir_ * walkSpeed_;
			state_ = State::Walk;
		}
		else if ((KeyA | KeyLeft).pressed() && !aimFlag_)
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

	if (state_ == State::Aiming)
	{
		tex.draw(pos_);
	}
	else
	{
		if (playerDir_ == 1) tex.draw(pos_);
		else if (playerDir_ == -1) tex.mirrored().draw(pos_);
	}
}

void Player::receiveDamage(int damage)
{
	hp_ -= damage;
}

void Player::knockBackToEnemy(const Array<std::unique_ptr<Enemy>>& enemies)
{
	for (const auto& enemy : enemies)
	{
		if (!isInvincible_ && this->getRectF().intersects(enemy->getRectF()))
		{
			collisionalTimer_.restart();
			invincibleTimer_.restart();
			receiveDamage(1);
			isKnockback_ = true;
			isInvincible_ = true;
			knockBackDir_ = (pos_.x < enemy->getPosX()) ? -1 : 1;
		}
	}
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

	aimFlag_ = false;

	const bool isFirstClick = MouseL.down();
	const bool isHolding = MouseL.pressed();
	const bool inAimingState = (state_ == State::Aiming);

	const auto t = camera.createTransformer();
	const Vec2 startPos = pos_ + kBulletOffset;
	const Vec2 cursorWorld = Cursor::Pos();

	bool fired = false;

	if (isFirstClick)
	{
		// 任意の武器: クリックの瞬間に aiming へ
		speed_.x = 0;
		aimFlag_ = true;
		attackDir_ = (Cursor::Pos() - getCenter()).normalize();
		state_ = State::Aiming;
		shotNow_ = true;

		// 単発武器のみ: 同フレームで発射
		if (!weapon_->isContinuous())
		{
			fired = weapon_->tryFire(startPos, cursorWorld, playerBullets_);
		}
	}
	else if (isHolding && inAimingState && weapon_->isContinuous())
	{
		// 連射武器: aiming 中の継続クリック
		shotNow_ = true;
		fired = weapon_->tryFire(startPos, cursorWorld, playerBullets_);
	}

	if (fired)
	{
		AudioAsset(GameAssets::Audio::Shot).playOneShot();
	}
}
