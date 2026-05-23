#include "GameClear.hpp"

GameClear::GameClear(const InitData & init) : IScene{ init }
{
	// 背景の色を設定 | Set background color
	Scene::SetBackground(Palette::Yellow);

}
// GameClearを表示
void GameClear::update()
{
	Font(U"GameClear").drawAt(80, 360, 240, Palette::Red);
	// KeyEnter | KeyZ | KeySpace | MouseLを押すとゲームをタイトルのシーンに移遷
	if ((KeyEnter | KeyZ | KeySpace | MouseL).down()) {
		changeScene(SceneName::Title);
	}
}


void GameClear::draw() const {
	TextureAsset(U"title").draw(0, 0);
	Font(U"GameClear").drawAt(80, 360, 240, Palette::Red);
}
