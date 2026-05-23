#include "GameOver.hpp"
#include "AssetIDs.hpp"

GameOver::GameOver(const InitData& init) : IScene{ init }
{
	Scene::SetBackground(Palette::Blue);
}

void GameOver::update()
{
	if ((KeyEnter | KeyZ | KeySpace | MouseL).down())
	{
		changeScene(SceneName::Title);
	}
}

void GameOver::draw() const
{
	TextureAsset(GameAssets::Texture::GameOver).drawAt(360, 240);
}
