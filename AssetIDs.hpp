#pragma once
#include "Basic.hpp"

// アセット ID の一元定義。
// 注: Siv3D に s3d::AssetID 型があるため "AssetID" 名は使えない (ADL 衝突)。
// プロジェクト固有の名前空間として "GameAssets" を採用。
namespace GameAssets
{
	namespace Texture
	{
		inline constexpr StringView Bullet = U"bullet";
		inline constexpr StringView Bullet2 = U"bullet2";
		inline constexpr StringView Title = U"title";
		inline constexpr StringView LockOn = U"Lock_on";
		inline constexpr StringView Map1 = U"MAP1";
		inline constexpr StringView Background = U"backGround";
		inline constexpr StringView GameOver = U"gameOver";
		inline constexpr StringView Octopus1 = U"OCTOPUS_1";
		inline constexpr StringView Octopus2 = U"OCTOPUS_2";
		inline constexpr StringView Maguro1 = U"MAGURO_1";
		inline constexpr StringView Maguro2 = U"MAGURO_2";
		inline constexpr StringView GunNormal = U"GUN_NORMAL";
		inline constexpr StringView GunShot = U"GUN_SHOT";
		inline constexpr StringView GunMachine = U"GUN_MACHINE";
		inline constexpr StringView Heart = U"Heart";
	}

	// 音声は SoundSystem (Sound::SE) で管理する

	namespace Font
	{
		inline constexpr StringView StageTitle = U"StageTitle";
	}
}
