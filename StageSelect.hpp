#pragma once
#include "Basic.hpp"
#include "ElipseUtil.hpp"
#include "StageElipse.hpp"

class StageSelect : public App::Scene
{
public:
	StageSelect(const InitData& init);
	void update() override;
	void draw() const override;

private:
	void mouseWheelInput();
	void gameStartKeyInput();
	void gameSelectKeyInput();
	void stageElipseUpdate();

	Array<int32> stageIDList_;
	size_t selectedStage_ = 0;

	StageElipse stageElipse_;

	const double timeSpan_ = 3.0;
	const double timeSpace_ = 0.1;
	double timeAnim_ = 0;

	const double wheelSpace_ = 10;
	double wheelCount_ = 0.0;
	int wheelFlag_ = 0;
};
