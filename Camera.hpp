#pragma once
#include "Basic.hpp"

class CustomCamera2D : public Camera2D
{
public:
	CustomCamera2D() = default;

	CustomCamera2D(const Vec2& center, double scale = 1.0,
		const Camera2DParameters& setting = Camera2DParameters::NoControl())
		: Camera2D(center, scale, setting)
	{
	}

	Vec2 startPos{ 0, 0 };  // カメラの最初の位置

	Vec2 getTargetCenter() const { return this->m_targetCenter; }
};
