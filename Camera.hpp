#pragma once

class CustomCamera2D : public Camera2D {
public:
	CustomCamera2D() {}
	CustomCamera2D(const Vec2& getCenter, double scale = 1.0, const Camera2DParameters& setting = Camera2DParameters::NoControl())
		noexcept : Camera2D(getCenter, scale, setting) {
	};

	Vec2 startPos;	// カメラの最初の位置
	Vec2 getTargetCenter() const { return this->m_targetCenter; }
};

