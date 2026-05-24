#pragma once
#include <functional>

class Player;
class Block;
class Enemy;
class Bullet;
class CustomCamera2D;

namespace Collision
{
	// 敵撃破時に呼ばれるコールバック (撃破座標, ボスかどうか)
	using OnEnemyDefeat = std::function<void(const Vec2&, bool isBoss)>;

	// 壁とプレイヤーの当たり判定
	void CollisionWithWall(const Array<Block>& blocks, std::unique_ptr<Player>& player);

	// 壁と敵の当たり判定
	void CollisionWithWall(const Array<Block>& blocks, Array<std::unique_ptr<Enemy>>& enemies);

	// 床とプレイヤーの当たり判定
	void CollisionWithGround(const Array<Block>& blocks, std::unique_ptr<Player>& player);

	// 床と敵の当たり判定
	void CollisionWithGround(const Array<Block>& blocks, Array<std::unique_ptr<Enemy>>& enemies);

	// 弾の生存判定 (壁との衝突、画面外判定)
	void CheckBulletsAlive(Array<std::unique_ptr<Bullet>>& bullets,
		const Array<Block>& blocks, const CustomCamera2D& camera);

	// 弾配列とプレイヤーの当たり判定 (ヒットしたら true)
	bool CollisionWithBullet(Array<std::unique_ptr<Bullet>>& bullets,
		std::unique_ptr<Player>& player);

	// 弾配列と敵配列の当たり判定 (ボス撃破で true)
	// onDefeat: 敵撃破時に呼ばれる (nullptr 可)
	bool CollisionWithBullet(Array<std::unique_ptr<Bullet>>& bullets,
		Array<std::unique_ptr<Enemy>>& enemies,
		const OnEnemyDefeat& onDefeat = nullptr);
}
