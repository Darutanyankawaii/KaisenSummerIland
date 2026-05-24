#include "Collision.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Block.hpp"
#include "Enemy.hpp"
#include "Camera.hpp"

namespace {
	constexpr double kCollisionEpsilon = FLT_EPSILON;
	constexpr double kBulletAliveAreaScale = 1.5;

	// --- 壁との当たり判定 (共通テンプレート) ---
	// Entity は getPosX/setPosX/getSpeedX/getSizeX/getRectF を持つこと。
	template<class Entity, class OnRightHit, class OnLeftHit>
	void resolveWallTouch(const RectF& blockRegion, Entity& e,
		OnRightHit onRight, OnLeftHit onLeft)
	{
		// 右側面が壁にぶつかった場合
		if (e.getSpeedX() > 0)
		{
			if (blockRegion.intersects(e.getRectF().right().stretched(-kCollisionEpsilon)))
			{
				e.setPosX(blockRegion.leftCenter().x - e.getSizeX() - kCollisionEpsilon);
				onRight();
			}
		}
		// 左側面が壁にぶつかった場合
		if (e.getSpeedX() < 0)
		{
			if (blockRegion.intersects(e.getRectF().left().stretched(-kCollisionEpsilon)))
			{
				e.setPosX(blockRegion.rightCenter().x + kCollisionEpsilon);
				onLeft();
			}
		}
	}

	// --- 床/天井との当たり判定 (共通テンプレート) ---
	template<class Entity>
	void resolveGroundTouchSolid(const RectF& blockRegion, Entity& e,
		bool useGroundFlag, bool& outGroundedThisBlock)
	{
		// 床補正
		if (blockRegion.intersects(e.getRectF().bottom()))
		{
			const double blockTop = blockRegion.topCenter().y;
			if (e.getPosY() + e.getSizeY() > blockTop)
			{
				e.setPosY(blockTop - e.getSizeY());
			}
			e.setSpeedY(0);
			if (useGroundFlag) outGroundedThisBlock = true;
		}

		// 天井補正
		if (e.getSpeedY() < 0)
		{
			if (blockRegion.intersects(e.getRectF().top()))
			{
				const double blockBottom = blockRegion.bottomCenter().y;
				if (e.getPosY() - e.getSizeY() < blockBottom)
				{
					e.setPosY(blockBottom + kCollisionEpsilon);
				}
				e.setSpeedY(0);
			}
		}
	}
}

// ===== 壁との当たり判定 =====

void Collision::CollisionWithWall(const Array<Block>& blocks, std::unique_ptr<Player>& player)
{
	for (const auto& block : blocks)
	{
		if (block.getFlag() != 1) continue;
		resolveWallTouch(block.getRegion(), *player,
			[&] { player->setSpeedX(0); },
			[&] { player->setSpeedX(0); });
	}
}

void Collision::CollisionWithWall(const Array<Block>& blocks, Array<std::unique_ptr<Enemy>>& enemies)
{
	for (const auto& block : blocks)
	{
		if (block.getFlag() != 1) continue;
		for (auto& enemy : enemies)
		{
			if (!enemy->hasTerrainCollision()) continue;
			resolveWallTouch(block.getRegion(), *enemy,
				[&] { enemy->setFacingRight(false); },
				[&] { enemy->setFacingRight(true); });
		}
	}
}

// ===== 床/天井 =====

void Collision::CollisionWithGround(const Array<Block>& blocks, std::unique_ptr<Player>& player)
{
	player->setGround(false);

	for (const auto& block : blocks)
	{
		if (block.getFlag() == 1)
		{
			bool grounded = false;
			resolveGroundTouchSolid(block.getRegion(), *player, true, grounded);
			if (grounded) player->setGround(true);
		}
		else
		{
			// 上からのみ着地できる薄いブロック等
			if (block.getRegion().intersects(player->getRectF().bottom()) && player->getSpeedY() >= 0)
			{
				const double blockTop = block.getRegion().topCenter().y;
				player->setPosY(blockTop - player->getSizeY());
				player->setSpeedY(0);
				player->setGround(true);
			}
		}
	}
}

void Collision::CollisionWithGround(const Array<Block>& blocks, Array<std::unique_ptr<Enemy>>& enemies)
{
	for (const auto& block : blocks)
	{
		for (auto& enemy : enemies)
		{
			if (!enemy->hasTerrainCollision()) continue;

			if (block.getFlag() == 1)
			{
				bool grounded = false; // 敵では未使用
				resolveGroundTouchSolid(block.getRegion(), *enemy, false, grounded);
				// 着地系の補正後、敵の縦加速も 0 に
				if (enemy->getSpeedY() == 0)
				{
					enemy->setAccelY(0);
				}
			}
			else
			{
				if (block.getRegion().intersects(enemy->getRectF().bottom()) && enemy->getSpeedY() >= 0)
				{
					const double blockTop = block.getRegion().topCenter().y;
					enemy->setPosY(blockTop - enemy->getSizeY());
					enemy->setSpeedY(0);
				}
			}
		}
	}
}

// ===== 弾の生存判定 =====

void Collision::CheckBulletsAlive(Array<std::unique_ptr<Bullet>>& bullets,
	const Array<Block>& blocks, const CustomCamera2D& camera)
{
	// 画面外を削除
	const Vec2 leftTop = camera.getMat3x2().inverse().transformPoint(Float2(0, 0));
	const RectF area = RectF(leftTop, SCENE_WIDTH, SCENE_HEIGHT)
		.scaled(kBulletAliveAreaScale, kBulletAliveAreaScale);

	bullets.remove_if([&](const std::unique_ptr<Bullet>& b) {
		return !area.contains(b->getCircle());
		});

	// ブロックと衝突した弾を削除 (薄いブロック (足場) や装飾ブロックは貫通させる)
	bullets.remove_if([&](const std::unique_ptr<Bullet>& b) {
		for (const auto& block : blocks)
		{
			if (block.getFlag() != 1) continue;
			if (block.getRegion().intersects(b->getCircle())) return true;
		}
		return false;
		});
}

// ===== 弾と敵 =====

bool Collision::CollisionWithBullet(Array<std::unique_ptr<Bullet>>& bullets,
	Array<std::unique_ptr<Enemy>>& enemies)
{
	bool bossKilled = false;

	for (auto enemy = enemies.begin(); enemy != enemies.end();)
	{
		bool enemyDead = false;

		for (auto bt = bullets.begin(); bt != bullets.end();)
		{
			if ((*enemy)->getRectF().intersects((*bt)->getCircle()))
			{
				bt = bullets.erase(bt);
				(*enemy)->takeDamage(1);

				if ((*enemy)->getHp() < 1)
				{
					if ((*enemy)->isBoss()) bossKilled = true;
					enemyDead = true;
				}
				break;
			}
			else
			{
				++bt;
			}
		}

		if (enemyDead)
		{
			enemy = enemies.erase(enemy);
			if (bossKilled) return true;
		}
		else
		{
			++enemy;
		}
	}

	return false;
}

// ===== 弾とプレイヤー =====

bool Collision::CollisionWithBullet(Array<std::unique_ptr<Bullet>>& bullets,
	std::unique_ptr<Player>& player)
{
	for (auto bt = bullets.begin(); bt != bullets.end();)
	{
		if (player->getRectF().intersects((*bt)->getCircle()))
		{
			const Vec2 bulletPos = (*bt)->getPos();
			bt = bullets.erase(bt);
			player->onBulletHit(bulletPos);
			return true;
		}
		++bt;
	}
	return false;
}

