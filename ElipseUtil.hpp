#pragma once

class ElipseUtil
{
public:
	enum class State
	{
		Attention,
		NotAttention,
		None
	};

	ElipseUtil(const Vec2& pos, const Vec2& size, State state);
	~ElipseUtil();

	void changePos(const Vec2& pos);
	void changeSize(const Vec2& size);
	void changeState(State state);

	void draw() const;

private:
	State elipseState_ = State::NotAttention;

	Ellipse ellipse_;
	Vec2 pos_;
	Vec2 size_;
};
