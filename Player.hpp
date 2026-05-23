#pragma once
#include "Basic.hpp"
#include "Camera.hpp"
#include "Animation.hpp"
#include "Enemy.hpp"
#include "Object.hpp"

class Bullet;
class Enemy;

class Player
	//: public Object
{
public:
	Player();
	Player(const Vec2 pos);
	~Player();

	// X方向の更新処理
	void updateX();

	// Y方向の更新処理
	void updateY();

	// 最終更新処理
	void lastUpdate(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_);

	// 描画処理
	void draw() const;

	//ダメージ処理
	void recoverDamage(int damage);
	void receiveDamage(int damage);

	void knockBackToEnemy(Array<std::unique_ptr<Enemy>>& enemies);

	int getHp() const { return hp_; };
	int getDir() const { return playerDir_; };

	double getPosX() const { return pos_.x; };
	double getPosY() const { return pos_.y; };
	double getSpeedX() const { return speed_.x; };
	double getSpeedY() const { return speed_.y; };
	double getSizeX() const { return SIZE.x; };
	double getSizeY() const { return SIZE.y; };

	Vec2 getPos() const { return pos_; };
	Vec2 getSize() const { return SIZE; };
	Vec2 getCenter() const { return pos_ + SIZE / 2; };

	RectF getRectF() const { return RectF{ pos_, SIZE }; };

	void setGround(bool isGround) { isGround_ = isGround; };

	void setWeapon(int wepaon) { weapon_ = wepaon; };

	void setPosX(double x) { pos_ = Vec2{ x,pos_.y }; };
	void setPosY(double y) { pos_ = Vec2{ pos_.x,y }; };
	void setSpeedX(double x) { speed_ = Vec2{ x,speed_.y }; };
	void setSpeedY(double y) { speed_ = Vec2{ speed_.x,y }; };

	void setPos(Vec2 pos) { pos_ = pos; };

private:
	// プレイヤーの動作状態一覧
	enum State {
		stand,
		walk,
		jump,
		fall,
		aiming,
	};

	// アニメーション一覧
	OrderedTable<State, IAnimation*> animations;
	// 現在のアニメーション
	IAnimation* animation;

	// アニメーション関連の初期化
	void initAnimations();
	// アニメーションをロード
	IAnimation* loadAnimation(State state, bool isLoop = false);

	// State型変数をString型へ変換
	String stateToString(State state) const;

	// サウンド再生
	void playSound();

	// 攻撃処理
	void attack(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_);

	const Vec2 SET_SPPED = { 0, 0 };
	const Vec2 SET_ACCEL = { 0, 0 };
	const Vec2 SIZE{ 64,64 };

	const int32 WALK_SPEED = 5;//歩行速度

	Vec2 pos_;
	Vec2 speed_;
	Vec2 accel_;

	int32 playerDir_ = 1;// 向きの方向 左(-1),操作なし(0),右(1)
	Vec2 attackDir_; // 銃撃方向
	bool isGround_;// プレーヤーの地面の接着

	int32 hp_ = 5;// プレイヤーのHP

	//重力
	float gravity = 1.0f;

	//銃打っている判定
	bool aimFlag = false;

	//銃を打ってから暫く打ち続ける状態のフラグ
	bool shotNow = false;
	double shotTime = 0.0;

	// プレイヤーの動作状態
	State state = State::stand;
	State prevState = State::stand;

	// ==============
	// ノックバック関連
	// ==============
	// ノックバック中かどうか
	bool isKnockback = false;
	// 無敵状態かどうか
	bool isInvincible = false;
	// ノックバックの方向
	int knockBackDir = 1;

	//ノックバック時間の設定。
	Timer collisionalTimer{ 0.2s, StartImmediately::No };
	//invincibleは無敵という意味。無敵時間。
	Timer invincibleTimer{ 0.7s, StartImmediately::No };

	//現在の使用武器
	//0:水鉄砲,1:ヒトデ,2:散弾,3:連射,4:バケツ
	int weapon_ = 0;

	//クールタイム
	double cooltime;
};
