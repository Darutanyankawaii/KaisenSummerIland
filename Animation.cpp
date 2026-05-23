#include "Animation.hpp"

Animation::Animation(const Array<Texture>& frames, const Array<float>& layouts,
	bool isLoop, bool isAwake, int index) :
	frames(frames), layouts(layouts), isLoop(isLoop), index(index) {
	init(index, isAwake);
}

Animation::Animation(const String& dirPath, bool isLoop, bool isAwake, int index) :
	isLoop(isLoop), index(index) {
	load(dirPath);
	init(index, isAwake);
}

void Animation::update() {
	// 現在のフレームが終了したとき
	if (timer.isStarted() && timer.reachedZero()) {
		// 次のフレームへ移動
		index++;

		// 最後のフレームに到達したとき
		if (index >= frames.size()) {
			if (isLoop) {
				index = 0;
			}
			else {
				index--;			// 最終フレームで止める
				_isFinish = true;
				return;
			}
		}

		// 次のフレームのTimerを作成
		timer = Timer(Duration(layouts[index]), StartImmediately::Yes);
	}
}

Texture Animation::getTexture() const {
	return frames[index];
}

bool Animation::isFinish() const {
	return _isFinish;
}

void Animation::init(int index, bool isAwake) {
	index = index;

	if (isAwake) {
		timer = Timer(Duration(layouts[index]), StartImmediately::Yes);
	}
}

void Animation::load(const String& dirPath) {
	String path = U"{}/layout.csv"_fmt(dirPath);
	const CSV csv(path);

	if (not csv)
		throw Error(U"Failed to load {}"_fmt(path));

	for (int i = 0; i < csv.columns(0); i++) {
		layouts.push_back(Parse<float>(csv[0][i]));
	}

	for (int i = 0; i < layouts.size(); i++) {
		frames.push_back(Texture(U"{}/{}.png"_fmt(dirPath, i)));
	}
}

StandAnimation::StandAnimation(int& weapon_, const String& dirPath, bool isLoop, bool isAwake, int index) :
	weapon_(weapon_), isLoop(isLoop) {
	load(dirPath);
	init(index, isAwake);
}

void StandAnimation::update() {
	animations[weapon_]->update();
}

void StandAnimation::load(const String& dirPath) {
	for (int i = 0; i < 3; i++) {
		String path = U"{}/{}"_fmt(dirPath, i);
		animations.push_back(new Animation(path));
	}
}


AimingAnimation::AimingAnimation(Vec2& playerDir, int& weapon_, const String& dirPath, bool isLoo, bool isAwake, int index) :
	playerDir(playerDir), weapon_(weapon_) {
	load(dirPath);
	init(index, isAwake);
}

void AimingAnimation::load(const String& dirPath) {
	// [Todo] ２重ループで読み込む
	// 0: 水鉄砲のアニメーションを読み込む
	for (int i = 0; i < 7; i++) {
		String path = U"{}/{}/{}"_fmt(dirPath, 0, i);
		int key = 0 * 10 + i;
		animations.emplace(key, new Animation(path));
	}
	// 2: 散弾のアニメーションを読み込む
	for (int i = 0; i < 7; i++) {
		String path = U"{}/{}/{}"_fmt(dirPath, 2, i);
		int key = 2 * 10 + i;
		animations.emplace(key, new Animation(path));
	}
	// 3: 連射のアニメーションを読み込む
	for (int i = 0; i < 7; i++) {
		String path = U"{}/{}/{}"_fmt(dirPath, 3, i);
		int key = 3 * 10 + i;
		animations.emplace(key, new Animation(path));
	}
}

void AimingAnimation::update() {
	// 方向によるAngleの更新
	float angle = Math::ToDegrees(std::atan2(-playerDir.y, playerDir.x));
#ifdef DEBUGGING
	Print << U"angle: " << angle;
#endif

	if (-90.0 <= angle && angle <= -22.5f)
		dirIndex = 0;
	else if (-22.5f < angle && angle <= 22.5f)
		dirIndex = 1;
	else if (22.5f < angle && angle <= 67.5f)
		dirIndex = 2;
	else if (67.5f < angle && angle <= 112.5f)
		dirIndex = 3;
	else if (112.5f < angle && angle <= 157.5f)
		dirIndex = 4;
	else if (157.5f < angle || angle <= -157.5)
		dirIndex = 5;
	else
		dirIndex = 6;

	animations[weapon_ * 10 + dirIndex]->update();
}
