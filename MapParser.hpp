#pragma once
#include "LoadedStage.hpp"

// CSV からマップを読み込むパーサ。
// セル先頭文字でディスパッチし、各 handler 関数で個別のスポーンを処理する。
class MapParser
{
public:
	static LoadedStage parse(const String& csvPath);

private:
	struct CellContext
	{
		const String& cell;
		Vec2 cellPos;
		size_t col;
		size_t row;
		LoadedStage& out;
	};

	using Handler = void(*)(const CellContext&);

	static void handleBlock(const CellContext&);          // B
	static void handleNonCollidable(const CellContext&);  // N
	static void handleHalfBlock(const CellContext&);      // H
	static void handlePlayerStart(const CellContext&);    // p
	static void handleItem(const CellContext&);           // i
	static void handleEnemy(const CellContext&);          // E
	static void handleGoal(const CellContext&);           // G
	static void handleBossArea(const CellContext&);       // C
	static void handlePlainOne(const CellContext&);       // "1"

	static Handler lookupHandler(const String& cell);

	// CSV セルの 2 桁の数字 (cell[start] cell[start+1]) を int に変換
	static int parseTwoDigit(const String& cell, size_t start);
};
