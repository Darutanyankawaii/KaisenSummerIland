#pragma once
#include "Basic.hpp"
#include "Block.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Item.hpp"
#include "LoadedStage.hpp"

class Game : public App::Scene
{
public:
	Game(const InitData& init);
	~Game() override;
	void update() override;
	void draw() const override;
	void updateCamera();

	void updatePlayer();
	void updateEnemies();
	void updateBullets();

	void checkItem();
	void checkGoal();
	void drawItem() const;

	void PutItem(const LoadedStage& stage);
	void PutEnemy(const LoadedStage& stage);
	void PutBlocks(const LoadedStage& stage);
	void registMaptip();
	void knockBack();

private:
	std::unique_ptr<Player> player_;

	Array<std::unique_ptr<Bullet>> playerBullets_;

	bool cooltimeFlag_ = false;
	double bulletAccumulator_ = 0;

	Array<std::unique_ptr<Enemy>> enemies_;
	Array<Block> blocks_;

	Point mapSize_{ 0, 0 };
	Texture maptip_;
	Array<TextureRegion> maps_;

	Array<std::unique_ptr<Item>> items_;

	LoadedStage loadedStage_;

	CustomCamera2D camera_;
};
