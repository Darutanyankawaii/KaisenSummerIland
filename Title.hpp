#pragma once
# include "Basic.hpp"

class Title :public App::Scene
{
public:
	Title(const InitData& init);//Titleシーンを起動すると一番最初に一度だけ起動する関数。変数や配列の初期化等にもちいる。
	void update() override;///Titleシーン中常に起動している関数。ゲーム内の様々な機能を管理する。
	void draw() const override;//Titleシーン中常に起動している描画用の関数。ゲーム内のUI等を管理する。
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

	const Texture texturetitle{ U"title.png" };
};
