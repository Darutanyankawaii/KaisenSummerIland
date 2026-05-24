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
	// crab スプライトは 48x48 だが実体は y=31..47, x=2..43 (足元のみ)
	this->SIZE_ = Vec2(42, 17);
	this->spriteDrawOffset_ = Vec2(-2, -31);
	// CSV のスポーン位置 (sprite top-left 意図) を hitbox top-left に再変換
	this->pos_ = pos - this->spriteDrawOffset_;
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
	animation_.currentTexture().draw(getSpriteDrawPos());
#ifdef DEBUGGING
	RectF(pos_, SIZE_).drawFrame(1.0, Palette::Green);
#endif
}

// ===== Tako =====

Tako::Tako(const Vec2& pos)
	: Enemy(pos)
	, ai_(std::make_unique<TakoAI>())
{
	// octopus スプライト 48x48 のうち実体は y=0..46, x=11..36 (縦長で細い)
	SIZE_ = Vec2(26, 47);
	spriteDrawOffset_ = Vec2(-11, 0);
	pos_ = pos - spriteDrawOffset_;
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
	const Vec2 drawAt = getSpriteDrawPos();
	if (!restingPhase_)
		TextureAsset(GameAssets::Texture::Octopus1).draw(drawAt);
	else
		TextureAsset(GameAssets::Texture::Octopus2).draw(drawAt);
}

// ===== Maguro =====

Maguro::Maguro(const Vec2& pos)
	: Enemy(pos, Vec2(80, 80), BOSS_HP, BOSS_ID, false)
	, ai_(std::make_unique<MaguroAI>())
{
	hp_ = BOSS_HP;

	// boss スプライト 80x80 のうち実体は y=1..79, x=6..72 (余白 1~7 px)
	SIZE_ = Vec2(67, 79);
	spriteDrawOffset_ = Vec2(-6, -1);
	pos_ = pos - spriteDrawOffset_;

	for (int i = 0; i < IMAGE_NUM; ++i)
	{
		animations_ << loadAnimation(U"boss/Act{}/boss"_fmt(i + 1), false);
		animations_[i].start();
	}
}

Maguro::~Maguro() = default;

void Maguro::update()
{
	constexpr double kMaguroBulletSpeed = 300.0; // units/sec
	const double dt = Scene::DeltaTime();
	for (auto& bullet : bullets_)
	{
		bullet->addPos(bullet->getDir() * kMaguroBulletSpeed * dt);
		bullet->decreaseLifeSpan(dt);
	}
	bullets_.remove_if([](const std::unique_ptr<Bullet>& b) {
		return b->getLifeSpan() < 0 || b->isHit();
		});
}

void Maguro::draw() const
{
	const bool useAct = animeFlag_;
	const Vec2 drawAt = getSpriteDrawPos();

	if (this->dir_)
	{
		if (!useAct)
			TextureAsset(GameAssets::Texture::Maguro1).draw(drawAt);
		else
			animations_[animeDir_].currentTexture().draw(drawAt);
	}
	else
	{
		if (!useAct)
			TextureAsset(GameAssets::Texture::Maguro2).draw(drawAt);
		else
			animations_[animeDir_].currentTexture().draw(drawAt);
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
