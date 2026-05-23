#include "Collision.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Block.hpp"
#include "Enemy.hpp"
#include "Camera.hpp"

// 壁とプレイヤーの当たり判定
void Collision::CollisionWithWall(const Array<Block>& blocks, std::unique_ptr<Player>& player)
{
	//ブロックの壁部分とプレイヤーの位置補正
	for (auto& block : blocks)
	{
		if (block.getFunction() == 1) {
			//プレイヤー右側面がブロックと接触
			if (player->getSpeedX() > 0)
			{
				if (block.getRectF().intersects(player->getRectF().right().stretched(-FLT_EPSILON)))   //FLT_EPSILON＝ものすごく小さい数0.0000000001
				{
					player->setSpeedX(block.getRectF().leftCenter().x - player->getSizeX() - FLT_EPSILON);
					player->setSpeedX(0);
				}
			}
			//プレイヤー左側面がブロックと接触
			if (player->getSpeedX() < 0) {
				if (block.getRectF().intersects(player->getRectF().left().stretched(-FLT_EPSILON)))
				{
					player->setSpeedX(block.getRectF().rightCenter().x + FLT_EPSILON);
					player->setSpeedX(0);
				}
			}
		}
	}
}

// 壁と敵の当たり判定
void Collision::CollisionWithWall(const Array<Block>& blocks, Array<std::unique_ptr<Enemy>>& enemies)
{
	//ブロックの壁部分とプレイヤーの位置補正
	for (auto& block : blocks)
	{
		if (block.getFunction() == 1)
		{
			for (auto& enemy : enemies)
			{
				if (enemy->hitbox == true)
				{
					// enemy右側面がブロックと接触
					if (enemy->speed_.x > 0)
					{
						if (block.getRectF().intersects(enemy->getRectF().right().stretched(-FLT_EPSILON)))   //FLT_EPSILON＝ものすごく小さい数0.0000000001
						{
							enemy->pos_.x = block.getRectF().leftCenter().x - enemy->SIZE.x - FLT_EPSILON;
							enemy->dir = false;
#ifdef DEBUGGING
							Print << U"Collision: {}"_fmt(j);
#endif
						}
					}

					// enemy左側面がブロックと接触
					if (enemy->speed_.x < 0) {
						if (block.getRectF().intersects(enemy->getRectF().left().stretched(-FLT_EPSILON)))
						{
							enemy->pos_.x = block.getRectF().rightCenter().x + FLT_EPSILON;
							enemy->dir = true;
#ifdef DEBUGGING
							Print << U"Collision: {}"_fmt(j);
#endif
						}
					}
				}
			}
		}
	}
}

// 床とプレイヤーの当たり判定
void Collision::CollisionWithGround(const Array<Block>& blocks, std::unique_ptr<Player>& player)
{
	player->setGround(false);

	for (auto& block :blocks)
	{
		if (block.getFunction() == 1)
		{
			//ブロックの床部分とプレイヤーの位置補正
			if (block.getRectF().intersects(player->getRectF().bottom()))
				//RectF[A].intersect(RectF[B]) AとBの図形が重なったときに[TRUE]を返す 
			{
				//プレイヤーの位置補正
				const double blockY = block.getRectF().topCenter().y;
				if (player->getPosY() + player->getSizeY() > blockY)
				{
					player->setPosY(blockY - player->getSizeY());
				}
				player->setSpeedY(0);
				player->setGround(true);
			}

			//ブロックの最下部とプレイヤーの位置補正
			if (player->getSpeedY() < 0)
			{
				if (block.getRectF().intersects(player->getRectF().top()))
				{
					//プレイヤーの位置補正
					const double blockY = block.getRectF().bottomCenter().y;
					if (player->getPosY() - player->getSizeY() < blockY)
					{
						player->setPosY(blockY + FLT_EPSILON);
					}
					player->setSpeedY(0);
				}
			}
		}
		else
		{
			//ブロックの床部分とプレイヤーの位置補正
			if (block.getRectF().intersects(player->getRectF().bottom()) && player->getSpeedY() >= 0)
				//RectF[A].intersect(RectF[B]) AとBの図形が重なったときに[TRUE]を返す 
			{
				////プレイヤーの位置補正
				const double blockY = block.getRectF().topCenter().y;
				player->setPosY(blockY - player->getSizeY());
				player->setSpeedY(0);
				player->setGround(true);
			}
		}
	}
}

// 床と敵の当たり判定
void Collision::CollisionWithGround(const Array<Block>& blocks, Array<std::unique_ptr<Enemy>>& enemies)
{
	for (auto& block :blocks)
	{
		for (auto& enemy : enemies)
		{
			if (enemy->hitbox == true)
			{
				if (block.getFunction() == 1)
				{
					//ブロックの床部分とプレイヤーの位置補正
					//RectF[A].intersect(RectF[B]) AとBの図形が重なったときに[TRUE]を返す
					if (block.getRectF().intersects(enemy->getRectF().bottom()))
					{
						//プレイヤーの位置補正
						double blockY = block.getRectF().topCenter().y;
						if (enemy->pos_.y + enemy->SIZE.y > blockY)
						{
							enemy->pos_.y = blockY - enemy->SIZE.y - FLT_EPSILON;
						}
						enemy->speed_.y = 0;
						enemy->accel_.y = 0;
					}

					//ブロックの最下部と敵の位置補正
					if (enemy->speed_.y < 0)
					{
						if (block.getRectF().intersects(enemy->getRectF().top()))
						{
							//敵の位置補正
							double blockY = block.getRectF().bottomCenter().y;
							if (enemy->pos_.y - enemy->SIZE.y < blockY)
							{
								enemy->pos_.y = blockY + FLT_EPSILON;
							}
							enemy->speed_.y = 0;
							enemy->accel_.y = 0;
						}
					}
				}
				else
				{
					//ブロックの床部分とプレイヤーの位置補正
					//RectF[A].intersect(RectF[B]) AとBの図形が重なったときに[TRUE]を返す
					if (block.getRectF().intersects(enemy->getRectF().bottom()) && enemy->speed_.y >= 0)
					{
						//プレイヤーの位置補正
						double blockY = block.getRectF().topCenter().y;
						enemy->pos_.y = blockY - enemy->SIZE.y;
						enemy->speed_.y = 0;
					}
				}
			}
		}
	}
}
// 弾の生存判定 (壁との衝突、画面外判定)
void Collision::CheckBulletsAlive(Array<std::unique_ptr<Bullet>>& bullets, const Array<Block>& blocks, const CustomCamera2D& camera)
{
	// 画面の範囲外にでたとき
	const Vec2 leftTop = camera.getMat3x2().inverse().transformPoint(Float2(0, 0));
	const RectF area = RectF(leftTop, SCENE_WIDTH, SCENE_HEIGHT).scaled(1.5, 1.5);
	for (auto it = bullets.begin(); it != bullets.end();)
	{
		if (!area.contains((*it)->getCircle()))
		{
			it = bullets.erase(it);
		}
		else
		{
			++it;
		}

	}

	// ブロックと衝突したとき
	for (const auto& block : blocks)
	{
		for (auto it = bullets.begin(); it != bullets.end();)
		{
			if (block.getRectF().intersects((*it)->getCircle()))
			{
				it = bullets.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

// 弾と敵の当たり判定
bool Collision::CollisionWithBullet(Array<std::unique_ptr<Bullet>>& bullets, Array<std::unique_ptr<Enemy>>& enemies)
{
	// 敵と衝突したとき
	for (auto enemy = enemies.begin(); enemy != enemies.end();)
	{
		bool isDelete = false;

		for (auto bt = bullets.begin(); bt != bullets.end();)
		{
			if ((*enemy)->getRectF().intersects((*bt)->getCircle()))
			{
				bt = bullets.erase(bt);

				(*enemy)->hp_--;

				if ((*enemy)->hp_ < 1)
				{
					if ((*enemy)->ID == -1)
					{
						enemy = enemies.erase(enemy);
						return true;
					}
					else
					{
						enemy = enemies.erase(enemy);
					}
					isDelete = true;
				}
				break;
			}
			else
			{
				++bt;
			}
		}

		if (!isDelete)
		{
			++enemy;
		}
	}

	return false;
}

// 弾と主人公の当たり判定
bool Collision::CollisionWithBullet(Array<std::unique_ptr<Bullet>>& bullets, std::unique_ptr<Player>& player)
{
	for (auto bt = bullets.begin(); bt != bullets.end();)
	{
		if (player->getRectF().intersects((*bt)->getCircle()))
		{
			bt = bullets.erase(bt);
			player->receiveDamage(1);
			return false;
			break;
		}
		else
		{
			++bt;
		}
	}
	return true;
}

void Collision::CollisionWithBullet(std::unique_ptr<Bullet>& bullet, std::unique_ptr<Player>& player)
{
	if (player->getRectF().intersects(bullet->getCircle()))
	{
		player->receiveDamage(1);
	}
}
