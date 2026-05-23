#pragma once
#include"Basic.hpp"

void RegisterAudioAsset() {
	AudioAsset::Register(U"walk", U"sound/walk.ogg");
	AudioAsset::Register(U"shot", U"sound/shot.ogg");
	AudioAsset::Register(U"bgm", U"sound/bgm.mp3");
}

void RegisterFontAsset() {
	FontAsset::Register(U"StageTitle", 40, Typeface::Black);
}

void ResisterTextureAsset() {
	// 弾の読み込み
	TextureAsset::Register(U"bullet", U"bullet.png");
	TextureAsset::Register(U"bullet2", U"bullet2.png");
	TextureAsset::Register(U"title", U"title.png");
	TextureAsset::Register(U"Lock_on", U"image/LockOn.png");

	// ブロックの読み込み
	TextureAsset::Register(U"MAP1", U"maptip.png");
	TextureAsset::Register(U"backGround", U"background.png");
	TextureAsset::Register(U"gameOver", U"gameOver.png");

	// 敵の読み込み
	TextureAsset::Register(U"OCTOPUS_1", U"image/octopus/octopus_1.png");
	TextureAsset::Register(U"OCTOPUS_2", U"image/octopus/octopus_2.png");
	TextureAsset::Register(U"MAGURO_1", U"image/boss/boss_left.png");
	TextureAsset::Register(U"MAGURO_2", U"image/boss/boss_right.png");

	//アイテム(武器)の読み込み
	TextureAsset::Register(U"GUN_NORMAL", U"image/item_gun/normal_gun.png");
	TextureAsset::Register(U"GUN_SHOT", U"image/item_gun/shot_gun.png");
	TextureAsset::Register(U"GUN_MACHINE", U"image/item_gun/machine_gun.png");

	for (int i = 1; i < (int)charNum; i++)
	{
		Array<TextureRegion> anime_;
		Texture image_;

		Texture setImage{ U"{}.png"_fmt((RegistChar)Zero + i) };
		const Vec2 SIZE = setImage.size();
		
		for (int j = 0; j < SIZE.x / SIZE.y; j++)
		{
			anime_ << image_(j * SIZE.y, SIZE.y, SIZE.y, SIZE.y);
			TextureAsset::Register(U"{}{}"_fmt(((RegistChar)Zero + i), j), U"{}.png"_fmt(((RegistChar)Zero + i)));
		}
	}

	// その他
	TextureAsset::Register(U"Heart", U"heart.png");
}
