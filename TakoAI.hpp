#pragma once
#include "Basic.hpp"

class Tako;

enum class TakoPhase
{
	Resting, // パターン 2 (3 秒待機)
	Dashing, // パターン 1 (0.8 秒突進)
};

class TakoAI
{
public:
	void tick(Tako& self, double dt);

private:
	TakoPhase phase_ = TakoPhase::Resting;
	double phaseTimer_ = 0.0;
	double gasTimer_ = 0.0;
	double phaseDuration_ = 1.0; // 最初は突進開始までの待ち
};
