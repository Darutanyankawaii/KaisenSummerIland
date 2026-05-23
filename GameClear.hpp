#pragma once
# include "Basic.hpp"

class GameClear :public App::Scene
{
public:
	GameClear(const InitData& init);//Titleシーンを起動すると一番最初に一度だけ起動する関数。変数や配列の初期化等にもちいる。
	void update() override;///Titleシーン中常に起動している関数。ゲーム内の様々な機能を管理する。
	void draw() const override;//Titleシーン中常に起動している描画用の関数。ゲーム内のUI等を管理する。
private:
	// 太文字のフォント
	Font Font{ 48, U"example/font/DotGothic16/DotGothic16-Regular.ttf" };
};
