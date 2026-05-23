#include "Enemy.hpp"
#include "MaguroAI.hpp"
#include "TakoAI.hpp"
#include "AssetIDs.hpp"

Animation Enemy::loadAnimation(const String& pathDir, bool loop)
{
	const String file = U"image/" + pathDir;
	const CSV csv(file + U".csv");
	if (not csv)
	{
		throw Error(U"Failed to load {}.csv"_fmt(file));
	}
	Array<float> durations;
	for (size_t i = 0; i < csv.columns(0); ++i)
	{
		durations.push_back(Parse<float>(csv[0][i]));
	}
	Array<Texture> frames;
	for (size_t i = 0; i < durations.size(); ++i)
	{
		frames.push_back(Texture(file + U"_" + Format(i) + U".png"));
	}
	return Animation(frames, durations, loop);
}

Enemy::Enemy(const Vec2& pos)
	: pos_(pos)
{
}

Enemy::Enemy(const Vec2& pos, const Vec2& size, int hp, int id, bool loop)
	: pos_(pos), SIZE_(size), hp_(hp), ID_(id), loop_(loop)
{
}

Enemy::~Enemy() = default;

void Enemy::update()
{
}

void Enemy::moveX()
{
	speed_.x = dir_ ? walkSpeed_ : -walkSpeed_;
	pos_.x += speed_.x;
}

void Enemy::moveY()
{
	accel_.y = gravity_;
	speed_.y += accel_.y;
	pos_.y += speed_.y;
}

// ===== Curage =====

Curage::Curage(const Vec2& pos)
	: Enemy(pos)
{
	animation_ = loadAnimation(U"jellyfish/jellyfish");
	animation_.start();
	this->accel_.y += this->gravity_;
}

void Curage::update()
{
	animation_.update();
}

void Curage::draw() const
{
	animation_.currentTexture().draw(pos_);
}

// ===== Kani =====

Kani::Kani(const Vec2& pos)
	: Enemy(pos)
{
	animation_ = loadAnimation(U"crab/crab");
	animation_.start();
	this->accel_.y += this->gravity_;
	this->SIZE_ = Vec2(80, 80);
}

void Kani::update()
{
	animation_.update();
}

void Kani::moveX()
{
	dir_ = playerDir_ > 0;
	Enemy::moveX();
}

void Kani::draw() const
{
	animation_.currentTexture().draw(pos_);
#ifdef DEBUGGING
	RectF(pos_, SIZE_).drawFrame(1.0, Palette::Green);
#endif
}

// ===== Tako =====

Tako::Tako(const Vec2& pos)
	: Enemy(pos)
	, ai_(std::make_unique<TakoAI>())
{
}

Tako::~Tako() = default;

void Tako::update()
{
	// AI 駆動: 移動・ガス弾管理を全部 AI に任せる
	ai_->tick(*this, Scene::DeltaTime());
}

void Tako::moveX()
{
	// 通常の moveX は無効化 (AI 側で速度を管理)
}

void Tako::draw() const
{
	for (const auto& bullet : bullets_)
	{
		TextureAsset(GameAssets::Texture::Bullet).drawAt(bullet->getPos());
	}
	if (!restingPhase_)
		TextureAsset(GameAssets::Texture::Octopus1).draw(pos_);
	else
		TextureAsset(GameAssets::Texture::Octopus2).draw(pos_);
}

// ===== Maguro =====

Maguro::Maguro(const Vec2& pos)
	: Enemy(pos, Vec2(80, 80), BOSS_HP, BOSS_ID, false)
	, ai_(std::make_unique<MaguroAI>())
{
	hp_ = BOSS_HP;

	for (int i = 0; i < IMAGE_NUM; ++i)
	{
		animations_ << loadAnimation(U"boss/Act{}/boss"_fmt(i + 1), false);
		animations_[i].start();
	}
}

Maguro::~Maguro() = default;

void Maguro::update()
{
	// 弾の移動
	for (auto& bullet : bullets_)
	{
		bullet->addPos(bullet->getDir() * 5);
	}
}

void Maguro::draw() const
{
	const bool useAct = animeFlag_;

	if (this->dir_)
	{
		if (!useAct)
			TextureAsset(GameAssets::Texture::Maguro1).draw(pos_);
		else
			animations_[animeDir_].currentTexture().draw(pos_);
	}
	else
	{
		if (!useAct)
			TextureAsset(GameAssets::Texture::Maguro2).draw(pos_);
		else
			animations_[animeDir_].currentTexture().draw(pos_);
	}

	for (const auto& bullet : bullets_)
	{
		TextureAsset(GameAssets::Texture::Bullet2).drawAt(bullet->getPos());
	}
}

void Maguro::moveX()
{
	ai_->tick(*this, Scene::DeltaTime());
}

void Maguro::moveY()
{
	this->accel_.y = this->gravity_;
	this->speed_.y += this->accel_.y;
	this->pos_.y += this->speed_.y;
}
