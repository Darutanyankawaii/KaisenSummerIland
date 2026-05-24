#include "PlayerAnimationSet.hpp"

namespace {
	bool fileExists(const String& dirPath)
	{
		return FileSystem::Exists(U"{}/layout.csv"_fmt(dirPath));
	}
}

void PlayerAnimationSet::load()
{
	// Stand: 武器ごと (0=Water, 1=Starfish, 2=Shotgun, 3=Machine, 4=Bucket)
	// 旧コードは weapon=0,1,2 のみ存在を期待していたが、ここでは存在する全てをロード。
	for (int w = 0; w <= 4; ++w)
	{
		const String path = U"image/player/stand/{}"_fmt(w);
		if (fileExists(path))
		{
			stand_.emplace(w, Animation::loadFromDir(path, true, true));
		}
	}

	// Walk / Jump / Fall (現状は walk のみ専用、jump/fall は stand を流用するが将来差し替え可能)
	if (fileExists(U"image/player/walk"))
	{
		walk_ = Animation::loadFromDir(U"image/player/walk", true, true);
	}
	if (fileExists(U"image/player/jump"))
	{
		jump_ = Animation::loadFromDir(U"image/player/jump", true, true);
	}
	if (fileExists(U"image/player/fall"))
	{
		fall_ = Animation::loadFromDir(U"image/player/fall", true, true);
	}

	// Aiming: weapon ∈ {0, 1, 2, 3} × dir ∈ {0..6}
	for (int w : { 0, 1, 2, 3 })
	{
		for (int d = 0; d < 7; ++d)
		{
			const String path = U"image/player/aiming/{}/{}"_fmt(w, d);
			if (fileExists(path))
			{
				aiming_.emplace(w * 10 + d, Animation::loadFromDir(path, true, true));
			}
		}
	}

	currentAnimation_ = select(currentState_, currentWeapon_, currentDirIndex_);
}

int PlayerAnimationSet::directionIndex(const Vec2& aimDir)
{
	const double angle = Math::ToDegrees(std::atan2(-aimDir.y, aimDir.x));

	if (-90.0 <= angle && angle <= -22.5)      return 0;
	else if (-22.5 < angle && angle <= 22.5)   return 1;
	else if (22.5 < angle && angle <= 67.5)    return 2;
	else if (67.5 < angle && angle <= 112.5)   return 3;
	else if (112.5 < angle && angle <= 157.5)  return 4;
	else if (157.5 < angle || angle <= -157.5) return 5;
	return 6;
}

Animation* PlayerAnimationSet::select(State state, int weapon, int dirIndex)
{
	switch (state)
	{
	case State::Stand:
	{
		auto it = stand_.find(weapon);
		if (it == stand_.end()) it = stand_.find(0); // fallback
		return (it != stand_.end()) ? &it->second : nullptr;
	}
	case State::Jump:
		if (jump_.hasFrames()) return &jump_;
		// fall-through: stand を流用
		[[fallthrough]];
	case State::Fall:
		if (state == State::Fall && fall_.hasFrames()) return &fall_;
		[[fallthrough]];
	case State::Walk:
		if (state == State::Walk && walk_.hasFrames()) return &walk_;
		// fallback to stand
		{
			auto it = stand_.find(weapon);
			if (it == stand_.end()) it = stand_.find(0);
			return (it != stand_.end()) ? &it->second : nullptr;
		}
	case State::Aiming:
	{
		auto it = aiming_.find(weapon * 10 + dirIndex);
		if (it == aiming_.end()) it = aiming_.find(0 * 10 + dirIndex);
		if (it == aiming_.end()) it = aiming_.find(0);
		return (it != aiming_.end()) ? &it->second : nullptr;
	}
	}
	return nullptr;
}

void PlayerAnimationSet::tick(State state, int weapon, const Vec2& aimDir)
{
	const int dirIndex = directionIndex(aimDir);
	const bool stateChanged = (state != currentState_);
	const bool weaponChanged = (weapon != currentWeapon_);
	const bool dirChanged = (state == State::Aiming) && (dirIndex != currentDirIndex_);

	currentState_ = state;
	currentWeapon_ = weapon;
	currentDirIndex_ = dirIndex;

	if (stateChanged || weaponChanged || dirChanged || currentAnimation_ == nullptr)
	{
		currentAnimation_ = select(state, weapon, dirIndex);
		if (currentAnimation_) currentAnimation_->restart();
	}

	if (currentAnimation_) currentAnimation_->update();
}

Texture PlayerAnimationSet::currentTexture() const
{
	if (!currentAnimation_) return Texture{};
	return currentAnimation_->currentTexture();
}

bool PlayerAnimationSet::isFinished() const
{
	return currentAnimation_ && currentAnimation_->isFinished();
}
