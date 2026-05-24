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

	RectF start{ 510, 260, 190, 35 };
	RectF continueBtn{ 510, 310, 190, 35 };
	RectF howtoplay{ 610, 360, 90, 35 };
	RectF quit{ 510, 410, 190, 35 };

	String startText = U"ゲームスタート";
	String continueText = U"つづきから";
	String howtoplayText = U"遊び方";
	String quitText = U"ゲームを閉じる";
	String titleText = U"     海鮮★\nSummerアイランド";
};
