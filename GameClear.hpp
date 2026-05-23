#pragma once
#include "Basic.hpp"

class GameClear : public App::Scene
{
public:
	GameClear(const InitData& init);
	void update() override;
	void draw() const override;

private:
	// クリア表示用のフォント
	Font titleFont{ 48, U"example/font/DotGothic16/DotGothic16-Regular.ttf" };
};
