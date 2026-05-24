#include "StageSelect.hpp"
#include "StageRepository.hpp"
#include "AssetIDs.hpp"

namespace {
	constexpr Color kColorA{ 255, 151, 75 };
	constexpr Color kColorB{ 0, 175, 110 };
	constexpr int kMargin = 50;
}

StageSelect::StageSelect(const InitData& init) : IScene{ init }
{
	selectedStage_ = 0;
	Scene::SetBackground(ColorF{ 0.0, 1.0, 1.0 });

	stageIDList_ = StageRepository::instance().sortedIDs();
}

void StageSelect::update()
{
	mouseWheelInput();
	gameStartKeyInput();
	gameSelectKeyInput();
	stageElipseUpdate();
}

void StageSelect::draw() const
{
	for (size_t i = 0; i < stageIDList_.size(); ++i)
	{
		const int yOffset = -50 * (static_cast<int>(i) - static_cast<int>(selectedStage_));
		const StageData sd = StageRepository::instance().get(stageIDList_[i]);

		FontAsset(GameAssets::Font::StageTitle)(sd.name)
			.drawAt(Scene::Center() + Point(0, yOffset),
				ColorF(1.0, 1.0, 1.0, (i == selectedStage_) ? 1.0 : 0.5));
	}


	stageElipse_.draw();
}

void StageSelect::mouseWheelInput()
{
	//マウスホイール
	//一定の値まで回すとカウント
	wheelCount_ += Mouse::Wheel() * 5;

	if (wheelCount_ > wheelSpace_)
	{
		wheelCount_ = 0;
		wheelFlag_ = -1;
	}
	else if (wheelCount_ < -wheelSpace_)
	{
		wheelCount_ = 0;
		wheelFlag_ = 1;
	}
}

void StageSelect::gameStartKeyInput()
{
	//ゲームスタート
	if ((KeyEnter | KeyZ | KeySpace | MouseL).down())
	{
		if (!stageIDList_.isEmpty())
		{
			getData().currentStageID = stageIDList_[selectedStage_];
			changeScene(SceneName::Game);
		}
	}
}

void StageSelect::gameSelectKeyInput()
{
	if (!stageIDList_.isEmpty())
	{
		if ((KeyW | KeyUp).down() || wheelFlag_ == 1)
		{
			wheelFlag_ = 0;
			selectedStage_ = (selectedStage_ + stageIDList_.size() - 1) % stageIDList_.size();
		}
		if ((KeyS | KeyDown).down() || wheelFlag_ == -1)
		{
			wheelFlag_ = 0;
			selectedStage_ = (selectedStage_ + 1) % stageIDList_.size();
		}
	}
}

void StageSelect::stageElipseUpdate()
{
	int32 startPoint = 0;
	int32 endPoint = stageIDList_.size() - 1;

	if (selectedStage_ == startPoint)
	{
		stageElipse_.readSelectedStage(2, 5, selectedStage_);
	}
	else if (selectedStage_ == startPoint + 1)
	{
		stageElipse_.readSelectedStage(1, 5, selectedStage_);
	}
	else if (selectedStage_ == (endPoint - 1))
	{
		stageElipse_.readSelectedStage(0, 4, selectedStage_);
	}
	else if (selectedStage_ == endPoint)
	{
		stageElipse_.readSelectedStage(0, 3, selectedStage_);
	}
	else
	{
		stageElipse_.readSelectedStage(0, 5, selectedStage_);
	}
}
