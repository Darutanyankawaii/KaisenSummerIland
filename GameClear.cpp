#include "GameClear.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"

GameClear::GameClear(const InitData& init) : IScene{ init }
{
	Scene::SetBackground(Palette::Yellow);
}

void GameClear::update()
{
	if ((KeyEnter | KeyZ | KeySpace | MouseL).down())
	{
		Sound::play(Sound::SE::MenuConfirm);
		changeScene(SceneName::Title);
	}
}

void GameClear::draw() const
{
	TextureAsset(GameAssets::Texture::Title).draw(0, 0);
	titleFont(U"GameClear").drawAt(80, 360, 240, Palette::Red);
}
