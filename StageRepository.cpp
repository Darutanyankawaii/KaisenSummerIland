#include "StageRepository.hpp"

StageRepository::StageRepository()
{
	load();
}

StageRepository& StageRepository::instance()
{
	static StageRepository repo;
	return repo;
}

void StageRepository::load()
{
	stages_.clear();
	sortedIDs_.clear();

	// ID 0 はフォールバック (存在しないステージ ID 要求時に返す)
	stages_.emplace(0, StageData{});

	const CSV csv{ U"Maps/MapList.csv" };
	if (not csv)
	{
		throw Error{ U"Failed to load `Maps/MapList.csv`" };
	}

	for (size_t i = 0; i < csv.rows(); ++i)
	{
		if (csv[i].size() < 4) continue;
		const int id = Parse<int>(csv[i][0]);
		BgKind bg = BgKind::Sea;
		if (csv[i].size() >= 5)
		{
			const String& tag = csv[i][4];
			if (tag == U"sky") bg = BgKind::Sky;
		}
		stages_.emplace(id, StageData{
			U"Maps/" + csv[i][1],
			csv[i][2],
			Parse<int>(csv[i][3]),
			bg
			});
	}

	for (const auto& [key, _] : stages_)
	{
		if (key != 0) sortedIDs_.push_back(key);
	}
	sortedIDs_.sort();
}

void StageRepository::reload()
{
	load();
}

StageData StageRepository::get(int stageID) const
{
	auto it = stages_.find(stageID);
	if (it != stages_.end()) return it->second;
	return stages_.at(0);
}

StageData StageRepository::next(int currentStageID) const
{
	auto it = stages_.find(currentStageID);
	if (it == stages_.end()) return get(0);
	return get(it->second.nextStageID);
}

Array<int> StageRepository::sortedIDs() const
{
	return sortedIDs_;
}

bool StageRepository::contains(int stageID) const
{
	return stages_.contains(stageID);
}
