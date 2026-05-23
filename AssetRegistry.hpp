#pragma once
#include "Basic.hpp"

// 全アセットの一括登録を担う。Main から起動時に 1 回呼ぶ。
class AssetRegistry
{
public:
	static void registerAll();

private:
	static void registerTextures();
	static void registerAudio();
	static void registerFonts();
	static void registerCharSheets();
};
