#pragma once
#include "Basic.hpp"

class Title : public App::Scene
{
public:
	Title(const InitData& init);
	void update() override;
	void draw() const override;

private:
	Font font{ FontMethod::MSDF, 48 };
	Font fontMSDF{ FontMethod::MSDF, 48, Typeface::Bold };

	RectF start{ 510, 300, 190, 35 };
	RectF howtoplay{ 610, 350, 90, 35 };
	RectF quit{ 510, 400, 190, 35 };

	String startText = U"ゲームスタート";
	String howtoplayText = U"遊び方";
	String quitText = U"ゲームを閉じる";
	String titleText = U"     海鮮★\nSummerアイランド";
};
