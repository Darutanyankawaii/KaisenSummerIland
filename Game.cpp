#include "Game.hpp"
#include "Collision.hpp"
#include "StageSelect.hpp"


Game::Game(const InitData& init) : IScene{ init }
{
	maptip = TextureAsset(U"MAP1");//ステージデータをステージに読み込み
	registMaptip();

	convertMapData(StageSelect::getStageData(getData().currentStageID).path, normalStage); //ステージのデータを読み込み
}

Game::~Game()
{
	Audio(U"shot").stopAllShots();
}

void Game::update()
{
	Cursor::RequestStyle(CursorStyle::Hidden);
	ClearPrint();

	// 更新関数
	updatePlayer();
	updateEnemies();
	updateBullets();

	knockBack(); // ノックバックの判定(敵とプレイヤーの衝突)
	checkItem(); //アイテムの獲得判定
	player_->lastUpdate(camera, playerBullets_);// プレイヤーの最終更新関数
	updateCamera(); //Cameraの更新
	checkGoal(); // ゴール判定

	//体力がゼロの時にGameOverシーンに遷移
	if (player_->getHp() <= 0)
	{
		changeScene(SceneName::GameOver);
	}
}

void Game::updatePlayer()
{
	// プレイヤーのX方向の更新
	player_->updateX();

	// x方向の移動制限
	player_->setPosX(Clamp(player_->getPosX(), 0.0, double(mapSize.x - player_->getSizeX())));

	// 壁との当たり判定
	Collision::CollisionWithWall(blocks, player_);

	// プレイヤーのY方向の更新
	player_->updateY();

	// 地面との当たり判定
	Collision::CollisionWithGround(blocks, player_);

	//玉攻撃におけるクールタイム
	if (!cooltimeFlag)
	{
		bulletAccumulator += Scene::DeltaTime();
		if (bulletAccumulator > 1)
		{
			cooltimeFlag = true;
			bulletAccumulator = 0;
		}
	}
}

void Game::updateEnemies() {

	// 敵のX方向の更新
	for (auto& enemy : enemies)
	{
		enemy->moveX();
		enemy->setPDir(player_->getDir());
		enemy->setPPos(player_->getPos());
		enemy->setPRect(player_->getRectF());
		enemy->getArea(normalStage.bossArea);
	}

	Collision::CollisionWithWall(blocks, enemies);

	// 敵のY方向の更新
	for (auto& enemy : enemies)
	{
		enemy->moveY();
	}

	Collision::CollisionWithGround(blocks, enemies);

	for (auto& enemy : enemies)
	{
		enemy->update();
	}
}

void Game::updateBullets()
{
	//球の移動
	for (auto& bullet : playerBullets_)
	{
		if (!bullet) continue;
		bullet->update();
	}

	//プレイヤーと敵の球の当たり判定
	if (cooltimeFlag)
	{
		//cooltimeFlag = Collision::CollisionWithBullet(enemyBullets_, player_);
	}

	// プレイヤーの弾とゲーム内のオブジェクトの当たり判定
	Collision::CheckBulletsAlive(playerBullets_, blocks, camera);

	//ボスのクリア判定要調整
	if (Collision::CollisionWithBullet(playerBullets_, enemies))
	{
		changeScene(SceneName::GameClear);
	}

	// 敵の弾とゲーム内のオブジェクトの当たり判定
	for (auto& enemy : enemies)
	{
		Collision::CollisionWithBullet(enemy->bullets, player_);
		Collision::CheckBulletsAlive(enemy->bullets, blocks, camera);
		// [Todo] Collision::CollisionWithBullet(enemies[i]->enemyBullets, hero);
	}

}

void Game::updateCamera() {
	camera.update();

	// Todo: Y軸方向のUpdate
	const double halfSceneWidth = Scene::Width() / 2.0f;
	float x = Clamp(player_->getCenter().x, halfSceneWidth, mapSize.x - halfSceneWidth);
	camera.setTargetCenter(Vec2(x, camera.getTargetCenter().y));
}

void Game::draw() const {
#ifdef DEBUGGING
	Print << U"ma" << maptip.size();
	Print << U"pos: {:.3}"_fmt(hero.pos);
	Print << U"speed: {:.3}"_fmt(hero.speed);
	Print << TextureAsset(U"backGround").size().x;
	Print << Scene::Width();

	Print << U"killflag ::" << bossKillFlag;
#endif

	// tの生存中はカメラ座標に基づいて座標変換が適用される
	{
		const auto t = camera.createTransformer();

		//ここをきれいに書きたい
		for (int i = -1; i < 5; i++)
		{
			TextureAsset(U"backGround").draw(TextureAsset(U"backGround").size().x * i, 0);
		}

		for (const auto& block : blocks)
		{
			block.getImage().draw(block.getPos());
		}
		// 当たり判定のあるブロックのみ当たり判定を表示
#ifdef DEBUGGING
		for (const auto& block : blocks)
		{
			block.getRectF().drawFrame(1, Palette::Seagreen);
		}
#endif DEBUGGING
		for(const auto& enemy : enemies)
		{
			enemy->draw();
		}

		for (const auto& bullet : playerBullets_)
		{
			TextureAsset(U"bullet").drawAt(bullet->getPos());
		}

		drawItem();

		// プレイヤーの描画
		player_->draw();

		TextureAsset(U"Lock_on").resized(30).drawAt(Cursor::Pos());

		// 射線を描画する
		//Line(hero.center(), Cursor::Pos()).draw(2.0, ColorF(1.0, 0.0, 0.0, 0.5));
	}

	// HPを描画
	for (int i = 0; i < player_->getHp(); i++)
	{
		TextureAsset(U"Heart").drawAt(680 - i * 45, 30);
	}

	// 半透明の円を描く
#ifdef DEBUGGING
	Circle{ Cursor::Pos(), 10 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
#endif
}

void Game::convertMapData(const String csv_path, Stage& tmpStage)
{

	CSV csv{ csv_path };

	if (not csv) {
		throw Error{ U"Failed to load CSV {}"_fmt(csv_path) };
	}

	mapSize = Point(csv.columns(0), csv.rows()) * blockSize;

	for (int row = 0; row < csv.rows(); row++)
	{
		for (int col = 0; col < csv.columns(row); col++)
		{
			const String stageSell = Parse<String>(csv[row][col]);
			//int sellNumber = (stageSell[1] - '0') * 10 + stageSell[2] - '0'; //要修正 原因空のセルがあるため
			const Vec2 objectPos = { col * blockSize, row * blockSize };
			Vec2 imagePos;

			switch (stageSell[0])
			{
				case 'p':
					setPlayer(objectPos);
					break;
				case 'B':
					imagePos = Vec2{ stageSell[2] - '0', stageSell[1] - '0' } * blockSize;
					putBlocks(objectPos, imagePos, 1);
					tmpStage.blockData << blockD{ Vec2{ col * blockSize, row * blockSize}, (stageSell[1] - '0') * 10 + stageSell[2] - '0', 1 };
					break;
				case 'N':
					imagePos = Vec2{ stageSell[2] - '0', stageSell[1] - '0' } *blockSize;
					putBlocks(objectPos, imagePos, -1);
					tmpStage.blockData << blockD{ Vec2{ col * blockSize, row * blockSize}, (stageSell[1] - '0') * 10 + stageSell[2] - '0', -1 };
					break;
				case 'H':
					imagePos = Vec2{ stageSell[2] - '0', stageSell[1] - '0' } *blockSize;
					putBlocks(objectPos, imagePos, 2);
					tmpStage.blockData << blockD{ Vec2{ col * blockSize, row * blockSize}, (stageSell[1] - '0') * 10 + stageSell[2] - '0', 2 };
					break;
				case 'i':
					putItem(objectPos, stageSell.narrow()[1] - '0');
					tmpStage.itemData << Items{ Vec2{ col * blockSize, row * blockSize}, stageSell.narrow()[1] - '0' };
					break;
				case 'E':
					putEnemy(objectPos, stageSell.narrow()[1] - '0');
					tmpStage.enemyData << Enemys{ objectPos, 1 };
					break;
			}


			if (stageSell[0] == 'G') {
				tmpStage.blockData << blockD{ Vec2{ col * blockSize, row * blockSize}, (stageSell[1] - '0') * 10 + stageSell[2] - '0', 4 };
			}
			else if (stageSell[0] == 'C') {
				Vec2 setBossArea = { (col - 0.5) * blockSize, (row - 0.5) * blockSize };
				tmpStage.bossArea << setBossArea;
				if (stageSell[1] == 'N') {
					tmpStage.blockData << blockD{ Vec2{ col * blockSize, row * blockSize}, (stageSell[1] - '0') * 10 + stageSell[2] - '0', -1 };
				}
			}
			else if (stageSell == U"1") {
				//ブロックの設定｛x座標,y座標,横の長さ,縦の長さ｝
				tmpStage.blockData << blockD{ Vec2{ col * blockSize, row * blockSize}, Parse<unsigned int>(stageSell), true };
			}
		}
	}
}

void Game::setPlayer(const Vec2& pos)
{
	player_ = std::make_unique<Player>();
	player_->setWeapon(static_cast<int>(WeaponName::Water_Gun));//プレイヤーの初期武器を設定
	player_->setPos(pos);
	camera.startPos = player_->getPos().movedBy(0, -1 * blockSize);
	camera = CustomCamera2D(camera.startPos);
}

void Game::putBlocks(const Vec2& blockPos, const Vec2& blockImagePos, int32 blockNum)
{
	const TextureRegion blockImage
		= maptip(blockImagePos, Vec2(blockSize, blockSize));
	switch (blockNum)
	{
	case -1://当たり判定のないブロック48x48
		//当たり判定を調整
		//blocks.push_back(Block( blockPos, Vec2{ blockSize, blockSize }, blockImage, -1));
		break;
	case 1://通常のブロック48x48
		blocks.push_back(Block( blockPos, Vec2{ blockSize, blockSize }, blockImage, 1));
		break;
	case 2://下から移動できるブロック48x24
		blocks.push_back(Block( blockPos, Vec2{ blockSize, blockSize / 2 }, blockImage, 2));
		break;
	case 3://下から移動できるブロック48x48
		blocks.push_back(Block( blockPos, Vec2{ blockSize, blockSize }, blockImage, 3));
		break;
	case 4://ゴールブロック
		blocks.push_back(Block(blockPos, Vec2{ blockSize, blockSize }, blockImage, 4));
	}
}

void Game::putEnemy(const Vec2& enemyPos, int32 enemyNum)
{
	switch (enemyNum)
	{
	case 1:
		enemies.push_back(std::make_unique<Curage>(enemyPos));
		break;
	case 2:
		enemies.push_back(std::make_unique<Kani>(enemyPos));
		break;
	case 3:
		enemies.push_back(std::make_unique<Tako>(enemyPos));
		break;
	case 9:
		enemies.push_back(std::make_unique<Maguro>(enemyPos));
		break;
	default:
		enemies.push_back(std::make_unique<Curage>(enemyPos));
		break;
	}
}

void Game::registMaptip()
{
	const Vec2 mapSize = maptip.size();

	for (int i = 0; i < mapSize.y / blockSize; i++)
	{
		for (int j = 0; j < mapSize.x / blockSize; j++)
		{
			maps << maptip(j * blockSize, i * blockSize, blockSize, blockSize);
		}
	}
}

void Game::knockBack()
{
	// playerとenemiesが衝突した際、1秒ごとにhero.HPが１減少し、ノックバック
	player_->knockBackToEnemy(enemies);
}

void Game::checkGoal()
{
	for (auto& block : blocks)
	{
		if (block.getFunction() == 4)
		{
			if (block.getRectF().intersects(player_->getRectF()))
			{
				// 次のシーンに移動する
				int currentStageID = getData().currentStageID;
				getData().currentStageID = StageSelect::getStageData(currentStageID).nextStageID;
				changeScene(SceneName::Game);
			}
		}
	}
}


