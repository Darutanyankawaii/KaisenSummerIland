#pragma once
#include <Siv3D.hpp> // OpenSiv3D v0.6.16

inline constexpr int SCENE_WIDTH = 720;
inline constexpr int SCENE_HEIGHT = 480;

inline constexpr int kBlockSize = 48;
// 旧コード互換 (新規コードは kBlockSize を使うこと)
inline constexpr int blockSize = kBlockSize;

enum class SceneName {
	Title,
	StageSelect,
	Game,
	How2Play,
	GameOver,
	GameClear
};

enum class BgKind
{
	Sea, // background.png (デフォルト、海と砂浜)
	Sky, // sky.png (空と雲)
};

struct StageData
{
	String path;
	String name;
	int nextStageID = 0;
	BgKind bg = BgKind::Sea;
};

struct GameData
{
	int currentStageID = 0;
};

// 文字種別を表す enum (Regist.hpp で素材ファイル名のキーに使用)。
// 既存コードが unscoped 名 (Zero, charNum) を直接参照しているため unscoped で維持。
enum RegistChar {
	Zero,
	Hero,
	CURAGE,
	KANI,
	TAKO,
	charNum  // 個数
};

using App = SceneManager<SceneName, GameData>;
