#include "Title.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"
#include "SaveManager.hpp"

Title::Title(const InitData& init) : IScene{ init }
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });
}

void Title::update()
{
	const bool hasSave = SaveManager::instance().hasSave();

	if (start.leftClicked())
	{
		Sound::play(Sound::SE::MenuConfirm);
		changeScene(SceneName::StageSelect);
	}
	else if (hasSave && continueBtn.leftClicked())
	{
		Sound::play(Sound::SE::MenuConfirm);
		// 「つづきから」もステージ選択画面へ遷移するが、StageSelect 側で
		// 最初の未クリアステージへカーソルを合わせるため、追加の状態は不要。
		changeScene(SceneName::StageSelect);
	}
	else if (howtoplay.leftClicked())
	{
		Sound::play(Sound::SE::MenuConfirm);
		changeScene(SceneName::How2Play);
	}
	else if (quit.leftClicked())
	{
		Sound::play(Sound::SE::MenuConfirm);
		System::Exit();
	}
}

void Title::draw() const
{
	TextureAsset(GameAssets::Texture::Title).draw();

	const bool hasSave = SaveManager::instance().hasSave();

	start.draw(Palette::White).drawFrame(0, 3, Palette::Black);
	continueBtn.draw(hasSave ? ColorF{ 1.0 } : ColorF{ 0.7, 0.7, 0.7 })
		.drawFrame(0, 3, hasSave ? ColorF{ 0.0 } : ColorF{ 0.5 });
	howtoplay.draw(Palette::White).drawFrame(0, 3, Palette::Black);
	quit.draw(Palette::White).drawFrame(0, 3, Palette::Black);

	font(startText).draw(25, 520, 260, Palette::Black);
	font(continueText).draw(25, 520, 310,
		hasSave ? ColorF{ 0.0 } : ColorF{ 0.5 });
	font(howtoplayText).draw(25, 620, 360, Palette::Black);
	font(quitText).draw(25, 520, 410, Palette::Black);

	fontMSDF(titleText).drawAt(TextStyle::Outline(0.7, Palette::Black), 45, 505, 180, Palette::White);

	constexpr ColorF kHoverColor{ 139.0 / 255, 0.0, 0.0, 0.3 };
	if (start.mouseOver())
	{
		start.draw(kHoverColor);
	}
	else if (hasSave && continueBtn.mouseOver())
	{
		continueBtn.draw(kHoverColor);
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
