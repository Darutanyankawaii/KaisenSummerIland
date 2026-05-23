#pragma once
#include "Basic.hpp"

class StageSelect : public App::Scene
{
public:
	StageSelect(const InitData& init);
	void update() override;
	void draw() const override;

private:
	Array<int> stageIDList_;
	size_t selectedStage_ = 0;

	const double timeSpan_ = 3.0;
	const double timeSpace_ = 0.1;
	double timeAnim_ = 0;

	const double wheelSpace_ = 10;
	double wheelCount_ = 0.0;
	int wheelFlag_ = 0;
};
