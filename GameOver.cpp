#include "GameOver.hpp"

GameOver::GameOver(const InitData& init) : IScene{ init }
{
    // 背景の色を設定 | Set background color
    Scene::SetBackground(Palette::Blue);
}
    // GameOverを表示
void GameOver::update()
{
    //Font(U"GameOver").drawAt(80, 360, 240, Palette::Red);
    // KeyEnter | KeyZ | KeySpace | MouseLを押すとゲームをタイトルのシーンに移遷
    if((KeyEnter | KeyZ | KeySpace | MouseL).down())
	{
        changeScene(SceneName::Title);
    }
}


void GameOver::draw() const
{
	TextureAsset(U"gameOver").drawAt(360,240);
}
