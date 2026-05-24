#include "Game.hpp"
#include "Collision.hpp"
#include "StageRepository.hpp"
#include "MapParser.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"
#include "SaveManager.hpp"

namespace {
	// 弾ヒット時の小さな閃光 (撃破ではない被弾)。短時間で消える。
	struct HitSparkEffect : IEffect
	{
		Vec2 pos_;
		double duration_ = 0.15;

		explicit HitSparkEffect(const Vec2& pos) : pos_(pos) {}

		bool update(double t) override
		{
			if (t > duration_) return false;
			const double progress = t / duration_;
			const double alpha = 1.0 - progress;
			const double r = 4.0 + progress * 14.0;
			// 白い閃光
			Circle(pos_, r).draw(ColorF{ 1.0, 1.0, 1.0, alpha });
			// 黄色の外輪
			Circle(pos_, r * 1.3).drawFrame(2.0, ColorF{ 1.0, 0.9, 0.3, alpha });
			return true;
		}
	};

	// 敵撃破時の拡散リング+内側フラッシュ。kDuration 秒で消える。
	struct EnemyDefeatEffect : IEffect
	{
		Vec2 pos_;
		ColorF color_;
		double maxRadius_;
		double duration_;

		EnemyDefeatEffect(const Vec2& pos, const ColorF& color, double maxRadius, double duration)
			: pos_(pos), color_(color), maxRadius_(maxRadius), duration_(duration) {}

		bool update(double t) override
		{
			if (t > duration_) return false;
			const double progress = t / duration_;
			const double r = maxRadius_ * progress;
			const double alpha = 1.0 - progress;
			Circle(pos_, r).drawFrame(3.0, ColorF{ color_, alpha });
			Circle(pos_, r * 0.5).draw(ColorF{ color_, alpha * 0.4 });
			return true;
		}
	};
}

Game::Game(const InitData& init) : IScene{ init }
{
	player_ = std::make_unique<Player>();
	player_->setWeapon(static_cast<int>(WeaponName::Water_Gun));

	const StageData stageData = StageRepository::instance().get(getData().currentStageID);
	loadedStage_ = MapParser::parse(stageData.path);

	mapSize_ = loadedStage_.mapSize;
	if (loadedStage_.playerStartSet)
	{
		// CSV のスポーンセル位置はスプライト top-left を意図しているため、
		// setSpawnAt で内部の hitbox top-left に変換する。
		player_->setSpawnAt(loadedStage_.playerStart);
		camera_.startPos = player_->getSpritePos().movedBy(0, -1 * kBlockSize);
		camera_ = CustomCamera2D(camera_.startPos);
	}

	PutBlocks(loadedStage_);
	PutEnemy(loadedStage_);
	PutItem(loadedStage_);

	maptip_ = TextureAsset(GameAssets::Texture::Map1);
	registMaptip();

	Sound::play(Sound::SE::StageStart);
}

Game::~Game()
{
	Sound::stopAllShots(Sound::SE::PlayerShot);
}

void Game::update()
{
	Cursor::RequestStyle(CursorStyle::Hidden);
	ClearPrint();

	if (KeyEscape.down())
	{
		changeScene(SceneName::Title);
		return;
	}

	updatePlayer();
	updateEnemies();
	updateBullets();

	knockBack();
	checkItem();
	player_->lastUpdate(camera_, playerBullets_);
	updateCamera();
	checkGoal();

	if (player_->getHp() <= 0)
	{
		Sound::play(Sound::SE::GameOverJingle);
		changeScene(SceneName::GameOver);
	}
}

void Game::updatePlayer()
{
	player_->updateX();
	player_->setPosX(Clamp(player_->getPosX(), 0.0,
		static_cast<double>(mapSize_.x) - player_->getSizeX()));

	Collision::CollisionWithWall(blocks_, player_);

	player_->updateY();

	Collision::CollisionWithGround(blocks_, player_);

	if (!cooltimeFlag_)
	{
		bulletAccumulator_ += Scene::DeltaTime();
		if (bulletAccumulator_ > 1)
		{
			cooltimeFlag_ = true;
			bulletAccumulator_ = 0;
		}
	}
}

void Game::updateEnemies()
{
	for (auto& enemy : enemies_)
	{
		enemy->moveX();
		enemy->setPlayerDir(player_->getDir());
		enemy->setPlayerPos(player_->getPos());
		enemy->setPlayerRect(player_->getRectF());
		enemy->setBossArea(loadedStage_.bossArea);
	}

	Collision::CollisionWithWall(blocks_, enemies_);

	for (auto& enemy : enemies_)
	{
		enemy->moveY();
	}

	Collision::CollisionWithGround(blocks_, enemies_);

	for (auto& enemy : enemies_)
	{
		enemy->update();
	}
}

void Game::updateBullets()
{
	for (auto& bullet : playerBullets_)
	{
		if (!bullet) continue;
		bullet->update();
	}

	Collision::CheckBulletsAlive(playerBullets_, blocks_, camera_);

	if (Collision::CollisionWithBullet(playerBullets_, enemies_,
		[this](const Vec2& pos, bool isBoss)
		{
			const ColorF c = isBoss ? ColorF{ 1.0, 0.4, 0.4 } : ColorF{ 1.0, 0.9, 0.4 };
			const double r = isBoss ? 130.0 : 60.0;
			const double d = isBoss ? 0.7 : 0.35;
			effects_.add<EnemyDefeatEffect>(pos, c, r, d);
			if (isBoss) camera_.triggerShake(14.0, 0.55);
		},
		[this](const Vec2& hitPos)
		{
			effects_.add<HitSparkEffect>(hitPos);
		}))
	{
		changeScene(SceneName::GameClear);
	}

	for (auto& enemy : enemies_)
	{
		if (Collision::CollisionWithBullet(enemy->bullets(), player_))
		{
			camera_.triggerShake(5.0, 0.18);
		}
		Collision::CheckBulletsAlive(enemy->bullets(), blocks_, camera_);
	}
}

void Game::draw() const
{
	{
		const auto t = camera_.createTransformer();
		const Transformer2D shakeTr{ Mat3x2::Translate(camera_.getShakeOffset()), TransformCursor::Yes };

		const BgKind bg = StageRepository::instance().get(getData().currentStageID).bg;
		const StringView bgTex = (bg == BgKind::Sky)
			? GameAssets::Texture::Sky
			: GameAssets::Texture::Background;
		const auto bgSize = TextureAsset(bgTex).size();
		// マップ全域を覆うようにタイル枚数を動的に決定 (左に 1 枚分、右に 1 枚分の余裕)
		const int tilesX = static_cast<int>((mapSize_.x + bgSize.x - 1) / bgSize.x) + 2;
		const int tilesY = static_cast<int>((mapSize_.y + bgSize.y - 1) / bgSize.y) + 1;
		for (int j = 0; j < tilesY; ++j)
		{
			for (int i = -1; i < tilesX - 1; ++i)
			{
				TextureAsset(bgTex).draw(bgSize.x * i, bgSize.y * j);
			}
		}

		{
			// タイル境界で隣接ピクセルが滲んで継ぎ目に見えないよう Nearest サンプリング固定
			const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
			for (const auto& bd : loadedStage_.blocks)
			{
				maps_[bd.num].draw(bd.pos);
			}
		}

#ifdef DEBUGGING
		for (const auto& block : blocks_)
		{
			block.getRegion().drawFrame(1, Palette::Seagreen);
		}
#endif

		for (const auto& enemy : enemies_)
		{
			enemy->draw();
		}

		for (const auto& bullet : playerBullets_)
		{
			TextureAsset(GameAssets::Texture::Bullet).drawAt(bullet->getPos());
		}

		drawItem();
		player_->draw();

		effects_.update();

		TextureAsset(GameAssets::Texture::LockOn).resized(30).drawAt(Cursor::Pos());
	}

	for (int i = 0; i < player_->getHp(); ++i)
	{
		TextureAsset(GameAssets::Texture::Heart).drawAt(680 - i * 45, 30);
	}

	// 武器クールタイム表示 (左下に円グラフ風: 残量に応じて扇形が縮む)
	{
		constexpr Vec2 center{ 60, 700 };
		constexpr double radius = 28.0;
		const double ratio = player_->getWeaponCooltimeRatio();
		// 背景の薄い円
		Circle(center, radius).draw(ColorF{ 0.1, 0.1, 0.1, 0.5 }).drawFrame(2.0, ColorF{ 1.0, 0.8 });
		if (ratio > 0.0)
		{
			// 残量を扇形で表示 (12 時方向起点、時計回り)
			const double angle = ratio * Math::TwoPi;
			Circle(center, radius - 4).drawPie(0.0, angle, ColorF{ 1.0, 0.6, 0.2, 0.85 });
		}
		else
		{
			// 発射可: 緑の中央円
			Circle(center, radius - 8).draw(ColorF{ 0.3, 1.0, 0.4, 0.7 });
		}
	}
}

void Game::updateCamera()
{
	camera_.updateShake(Scene::DeltaTime());
	camera_.update();

	const double halfSceneWidth = Scene::Width() / 2.0;
	const double x = Clamp(player_->getCenter().x, halfSceneWidth,
		static_cast<double>(mapSize_.x) - halfSceneWidth);
	camera_.setTargetCenter(Vec2(x, camera_.getTargetCenter().y));
}

void Game::PutBlocks(const LoadedStage& stage)
{
	for (const auto& bd : stage.blocks)
	{
		switch (bd.flag)
		{
		case 1:
			blocks_.push_back(Block(RectF{ bd.pos, kBlockSize }, 1));
			break;
		case 2:
			blocks_.push_back(Block(RectF{ bd.pos, kBlockSize, kBlockSize / 2 }, 2));
			break;
		case 3:
			blocks_.push_back(Block(RectF{ bd.pos, kBlockSize, kBlockSize }, 2));
			break;
		case 4:
			blocks_.push_back(Block(RectF{ bd.pos, kBlockSize, kBlockSize }, 4));
			break;
		default:
			break;
		}
	}
}

void Game::PutEnemy(const LoadedStage& stage)
{
	for (const auto& ed : stage.enemies)
	{
		switch (ed.num)
		{
		case 1: enemies_.push_back(std::make_unique<Curage>(ed.region));   break;
		case 2: enemies_.push_back(std::make_unique<Kani>(ed.region));     break;
		case 3: enemies_.push_back(std::make_unique<Tako>(ed.region));     break;
		case 4: enemies_.push_back(std::make_unique<AppleMan>(ed.region)); break;
		case 5: enemies_.push_back(std::make_unique<Fish>(ed.region));     break;
		case 9:
			enemies_.push_back(std::make_unique<Maguro>(ed.region));
			Sound::play(Sound::SE::BossSpawn);
			break;
		default: break;
		}
	}
}

void Game::registMaptip()
{
	const Size sizeI = maptip_.size();
	const int sizeX = sizeI.x;
	const int sizeY = sizeI.y;

	for (int i = 0; i < sizeY / kBlockSize; ++i)
	{
		for (int j = 0; j < sizeX / kBlockSize; ++j)
		{
			maps_ << maptip_(j * kBlockSize, i * kBlockSize, kBlockSize, kBlockSize);
		}
	}
}

void Game::knockBack()
{
	player_->knockBackToEnemy(enemies_);
}

void Game::checkGoal()
{
	for (const auto& block : blocks_)
	{
		if (block.getFlag() == 4 && block.getRegion().intersects(player_->getRectF()))
		{
			Sound::play(Sound::SE::StageClear);
			const int currentStageID = getData().currentStageID;
			SaveManager::instance().recordCleared(currentStageID);
			const int nextID = StageRepository::instance().get(currentStageID).nextStageID;
			if (nextID > 0 && StageRepository::instance().contains(nextID))
			{
				getData().currentStageID = nextID;
				changeScene(SceneName::Game);
			}
			else
			{
				changeScene(SceneName::GameClear);
			}
			return;
		}
	}
}
