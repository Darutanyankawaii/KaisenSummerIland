#pragma once
#include "Animation.hpp"

// プレイヤーのアニメーション一式 (状態 × 武器 × 方向)。
// 旧 StandAnimation/AimingAnimation の参照メンバ依存を排除。
// Player は毎フレーム tick(state, weapon, aimDir) を呼ぶだけで適切なテクスチャが取れる。
class PlayerAnimationSet
{
public:
	enum class State
	{
		Stand,
		Walk,
		Jump,
		Fall,
		Aiming,
	};

	// 全アニメをディスクから読み込む。
	// 存在しないファイルはスキップ (フォールバックは Stand/weapon=0)。
	void load();

	// 毎フレーム呼び出し。状態遷移時にリスタート、現在のアニメを update する。
	void tick(State state, int weapon, const Vec2& aimDir);

	// 描画用テクスチャを取得
	Texture currentTexture() const;

	bool isFinished() const;

	State currentState() const { return currentState_; }

	// 方向ベクトルから 0~6 の dir index を計算 (Aiming アニメ用)
	static int directionIndex(const Vec2& aimDir);

private:
	HashTable<int, Animation> stand_;            // key = weapon
	Animation walk_;
	Animation jump_;
	Animation fall_;
	HashTable<int, Animation> aiming_;           // key = weapon * 10 + dirIndex

	State currentState_ = State::Stand;
	int currentWeapon_ = 0;
	int currentDirIndex_ = 1;

	Animation* currentAnimation_ = nullptr;

	Animation* select(State state, int weapon, int dirIndex);
};
