#pragma once
#include "Basic.hpp"
#include "ElipseUtil.hpp"

class StageElipse
{
public:
	StageElipse();
	~StageElipse();

	void draw() const;

	void readSelectedStage(size_t startArea, size_t endArea, size_t senterArea);

private:
	Array<ElipseUtil> elipseList_;
	size_t senterArea_ = 0;

};
