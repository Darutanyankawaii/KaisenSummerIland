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
	pos_.x += speed_.x * FpsFactor();
}

void Enemy::moveY()
{
	accel_.y = gravity_;
	speed_.y += accel_.y * FpsFactor();
	pos_.y += speed_.y * FpsFactor();
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
	if (wallEscapeTimer_ > 0.0)
	{
		wallEscapeTimer_ -= Scene::DeltaTime();
	}
}

void Kani::moveX()
{
	// 壁衝突直後 (wallEscapeTimer_ 中) は反転した dir を維持して壁から離れる
	if (wallEscapeTimer_ <= 0.0)
	{
		dir_ = playerDir_ > 0;
	}
	Enemy::moveX();
}

void Kani::onRightWallHit()
{
	setFacingRight(false);
	wallEscapeTimer_ = 0.6;
}

void Kani::onLeftWallHit()
{
	setFacingRight(true);
	wallEscapeTimer_ = 0.6;
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
	spawnPos_ = pos_;
}

Tako::~Tako() = default;

void Tako::update()
{
	// AI 駆動: 移動・ガス弾管理を全部 AI に任せる
	ai_->tick(*this, Scene::DeltaTime());

	// 浮遊して画面外まで上昇しないよう、spawn から一定範囲に制限
	constexpr double kMaxRise = 80.0;
	const double upperBound = spawnPos_.y - kMaxRise;
	if (pos_.y < upperBound)
	{
		pos_.y = upperBound;
		if (speed_.y < 0) speed_.y = 0;
	}

	// 地面に張り付かないよう、spawn 以下に落ちて静止/落下中なら強制的に上昇
	if (pos_.y >= spawnPos_.y && speed_.y >= 0)
	{
		speed_.y = -2.0;
	}
}

void Tako::moveX()
{
	// 通常の moveX は無効化 (AI 側で速度を管理)
}

void Tako::draw() const
{
	for (const auto& bullet : bullets_)
	{
		TextureAsset(GameAssets::Texture::Bullet).drawAt(bullet->getPos(), Palette::Red);
	}
	const Vec2 drawAt = getSpriteDrawPos();
	if (!restingPhase_)
		TextureAsset(GameAssets::Texture::Octopus1).draw(drawAt);
	else
		TextureAsset(GameAssets::Texture::Octopus2).draw(drawAt);
}

// ===== AppleMan =====

AppleMan::AppleMan(const Vec2& pos)
	: Enemy(pos)
{
	// AppleMan スプライト 64x64 のうち実体は y=5..58, x=4..58
	SIZE_ = Vec2(55, 54);
	spriteDrawOffset_ = Vec2(-4, -5);
	pos_ = pos - spriteDrawOffset_;
	walkSpeed_ = 4;
	hp_ = 2;
	accel_.y += gravity_;
}

void AppleMan::moveX()
{
	// プレイヤーが十分離れている時のみ向きを更新 (同 X でのジッタ防止)
	constexpr double kFaceDeadzone = 24.0;
	const double dx = playerPos_.x - pos_.x;
	if (std::abs(dx) > kFaceDeadzone)
	{
		dir_ = (dx > 0);
	}
	// 接地中だけ歩く (空中は慣性維持で長く跳ぶ)
	if (speed_.y == 0.0)
	{
		speed_.x = dir_ ? walkSpeed_ : -walkSpeed_;
	}
	pos_.x += speed_.x * FpsFactor();
}

void AppleMan::update()
{
	// 接地中のみタイマーを進めて、間隔ごとに小ジャンプ
	constexpr double kJumpInterval = 0.9;
	constexpr double kJumpSpeed = -3.6;
	if (speed_.y == 0.0)
	{
		jumpTimer_ += Scene::DeltaTime();
		if (jumpTimer_ >= kJumpInterval)
		{
			speed_.y = kJumpSpeed;
			jumpTimer_ = 0.0;
		}
	}
}

void AppleMan::draw() const
{
	TextureAsset(GameAssets::Texture::AppleMan).draw(getSpriteDrawPos());
}

// ===== Fish =====

Fish::Fish(const Vec2& pos)
	: Enemy(pos)
{
	// fish スプライト 80x80 のうち実体は y=20..59, x=10..68
	SIZE_ = Vec2(59, 40);
	spriteDrawOffset_ = Vec2(-10, -20);
	pos_ = pos - spriteDrawOffset_;
	walkSpeed_ = 2;
	hp_ = 1;
	gravity_ = 0.0f;
	accel_.y = 0.0f;
	baseY_ = pos_.y;
}

void Fish::moveX()
{
	speed_.x = dir_ ? walkSpeed_ : -walkSpeed_;
	pos_.x += speed_.x * FpsFactor();
}

void Fish::moveY()
{
	swimTime_ += Scene::DeltaTime();
	constexpr double kAmplitude = 20.0;
	constexpr double kFreq = 1.2;
	pos_.y = baseY_ + std::sin(swimTime_ * kFreq) * kAmplitude;
}

void Fish::update()
{
	const double dt = Scene::DeltaTime();

	// 一定間隔で真下方向にバブルを射出
	constexpr double kBubbleInterval = 1.5;
	constexpr BulletParams kBubbleParams{
		.bulletSpeed = 1.0,
		.fallSpeed = 1.0,
		.size = 10,
		.lifeSpan = 4.0,
	};
	bubbleTimer_ += dt;
	if (bubbleTimer_ >= kBubbleInterval)
	{
		bullets_.push_back(std::make_unique<Bullet>(
			getRectF().center(), Vec2{ 0.0, 1.0 }, kBubbleParams));
		bubbleTimer_ = 0.0;
	}

	// バブル進行 (重力に近い感覚で下方向)
	constexpr double kBubbleFallSpeed = 90.0; // px/sec
	for (auto& b : bullets_)
	{
		b->addPos(Vec2{ 0.0, dt * kBubbleFallSpeed });
	}
}

void Fish::draw() const
{
	const Vec2 at = getSpriteDrawPos();
	const Texture tex = TextureAsset(GameAssets::Texture::Fish);
	if (dir_)
	{
		tex.mirrored().draw(at);
	}
	else
	{
		tex.draw(at);
	}

	// バブル描画
	for (const auto& b : bullets_)
	{
		TextureAsset(GameAssets::Texture::Bullet).drawAt(b->getPos(), Palette::Skyblue);
	}
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
	}
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
		TextureAsset(GameAssets::Texture::Bullet2).drawAt(bullet->getPos(), Palette::Red);
	}
}

void Maguro::moveX()
{
	ai_->tick(*this, Scene::DeltaTime());
}

void Maguro::moveY()
{
	this->accel_.y = this->gravity_;
	// 突進中は speed_.y が units/sec のため dt を適用 (それ以外は per-frame ベース)
	if (ai_->getPhase() == MaguroPhase::Rushing)
	{
		this->speed_.y += this->accel_.y;
		this->pos_.y += this->speed_.y * Scene::DeltaTime();
	}
	else
	{
		this->speed_.y += this->accel_.y * FpsFactor();
		this->pos_.y += this->speed_.y * FpsFactor();
	}
}
