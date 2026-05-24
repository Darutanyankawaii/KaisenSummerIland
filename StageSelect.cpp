#include "StageSelect.hpp"
#include "StageRepository.hpp"
#include "AssetIDs.hpp"
#include "SoundSystem.hpp"
#include "SaveManager.hpp"

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

	// セーブがあれば最初の未クリアステージへカーソルを合わせる
	const auto& sm = SaveManager::instance();
	for (size_t i = 0; i < stageIDList_.size(); ++i)
	{
		if (not sm.isCleared(stageIDList_[i]))
		{
			selectedStage_ = i;
			break;
		}
	}
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
	const Point drawableSize = Scene::Center();
	const Point num = Point(drawableSize.x / kMargin + 1, drawableSize.y / kMargin + 1);

	if (timeAnim_ < timeSpan_)
	{
		for (int i = -num.x; i < num.x + 1; ++i)
		{
			for (int j = -num.y; j < num.y + 1; ++j)
			{
				Circle(drawableSize + Point(i, j) * kMargin, kMargin * timeAnim_ / timeSpan_).draw(kColorB);
			}
		}
	}
	else if (timeAnim_ < timeSpan_ * 2 + timeSpace_ && timeAnim_ >= timeSpan_ + timeSpace_)
	{
		for (int i = -num.x; i < num.x; ++i)
		{
			for (int j = -num.y; j < num.y; ++j)
			{
				Circle(drawableSize + (Vec2(i, j) + Vec2(0.5, 0.5)) * kMargin,
					kMargin * (timeAnim_ - timeSpan_) / timeSpan_).draw(kColorA);
			}
		}
	}

	const auto& sm = SaveManager::instance();
	for (size_t i = 0; i < stageIDList_.size(); ++i)
	{
		const int yOffset = 50 * (static_cast<int>(i) - static_cast<int>(selectedStage_));
		const StageData sd = StageRepository::instance().get(stageIDList_[i]);
		const String label = sm.isCleared(stageIDList_[i]) ? (sd.name + U" ✓") : sd.name;
		FontAsset(GameAssets::Font::StageTitle)(label)
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
			Sound::play(Sound::SE::MenuConfirm);
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
			Sound::play(Sound::SE::MenuSelect);
		}
		if ((KeyS | KeyDown).down() || wheelFlag_ == -1)
		{
			wheelFlag_ = 0;
			selectedStage_ = (selectedStage_ + 1) % stageIDList_.size();
			Sound::play(Sound::SE::MenuSelect);
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
