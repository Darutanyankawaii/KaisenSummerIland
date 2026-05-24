#include "How2Play.hpp"
#include "SoundSystem.hpp"

How2Play::How2Play(const InitData& init) : IScene{ init }
{
	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });
}

void How2Play::update()
{
	if (back.leftClicked())
	{
		Sound::play(Sound::SE::MenuCancel);
		changeScene(SceneName::Title);
	}
}

void How2Play::draw() const
{
#ifdef DEBUGGING
	Circle{ Cursor::Pos(), 40 }.draw(ColorF{ 1.0, 0.0, 0.0, 0.5 });
#endif

	back.draw(Palette::White).drawFrame(0, 3, Palette::Black);

	font(backText).draw(25, 30, 25, Palette::Black);
	font(how2play_mouse).draw(30, 120, 25, Palette::Black);
	font(how2play_key).draw(30, 120, 130, Palette::Black);
	font(how2play_space).draw(30, 120, 230, Palette::Black);
	font(how2play_rule1).draw(30, 120, 300, Palette::Black);
	font(how2play_rule2).draw(30, 120, 370, Palette::Black);

	if (back.mouseOver())
	{
		back.draw(ColorF{ 139.0 / 255, 0.0, 0.0, 0.3 });
	}
}
