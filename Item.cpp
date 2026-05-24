#include "Game.hpp"
#include "Item.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"

void Game::PutItem(const LoadedStage& stage)
{
	for (const auto& itemSpawn : stage.items)
	{
		switch (itemSpawn.num)
		{
		case static_cast<int>(ItemName::Shaved_Ice):
			items_.push_back(std::make_unique<Shaved_Ice>(itemSpawn.region));
			break;
		case static_cast<int>(ItemName::Water_Gun):
			items_.push_back(std::make_unique<Water_Gun>(itemSpawn.region));
			break;
		case static_cast<int>(ItemName::Starfish):
			items_.push_back(std::make_unique<Starfish>(itemSpawn.region));
			break;
		case static_cast<int>(ItemName::Shotgun):
			items_.push_back(std::make_unique<Shotgun>(itemSpawn.region));
			break;
		case static_cast<int>(ItemName::Machine_Gun):
			items_.push_back(std::make_unique<Machine_Gun>(itemSpawn.region));
			break;
		case static_cast<int>(ItemName::Bucket):
			items_.push_back(std::make_unique<Bucket>(itemSpawn.region));
			break;
		default:
			break;
		}
	}
}

void Game::checkItem()
{
	for (auto it = items_.begin(); it != items_.end();)
	{
		if (player_->getRectF().intersects((*it)->get_rect()))
		{
			(*it)->get_item(player_);
			Sound::play(Sound::SE::ItemPickup);
			it = items_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Game::drawItem() const
{
	for (const auto& item : items_)
	{
		item->draw();
	}
}

// ===== Item =====

namespace {
	constexpr int kItemSize = 48;
}

Item::Item(const Vec2& region)
	: rect_(RectF(region, kItemSize))
{
}

const RectF& Item::get_rect() const
{
	return this->rect_;
}

// ===== かき氷 =====

Shaved_Ice::Shaved_Ice(const Vec2& region) : Item(region) {}

void Shaved_Ice::get_item(std::unique_ptr<Player>& player) const
{
	if (player->getHp() < 5)
	{
		player->recoverDamage(1);
		Sound::play(Sound::SE::HpRecover);
	}
}

void Shaved_Ice::draw() const
{
	this->get_rect().draw(Palette::Green).drawFrame(3, 3, Palette::Black);
}

// ===== 水鉄砲 =====

Water_Gun::Water_Gun(const Vec2& region) : Item(region) {}

void Water_Gun::get_item(std::unique_ptr<Player>& player) const
{
	player->setWeapon(static_cast<int>(WeaponName::Water_Gun));
}

void Water_Gun::draw() const
{
	TextureAsset(GameAssets::Texture::GunNormal).drawAt(this->get_rect().center());
}

// ===== ヒトデ =====

Starfish::Starfish(const Vec2& region) : Item(region) {}

void Starfish::get_item(std::unique_ptr<Player>& player) const
{
	player->setWeapon(static_cast<int>(WeaponName::Starfish));
}

void Starfish::draw() const
{
	this->get_rect().draw(Palette::Orange).drawFrame(3, 3, Palette::Black);
}

// ===== 散弾銃 =====

Shotgun::Shotgun(const Vec2& region) : Item(region) {}

void Shotgun::get_item(std::unique_ptr<Player>& player) const
{
	player->setWeapon(static_cast<int>(WeaponName::Shotgun));
}

void Shotgun::draw() const
{
	TextureAsset(GameAssets::Texture::GunShot).drawAt(this->get_rect().center());
}

// ===== 機関銃 =====

Machine_Gun::Machine_Gun(const Vec2& region) : Item(region) {}

void Machine_Gun::get_item(std::unique_ptr<Player>& player) const
{
	player->setWeapon(static_cast<int>(WeaponName::Machine_Gun));
}

void Machine_Gun::draw() const
{
	TextureAsset(GameAssets::Texture::GunMachine).drawAt(this->get_rect().center());
}

// ===== バケツ =====

Bucket::Bucket(const Vec2& region) : Item(region) {}

void Bucket::get_item(std::unique_ptr<Player>& player) const
{
	player->setWeapon(static_cast<int>(WeaponName::Bucket));
}

void Bucket::draw() const
{
	this->get_rect().draw(Palette::Silver).drawFrame(3, 3, Palette::Black);
}
