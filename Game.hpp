#pragma once
#include "Basic.hpp"
#include "Block.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Item.hpp"

class Game :public App::Scene
{
private:
	std::unique_ptr<Player> player_;//プレイヤー関連
	//玉関連
	Array<std::unique_ptr<Bullet>> playerBullets_;
	Array<std::unique_ptr<Bullet>> enemyBullets_;

	bool cooltimeFlag;
	double bulletAccumulator = 0;

	const int bulletSpeed = 10;

	//敵関連
	struct Enemys
	{
		Vec2 region_;
		int num;
	};//敵のデータ格納用

	Array<std::unique_ptr<Enemy>> enemies;

	//ブロック関連
	struct blockD {
		Vec2 pos_;
		unsigned int num;
		int flag_;
	};//ブロックのデータ格納用

	Array<Block> blocks;
	Array<Block> bossBlocks;

	Point mapSize;		// マップの大きさ
	Texture maptip;
	Array<TextureRegion> maps;

	//アイテム関連
	
	Array<Item*> items;
	struct Items
	{
		Vec2 region_;
		int num;
	};

	//データ関連
	struct Stage {
		Array<blockD> blockData;
		Array<Enemys> enemyData;
		Array<Items> itemData;
		Array<Vec2> bossArea;
	};

	Stage normalStage;

	// Camera
	CustomCamera2D camera;

	bool bossKillFlag = false;
public:
	Game(const InitData& init);//Gameシーンを起動すると一番最初に一度だけ起動する関数。変数や配列の初期化等にもちいる。
	~Game();		// 終了処理
	void update() override;//Gameシーン中常に起動している関数。ゲーム内の様々な機能を管理する。
	void draw() const override;//Gameシーン中常に起動している描画用の関数。ゲーム内のUI等を管理する。
	void updateCamera();


	void updatePlayer(); // プレイヤーの更新
	void updateEnemies(); // 敵の更新
	void updateBullets(); // 弾の更新

	//アイテムとキャラクターの当たり判定を管理する関数(Item.cpp内で定義)
	void checkItem();
	// ゴール判定
	void checkGoal();
	//アイテムを描画する関数(Item.cpp内で定義)
	void drawItem() const;
	//csvファイルの内容を変数に変換する関数
	void convertMapData(const String, Stage&);
	//アイテムを生成する関数(Item.cpp内で定義)
	void putItem(const Vec2&, int32);
	//敵の情報を配列に格納する関数
	void putEnemy(const Vec2&, int32);
	//ブロックの情報を配列に格納する関数
	void putBlocks(const Vec2&, const Vec2&, int32);
	//マップの情報を配列に格納する関数
	void registMaptip();
	//敵にぶつかると発生するノックバックの関数
	void knockBack();
	//プレイヤーの定義
	void setPlayer(const Vec2&);
};
