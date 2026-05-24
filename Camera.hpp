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

	// 画面シェイクをトリガー (intensity = px 振幅、duration = 秒)
	void triggerShake(double intensity, double duration)
	{
		// 既存より強い shake が来たら上書き
		if (intensity * duration > shakeIntensity_ * shakeTimer_)
		{
			shakeIntensity_ = intensity;
			shakeTimer_ = duration;
			shakeDuration_ = duration;
		}
	}

	void updateShake(double dt)
	{
		if (shakeTimer_ > 0.0)
		{
			shakeTimer_ = std::max(0.0, shakeTimer_ - dt);
		}
	}

	// 現在のシェイクオフセット (シェイク中以外は (0,0))
	Vec2 getShakeOffset() const
	{
		if (shakeTimer_ <= 0.0 || shakeDuration_ <= 0.0) return Vec2{ 0, 0 };
		const double ratio = shakeTimer_ / shakeDuration_; // 1.0 → 0.0 で減衰
		const double amp = shakeIntensity_ * ratio;
		return Vec2{ Random(-amp, amp), Random(-amp, amp) };
	}

private:
	double shakeIntensity_ = 0.0;
	double shakeTimer_ = 0.0;
	double shakeDuration_ = 0.0;
};
