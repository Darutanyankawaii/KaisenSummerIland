#include "Title.hpp"
#include "AssetIDs.hpp"

Title::Title(const InitData& init) : IScene{ init }
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });
}

void Title::update()
{
	if (start.leftClicked())
	{
		changeScene(SceneName::StageSelect);
	}
	else if (howtoplay.leftClicked())
	{
		changeScene(SceneName::How2Play);
	}
	else if (quit.leftClicked())
	{
		System::Exit();
	}
}

void Title::draw() const
{
	TextureAsset(GameAssets::Texture::Title).draw();

	start.draw(Palette::White).drawFrame(0, 3, Palette::Black);
	howtoplay.draw(Palette::White).drawFrame(0, 3, Palette::Black);
	quit.draw(Palette::White).drawFrame(0, 3, Palette::Black);

	font(startText).draw(25, 520, 300, Palette::Black);
	font(howtoplayText).draw(25, 620, 350, Palette::Black);
	font(quitText).draw(25, 520, 400, Palette::Black);

	fontMSDF(titleText).drawAt(TextStyle::Outline(0.7, Palette::Black), 45, 505, 220, Palette::White);

	constexpr ColorF kHoverColor{ 139.0 / 255, 0.0, 0.0, 0.3 };
	if (start.mouseOver())
	{
		start.draw(kHoverColor);
	}
	else if (howtoplay.mouseOver())
	{
		howtoplay.draw(kHoverColor);
	}
	else if (quit.mouseOver())
	{
		quit.draw(kHoverColor);
	}
}
