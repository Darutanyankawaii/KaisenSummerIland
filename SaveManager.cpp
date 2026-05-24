#include "SaveManager.hpp"

SaveManager& SaveManager::instance()
{
	static SaveManager s_instance;
	return s_instance;
}

SaveManager::SaveManager()
{
	load();
}

FilePath SaveManager::saveFilePath()
{
	// 実行ファイルと同じディレクトリに save.json を置く (cwd 依存を排除)
	return FileSystem::ParentPath(FileSystem::ModulePath()) + U"save.json";
}

void SaveManager::load()
{
	clearedIDs_.clear();
	const FilePath path = saveFilePath();
	if (not FileSystem::Exists(path)) return;

	const JSON json = JSON::Load(path);
	if (not json) return;
	if (not json.hasElement(kClearedStagesKey)) return;

	for (const auto& v : json[kClearedStagesKey].arrayView())
	{
		clearedIDs_.insert(v.get<int32>());
	}
}

void SaveManager::save() const
{
	Array<int32> sorted(clearedIDs_.begin(), clearedIDs_.end());
	sorted.sort();

	String text = U"{\n  \"" + String{ kClearedStagesKey } + U"\": [";
	for (size_t i = 0; i < sorted.size(); ++i)
	{
		if (i) text += U", ";
		text += Format(sorted[i]);
	}
	text += U"]\n}\n";

	const FilePath path = saveFilePath();
	TextWriter writer{ path };
	if (writer)
	{
		writer.write(text);
		Logger << U"[SaveManager] saved to " << path;
	}
	else
	{
		Logger << U"[SaveManager] FAILED to open " << path;
	}
}

void SaveManager::recordCleared(int stageID)
{
	if (clearedIDs_.contains(stageID)) return;
	clearedIDs_.insert(stageID);
	save();
}

bool SaveManager::isCleared(int stageID) const
{
	return clearedIDs_.contains(stageID);
}

void SaveManager::resetAll()
{
	clearedIDs_.clear();
	save();
}
