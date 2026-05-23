#pragma once
#include <Siv3D.hpp>

// アニメーションのInterface
class IAnimation {
public:
	// 更新メソッド(毎フレーム呼び出す)
	virtual void update() = 0;

	// 現在のフレームを取得
	virtual Texture getTexture() const = 0;

	// アニメーションが終了したか
	virtual bool isFinish() const = 0;

	// 現在のindexを取得
	virtual int getIndex() const = 0;

	// アニメーションを初期化
	virtual void init(int index = 0, bool isAwake = false) = 0;

	// アニメーションを開始
	virtual void start() = 0;

	// デストラクタ
	virtual ~IAnimation() {};
};

// アニメーションのテスクチャを管理するクラス
class Animation : public IAnimation {
private:
	// アニメーションの各フレームのテクスチャ
	Array<Texture> frames;
	// アニメーションの中割り(各フレーム間の時間)
	Array<float> layouts;
	// 現在のフレーム番号
	int index;
	// アニメーションをループさせるか？
	bool isLoop;
	// ループでない場合にアニメーションが最後まで到達したかどうか？
	bool _isFinish = false;
	// フレーム時間の計測タイマー
	Timer timer;

public:
	Animation() = default;

	// framesとlayoutsを渡して初期化するコンストラクタ
	Animation(const Array<Texture>& frames, const Array<float>& layouts,
		bool isLoop = false, bool isAwake = false, int index = 0);

	// ディレクトリから設定を読み込んで初期化するコンストラクタ
	Animation(const String& dirPath, bool isLoop = false, bool isAwake = false, int index = 0);

	virtual ~Animation() {};

	// 更新メソッド(毎フレーム呼び出す)
	virtual void update() override;

	// 現在のフレームを取得
	virtual Texture getTexture() const;

	// アニメーションが終了したか
	bool isFinish() const;

	// 現在のindexを取得
	int getIndex() const { return index; }

	// アニメーションを初期化
	void init(int index = 0, bool isAwake = false);

	// アニメーションを開始
	void start() {
		timer = Timer(Duration(layouts[index]), StartImmediately::Yes);
	}

	// Todo: アニメーションを一時停止
	// Todo: アニメーションを再開

private:
	// framesとlayoutsの読み込み
	virtual void load(const String& dirPath);
};

// Standに特化したアニメーション
class StandAnimation : public IAnimation {
private:
	// プレイヤーの所持するweapon
	int& weapon_;

	Array<IAnimation*> animations;

	bool isLoop;

public:
	StandAnimation(int& weapon_, const String& dirPath, bool isLoop = false, bool isAwake = false, int index = 0);

	// 更新メソッド(毎フレーム呼び出す)
	void update() override;

	// 現在のフレームを取得
	Texture getTexture() const override { return animations[weapon_]->getTexture(); }

	// アニメーションが終了したか
	bool isFinish() const override { return animations[weapon_]->isFinish(); }

	// 現在のindexを取得
	int getIndex() const override { return animations[weapon_]->getIndex(); }

	// アニメーションを初期化
	void init(int index = 0, bool isAwake = false) override {
		animations[weapon_]->init(index, isAwake);
	}

	// アニメーションを開始
	void start() override { animations[weapon_]->start(); }

private:
	// framesとlayoutsの読み込み
	void load(const String& dirPath);
};


// 攻撃に特化したアニメーション
class AimingAnimation : public IAnimation {
private:
	// プレイヤーの向いている方向の参照
	Vec2& playerDir;
	// プレイヤーの所持するweapon
	int& weapon_;
	// 方向に基づくframesのindexir;
	int dirIndex;
	// 現在のフレーム番号
	int index;
	// アニメーションをループさせるか？
	bool isLoop;
	// ループでない場合にアニメーションが最後まで到達したかどうか？
	bool _isFinish = false;

	// 各方向のAnimation
	//Array<IAnimation*> animations;
	HashTable<int, IAnimation*> animations;

public:
	AimingAnimation(Vec2& playerDir, int& weapon_, const String& dirPath, bool isLoop = false, bool isAwake = false, int index = 0);

	// 更新メソッド(毎フレーム呼び出す)
	void update() override;

	// 現在のフレームを取得
	Texture getTexture() const override { return animations.at(10 * weapon_ + dirIndex)->getTexture(); }

	// アニメーションが終了したか
	bool isFinish() const override { return animations.at(10 * weapon_ + dirIndex)->isFinish(); }

	// 現在のindexを取得
	int getIndex() const override { return animations.at(10 * weapon_ + dirIndex)->getIndex(); }

	// アニメーションを初期化
	void init(int index = 0, bool isAwake = false) override {
		animations.at(10 * weapon_ + dirIndex)->init(index, isAwake);
	}

	// アニメーションを開始
	void start() override { animations.at(10 * weapon_ + dirIndex)->start(); }

private:
	// framesとlayoutsの読み込み
	void load(const String& dirPath);
};
