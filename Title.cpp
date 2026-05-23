#include "Title.hpp"

Title::Title(const InitData& init) : IScene{ init }
{
	// 背景の色を設定 | Set background color
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });
}

void Title::update()
{

	if (start.leftClicked())
	{
		//ゲームシーンに遷移する
		changeScene(SceneName::StageSelect);
	}
	else if (howtoplay.leftClicked())
	{
		//遊び方のシーンに遷移
		changeScene(SceneName::How2Play);
	}
	else if (quit.leftClicked())
	{
		//プログラム終了
		System::Exit();
	}
}

void Title::draw() const
{
	texturetitle.draw();
	// 半透明の円を描く | Draw a semi-transparent circle
#ifdef DEBUGGING
	Circle{ Cursor::Pos(), 40 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
#endif
	
	start.draw(Palette::White).drawFrame(0, 3, Palette::Black);
	howtoplay.draw(Palette::White).drawFrame(0, 3, Palette::Black);
	quit.draw(Palette::White).drawFrame(0, 3, Palette::Black);

	font(startText).draw(25, 520, 300, Palette::Black);
	font(howtoplayText).draw(25, 620, 350, Palette::Black);
	font(quitText).draw(25, 520, 400, Palette::Black);

	fontMSDF(titleText).drawAt(TextStyle::Outline(0.7, Palette::Black), 45, 505, 220, Palette::White);
	////memo
	/*
	  右寄せ（昇順）
	　　-> (520, 300)
	    -> (620, 350)
		-> (520, 400)
	*/

	if (start.mouseOver())   
	{
		start.draw(ColorF(139, 0, 0, 0.3));
	}
	else if (howtoplay.mouseOver())
	{
		howtoplay.draw(ColorF(139, 0, 0, 0.3));
	}
	else if (quit.mouseOver())
	{
		quit.draw(ColorF(139, 0, 0, 0.3));
	}
}
