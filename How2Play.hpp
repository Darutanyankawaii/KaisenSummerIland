#pragma once
#include "Basic.hpp"

class How2Play : public App::Scene
{
public:
	How2Play(const InitData& init);
	void update() override;
	void draw() const override;

private:
	Font font{ FontMethod::MSDF, 48 };

	RectF back{ 20, 25, 70, 35 };

	String backText = U"戻る";
	String how2play_mouse = U"★マウス右クリックで狙いを定める\n　マウス左クリックで水鉄砲発射";
	String how2play_key = U"★'A or ←'キーで左に移動\n　'D or →'キーで右に移動";
	String how2play_space = U"★'Space'キーでジャンプ";
	String how2play_rule1 = U"★水鉄砲で敵を倒そう";
	String how2play_rule2 = U"★右上のライフがゼロになると\n　ゲームオーバー";
};
