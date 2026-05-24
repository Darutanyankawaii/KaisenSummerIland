#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Basic.hpp"
#include "AssetRegistry.hpp"
#include "AssetIDs.hpp"
#include "Game.hpp"
#include "StageSelect.hpp"
#include "Title.hpp"
#include "GameOver.hpp"
#include "GameClear.hpp"
#include "How2Play.hpp"

void Main()
{
	Window::Resize(SCENE_WIDTH, SCENE_HEIGHT);

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

	AudioAsset(GameAssets::Audio::BGM).setLoop(true);
	AudioAsset(GameAssets::Audio::BGM).play();

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
