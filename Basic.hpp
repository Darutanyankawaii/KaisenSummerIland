#pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.6.11

static constexpr int SCENE_WIDTH = 720;
static constexpr int SCENE_HEIGHT = 480;

const int blockSize = 48;

enum class SceneName {
	Title,
	StageSelect,
	Game,
	How2Play,
	GameOver,
	GameClear
};

struct StageData
{
	String path;
	String name;
	int nextStageID;
};

struct GameData
{
	int currentStageID;
};

enum RegistChar {
	Zero,
	Hero,
	CURAGE,
	KANI,
	TAKO,
	charNum//個数
};

using App = SceneManager<SceneName, GameData>;
