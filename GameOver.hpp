#pragma once
#include "Basic.hpp"

class GameOver : public App::Scene
{
public:
	GameOver(const InitData& init);
	void update() override;
	void draw() const override;

private:
	Font titleFont{ 48, U"example/font/DotGothic16/DotGothic16-Regular.ttf" };
};
