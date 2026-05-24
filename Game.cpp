#include "Game.hpp"
#include "Collision.hpp"
#include "StageRepository.hpp"
#include "MapParser.hpp"
#include "AssetIDs.hpp"

namespace {
	constexpr int kBackgroundTileCount = 6;
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
}

Game::~Game()
{
	Audio(GameAssets::Audio::Shot).stopAllShots();
}

void Game::update()
{
	Cursor::RequestStyle(CursorStyle::Hidden);
	ClearPrint();

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

	if (Collision::CollisionWithBullet(playerBullets_, enemies_))
	{
		changeScene(SceneName::GameClear);
	}

	for (auto& enemy : enemies_)
	{
		Collision::CollisionWithBullet(enemy->bullets(), player_);
		Collision::CheckBulletsAlive(enemy->bullets(), blocks_, camera_);
	}
}

void Game::draw() const
{
	{
		const auto t = camera_.createTransformer();

		const auto bgSize = TextureAsset(GameAssets::Texture::Background).size();
		for (int i = -1; i < kBackgroundTileCount - 1; ++i)
		{
			TextureAsset(GameAssets::Texture::Background).draw(bgSize.x * i, 0);
		}

		for (const auto& bd : loadedStage_.blocks)
		{
			maps_[bd.num].draw(bd.pos);
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

		TextureAsset(GameAssets::Texture::LockOn).resized(30).drawAt(Cursor::Pos());
	}

	for (int i = 0; i < player_->getHp(); ++i)
	{
		TextureAsset(GameAssets::Texture::Heart).drawAt(680 - i * 45, 30);
	}
}

void Game::updateCamera()
{
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
		case 1: enemies_.push_back(std::make_unique<Curage>(ed.region)); break;
		case 2: enemies_.push_back(std::make_unique<Kani>(ed.region));   break;
		case 3: enemies_.push_back(std::make_unique<Tako>(ed.region));   break;
		case 9: enemies_.push_back(std::make_unique<Maguro>(ed.region)); break;
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
			const int currentStageID = getData().currentStageID;
			getData().currentStageID = StageRepository::instance().get(currentStageID).nextStageID;
			changeScene(SceneName::Game);
			return;
		}
	}
}
