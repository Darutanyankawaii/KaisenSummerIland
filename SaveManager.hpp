#pragma once
#include "Basic.hpp"

// クリア進捗の永続化を担うシングルトン。単一スロット、JSON 形式 (App/save.json)。
// 起動時 1 回ロード、recordCleared() 呼び出し毎に即座にディスクへ書き出す。
class SaveManager
{
public:
	static SaveManager& instance();

	// ステージクリアを記録 (重複は無視)。新規時のみディスクへ書き出す
	void recordCleared(int stageID);

	bool isCleared(int stageID) const;
	bool hasSave() const { return not clearedIDs_.empty(); }

	// テスト/デバッグ用にセーブを全消去
	void resetAll();

private:
	SaveManager();
	void load();
	void save() const;

	HashSet<int> clearedIDs_;

	static FilePath saveFilePath();

	static constexpr StringView kClearedStagesKey = U"clearedStages";
};
