#pragma once
#include "Basic.hpp"

// 単一のアニメーション (フレーム列 + ループ可否)。
// 旧 IAnimation 階層は廃止。武器別・方向別の選択は PlayerAnimationSet に分離した。
class Animation
{
public:
	Animation() = default;

	Animation(const Array<Texture>& frames, const Array<float>& durations,
		bool loop = false, bool startNow = false, int startIndex = 0);

	// ディレクトリ (layout.csv + 連番 png) からロード
	static Animation loadFromDir(const String& dirPath, bool loop = false, bool startNow = false);

	void update();
	void start();
	void restart();

	Texture currentTexture() const;
	bool isFinished() const { return finished_; }
	int frameIndex() const { return index_; }
	bool hasFrames() const { return !frames_.isEmpty(); }

private:
	Array<Texture> frames_;
	Array<float> durations_;
	int index_ = 0;
	bool loop_ = false;
	bool finished_ = false;
	Timer timer_;

	void load(const String& dirPath);
};
