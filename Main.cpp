#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Basic.hpp"
#include "AssetRegistry.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"
#include "Game.hpp"
#include "StageSelect.hpp"
#include "Title.hpp"
#include "GameOver.hpp"
#include "GameClear.hpp"
#include "How2Play.hpp"

void Main()
{
	Window::Resize(SCENE_WIDTH, SCENE_HEIGHT);

	// Esc キーをタイトル復帰用に使うため、終了トリガーから除外する (× ボタンのみで終了)
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	App manager;
	manager
		.add<Title>(SceneName::Title)
		.add<StageSelect>(SceneName::StageSelect)
		.add<Game>(SceneName::Game)
		.add<GameOver>(SceneName::GameOver)
		.add<How2Play>(SceneName::How2Play)
		.add<GameClear>(SceneName::GameClear)
		;

	Scene::SetBackground(ColorF{ 1.0, 1.0, 1.0 });

	AssetRegistry::registerAll();
	Sound::init();

	Sound::start(Sound::SE::BGM);

	// FPS 制限 (sleep ベース)
	constexpr double kTargetFPS = 60.0;
	constexpr double kTargetMs = 1000.0 / kTargetFPS;
	Stopwatch sw;
	sw.start();

	while (System::Update())
	{
		if (!manager.update())
		{
			break;
		}
		const double elapsedMs = sw.msF();
		if (elapsedMs < kTargetMs)
		{
			System::Sleep(Duration{ (kTargetMs - elapsedMs) / 1000.0 });
		}
		sw.restart();
	}
}
