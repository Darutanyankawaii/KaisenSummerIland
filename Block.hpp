#pragma once
#include "Basic.hpp"

class Block
{
public:
	Block() = default;

	Block(const RectF& region, int flag)
		: region_(region), flag_(flag)
	{
	}

	const RectF& getRegion() const { return region_; }
	int getFlag() const { return flag_; }

	void setRegion(const RectF& region) { region_ = region; }
	void setFlag(int flag) { flag_ = flag; }

private:
	RectF region_{ Vec2{ 0, 0 }, 0 };
	int flag_ = 0;
};
