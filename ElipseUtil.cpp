#include "ElipseUtil.hpp"

ElipseUtil::ElipseUtil(const Vec2& pos, const Vec2& size, const State state)
	: pos_(pos),size_(size), ellipse_(pos,size), elipseState_(state)
{

}

ElipseUtil::~ElipseUtil()
{
}

void ElipseUtil::changePos(const Vec2& pos)
{
	pos_ = pos;
}

void ElipseUtil::changeSize(const Vec2& size)
{
	size_ = size;
}

void ElipseUtil::changeState(const State state)
{
	elipseState_ = state;
}

void ElipseUtil::draw() const
{
	if (elipseState_ == State::None)
	{
		return;
	}

	ellipse_
		.draw(elipseState_ == State::Attention ? Palette::Orange : Palette::Blue)
		.drawFrame(5, Palette::Black);
}
