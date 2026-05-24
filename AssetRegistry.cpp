#include "AssetRegistry.hpp"
#include "AssetIDs.hpp"

void AssetRegistry::registerAll()
{
	registerAudio();
	registerFonts();
	registerTextures();
	registerCharSheets();
}

void AssetRegistry::registerAudio()
{
	// 音声は SoundSystem (Sound::init) で一元管理する
}

void AssetRegistry::registerFonts()
{
	FontAsset::Register(String{ GameAssets::Font::StageTitle }, 40, Typeface::Black);
}

void AssetRegistry::registerTextures()
{
	// 弾・タイトル等
	TextureAsset::Register(String{ GameAssets::Texture::Bullet }, U"bullet.png");
	TextureAsset::Register(String{ GameAssets::Texture::Bullet2 }, U"bullet2.png");
	TextureAsset::Register(String{ GameAssets::Texture::Title }, U"title.png");
	TextureAsset::Register(String{ GameAssets::Texture::LockOn }, U"image/LockOn.png");

	// ブロック・背景
	TextureAsset::Register(String{ GameAssets::Texture::Map1 }, U"maptip.png");
	TextureAsset::Register(String{ GameAssets::Texture::Background }, U"background.png");
	TextureAsset::Register(String{ GameAssets::Texture::Sky }, U"sky.png");
	TextureAsset::Register(String{ GameAssets::Texture::GameOver }, U"gameOver.png");

	// 敵
	TextureAsset::Register(String{ GameAssets::Texture::Octopus1 }, U"image/octopus/octopus_1.png");
	TextureAsset::Register(String{ GameAssets::Texture::Octopus2 }, U"image/octopus/octopus_2.png");
	TextureAsset::Register(String{ GameAssets::Texture::Maguro1 }, U"image/boss/boss_left.png");
	TextureAsset::Register(String{ GameAssets::Texture::Maguro2 }, U"image/boss/boss_right.png");
	TextureAsset::Register(String{ GameAssets::Texture::AppleMan }, U"AppleMan.png");
	TextureAsset::Register(String{ GameAssets::Texture::Fish }, U"fish.png");

	// 武器アイテム
	TextureAsset::Register(String{ GameAssets::Texture::GunNormal }, U"image/item_gun/normal_gun.png");
	TextureAsset::Register(String{ GameAssets::Texture::GunShot }, U"image/item_gun/shot_gun.png");
	TextureAsset::Register(String{ GameAssets::Texture::GunMachine }, U"image/item_gun/machine_gun.png");

	// その他
	TextureAsset::Register(String{ GameAssets::Texture::Heart }, U"heart.png");
}

void AssetRegistry::registerCharSheets()
{
	// キャラクター画像の連番登録 (旧 Regist.hpp の挙動を保持)
	for (int i = 1; i < static_cast<int>(charNum); ++i)
	{
		const RegistChar charKind = static_cast<RegistChar>(static_cast<int>(Zero) + i);
		const Texture setImage{ U"{}.png"_fmt(static_cast<int>(charKind)) };
		const Size sizeI = setImage.size();
		if (sizeI.x <= 0 || sizeI.y <= 0) continue;
		const int columns = sizeI.x / sizeI.y;
		for (int j = 0; j < columns; ++j)
		{
			TextureAsset::Register(
				U"{}{}"_fmt(static_cast<int>(charKind), j),
				U"{}.png"_fmt(static_cast<int>(charKind)));
		}
	}
}
