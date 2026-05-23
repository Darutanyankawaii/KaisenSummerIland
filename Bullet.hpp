#pragma once
#include "Basic.hpp"

// 武器番号 (マップデータと一致させる必要があるため値は変更しない)
enum class WeaponName
{
	Water_Gun,   // 0
	Starfish,    // 1
	Shotgun,     // 2
	Machine_Gun, // 3
	Bucket,      // 4
};

// 弾の挙動パラメータ。武器ごとに異なる組合せを持たせる。
struct BulletParams
{
	double bulletSpeed = 10.0; // 弾速 (ブロック単位/秒)
	double fallSpeed = 1.0;    // 落下開始までのカウント
	int size = 8;              // 当たり判定の半径 (px)
	double lifeSpan = 3.0;     // 寿命 (秒)
};

class Bullet
{
public:
	// dir は単位ベクトル前提。createAimed() を経由するのが推奨。
	Bullet(const Vec2& pos, const Vec2& dir, const BulletParams& params);

	// 互換コンストラクタ: pos→target 方向に飛ばす (内部で正規化)
	Bullet(const Vec2& pos, const Vec2& target);

	// (Maguro 拡散弾のように) target が方向そのものでも吸収する。
	static std::unique_ptr<Bullet> createAimed(const Vec2& pos, const Vec2& target,
		const BulletParams& params);

	// pos から target への単位ベクトル。同位置の場合 (1,0) を返す。
	static Vec2 directionTo(const Vec2& from, const Vec2& to);

	void update();

	// ---- アクセサ (カプセル化) ----
	const Vec2& position() const { return pos_; }
	Vec2 getPos() const { return pos_; } // 旧 API
	Vec2 getDir() const { return dir_; }
	Circle getCircle() const { return Circle(pos_, size_); }
	double getLifeSpan() const { return lifeSpan_; }
	bool isHit() const { return hit_; }
	int getSize() const { return size_; }

	void decreaseLifeSpan(double dt) { lifeSpan_ -= dt; }
	void markHit() { hit_ = true; }

	void addPos(const Vec2& delta) { pos_ += delta; }
	void addDir(const Vec2& delta) { dir_ += delta; }

	// 旧 API 互換 (削除予定)
	void additionPos(const Vec2& delta) { addPos(delta); }
	void additionDir(const Vec2& delta) { addDir(delta); }

private:
	Vec2 pos_;
	Vec2 dir_;
	int size_;
	double bulletSpeed_;
	double fallSpeed_;
	double lifeSpan_;
	bool hit_ = false;
};
