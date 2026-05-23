#pragma once
#include "Basic.hpp"

// マップ CSV から読み込んだステージのスポーンデータ。
// Game に inner type で持っていたものを独立させ、MapParser から共有可能に。

struct BlockSpawn
{
	Vec2 pos{ 0, 0 };
	int num = 0;
	int flag = 0;
};

struct EnemySpawn
{
	Vec2 region{ 0, 0 };
	int num = 0;
};

struct ItemSpawn
{
	Vec2 region{ 0, 0 };
	int num = 0;
};

struct LoadedStage
{
	Array<BlockSpawn> blocks;
	Array<EnemySpawn> enemies;
	Array<ItemSpawn> items;
	Array<Vec2> bossArea;
	Vec2 playerStart{ 0, 0 };
	bool playerStartSet = false;
	Point mapSize{ 0, 0 };
};
