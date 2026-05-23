#include "Game.hpp"
#include "Item.hpp"

//Game=====

void Game::putItem(const Vec2& itemPos, int32 itemNum)
{
	switch (itemNum)
	{
	case static_cast<int>(ItemName::Shaved_Ice)://回復アイテム
		items << (Item*)new Shaved_Ice(itemPos);
		break;
	case static_cast<int>(ItemName::Water_Gun):
		items << (Item*)new Water_Gun(itemPos);
		break;
	case static_cast<int>(ItemName::Starfish):
		items << (Item*)new Starfish(itemPos);
		break;
	case static_cast<int>(ItemName::Shotgun):
		items << (Item*)new Shotgun(itemPos);
		break;
	case static_cast<int>(ItemName::Machine_Gun):
		items << (Item*)new Machine_Gun(itemPos);
		break;
	case static_cast<int>(ItemName::Bucket):
		items << (Item*)new Bucket(itemPos);
		break;
	default:
		break;
	}
	//for (size_t i = 0; i < stage.itemData.size(); i++)
	//{
	//	switch (stage.itemData[i].num)
	//	{
	//	case static_cast<int>(ItemName::Shaved_Ice)://回復アイテム
	//		items << (Item*)new Shaved_Ice(stage.itemData[i].region_);
	//		break;
	//	case static_cast<int>(ItemName::Water_Gun):
	//		items << (Item*)new Water_Gun(stage.itemData[i].region_);
	//		break;
	//	case static_cast<int>(ItemName::Starfish):
	//		items << (Item*)new Starfish(stage.itemData[i].region_);
	//		break;
	//	case static_cast<int>(ItemName::Shotgun):
	//		items << (Item*)new Shotgun(stage.itemData[i].region_);
	//		break;
	//	case static_cast<int>(ItemName::Machine_Gun):
	//		items << (Item*)new Machine_Gun(stage.itemData[i].region_);
	//		break;
	//	case static_cast<int>(ItemName::Bucket):
	//		items << (Item*)new Bucket(stage.itemData[i].region_);
	//		break;
	//	default:
	//		break;
	//	}
	//}
}

void Game::checkItem() {

	//プレイヤーとアイテムの当たり判定
	for (auto it = items.begin(); it != items.end();)
	{
		if (player_->getRectF().intersects((*it)->get_rect()))
		{
			(*it)->get_item(player_);
			it = items.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void Game::drawItem() const
{
	for (const auto& item : items)
	{
		item->draw();
	}
}

//アイテム=====

Item::Item(const Vec2& region_)
{
	//アイテムのサイズ
	constexpr int itemsize = 48;
	this->rect = RectF(region_, itemsize);
}

Item::~Item()
{

}

const RectF& Item::get_rect() const
{
	return this->rect;
}

//かき氷=====

Shaved_Ice::Shaved_Ice(const Vec2& region_)
	:Item(region_)
{

}

Shaved_Ice::~Shaved_Ice()
{
}

void Shaved_Ice::get_item(std::unique_ptr<Player>& player_) const
{
	if (player_->getHp() < 5) player_->recoverDamage(1);
}

void Shaved_Ice::draw() const
{
	this->get_rect().draw(Palette::Green).drawFrame(3, 3, Palette::Black);
}

//水鉄砲=====

Water_Gun::Water_Gun(const Vec2& region_)
	:Item(region_)
{

}

Water_Gun::~Water_Gun()
{

}

void Water_Gun::get_item(std::unique_ptr<Player>& player_) const
{
	player_->setWeapon(static_cast<int>(WeaponName::Water_Gun));
}

void Water_Gun::draw() const
{
	TextureAsset(U"GUN_NORMAL").drawAt(this->get_rect().center());
	//this->get_rect().draw(Palette::Blue).drawFrame(3, 3, Palette::Black);
}

//ヒトデ=====

Starfish::Starfish(const Vec2& region_)
	:Item(region_)
{

}

Starfish::~Starfish()
{

}

void Starfish::get_item(std::unique_ptr<Player>& player_) const
{
	player_->setWeapon(static_cast<int>(WeaponName::Starfish));
}

void Starfish::draw() const
{
	this->get_rect().draw(Palette::Orange).drawFrame(3, 3, Palette::Black);
}

//散弾銃=====

Shotgun::Shotgun(const Vec2& region_)
	:Item(region_)
{

}

Shotgun::~Shotgun()
{

}

void Shotgun::get_item(std::unique_ptr<Player>& player_) const
{
	player_->setWeapon(static_cast<int>(WeaponName::Shotgun));
}

void Shotgun::draw() const
{
	TextureAsset(U"GUN_SHOT").drawAt(this->get_rect().center());
	//this->get_rect().draw(Palette::Brown).drawFrame(3, 3, Palette::Black);
}

//機関銃=====

Machine_Gun::Machine_Gun(const Vec2& region_)
	:Item(region_)
{

}

Machine_Gun::~Machine_Gun()
{

}

void Machine_Gun::get_item(std::unique_ptr<Player>& player_) const
{
	player_->setWeapon(static_cast<int>(WeaponName::Machine_Gun));
}

void Machine_Gun::draw() const
{
	TextureAsset(U"GUN_MACHINE").drawAt(this->get_rect().center());
	//this->get_rect().draw(Palette::Black).drawFrame(3, 3, Palette::Black);
}

//バケツ=====

Bucket::Bucket(const Vec2& region_)
	:Item(region_)
{

}

Bucket::~Bucket()
{

}

void Bucket::get_item(std::unique_ptr<Player>& player_) const
{
	player_->setWeapon(static_cast<int>(WeaponName::Bucket));
}

void Bucket::draw() const
{
	this->get_rect().draw(Palette::Silver).drawFrame(3, 3, Palette::Black);
}
