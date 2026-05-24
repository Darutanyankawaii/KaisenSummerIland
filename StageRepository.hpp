#pragma once
#include "Basic.hpp"

// ステージメタデータの単一ロード/参照を担う。
// 旧 StageSelect::StageList の static inline 共有を排除し、明示的なシングルトン参照経由に。
class StageRepository
{
public:
	// 初回呼び出し時に CSV から読み込む (失敗時は Error)。
	static StageRepository& instance();

	StageData get(int stageID) const;
	StageData next(int currentStageID) const;
	Array<int> sortedIDs() const;
	bool contains(int stageID) const;

	// テスト/再読込用 (通常は使わない)
	void reload();

private:
	StageRepository();
	void load();

	HashTable<int, StageData> stages_;
	Array<int> sortedIDs_;
};
