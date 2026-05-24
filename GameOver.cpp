#include "GameOver.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"

GameOver::GameOver(const InitData& init) : IScene{ init }
{
	Scene::SetBackground(Palette::Blue);
}

void GameOver::update()
{
	if ((KeyEnter | KeyZ | KeySpace | KeyEscape | MouseL).down())
	{
		Sound::play(Sound::SE::MenuConfirm);
		changeScene(SceneName::Title);
	}
}

void GameOver::draw() const
{
	TextureAsset(GameAssets::Texture::GameOver).drawAt(360, 240);
}
