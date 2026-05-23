#include "Animation.hpp"

Animation::Animation(const Array<Texture>& frames, const Array<float>& durations,
	bool loop, bool startNow, int startIndex)
	: frames_(frames), durations_(durations), index_(startIndex), loop_(loop)
{
	if (frames_.size() != durations_.size())
	{
		throw Error(U"Animation: frames({}) != durations({})"_fmt(
			frames_.size(), durations_.size()));
	}
	if (startNow) start();
}

Animation Animation::loadFromDir(const String& dirPath, bool loop, bool startNow)
{
	Animation a;
	a.load(dirPath);
	a.loop_ = loop;
	if (startNow) a.start();
	return a;
}

void Animation::load(const String& dirPath)
{
	const String path = U"{}/layout.csv"_fmt(dirPath);
	const CSV csv(path);
	if (not csv)
	{
		throw Error(U"Failed to load {}"_fmt(path));
	}

	for (size_t i = 0; i < csv.columns(0); ++i)
	{
		durations_.push_back(Parse<float>(csv[0][i]));
	}
	for (size_t i = 0; i < durations_.size(); ++i)
	{
		frames_.push_back(Texture(U"{}/{}.png"_fmt(dirPath, i)));
	}

	if (frames_.size() != durations_.size())
	{
		throw Error(U"Animation '{}': frames({}) != durations({})"_fmt(
			dirPath, frames_.size(), durations_.size()));
	}
}

void Animation::start()
{
	if (durations_.isEmpty()) return;
	const int safe = Clamp(index_, 0, static_cast<int>(durations_.size()) - 1);
	timer_ = Timer(Duration(durations_[safe]), StartImmediately::Yes);
}

void Animation::restart()
{
	index_ = 0;
	finished_ = false;
	start();
}

void Animation::update()
{
	if (!timer_.isStarted() || !timer_.reachedZero()) return;

	++index_;

	if (index_ >= static_cast<int>(frames_.size()))
	{
		if (loop_)
		{
			index_ = 0;
		}
		else
		{
			--index_;
			finished_ = true;
			return;
		}
	}

	if (!durations_.isEmpty())
	{
		timer_ = Timer(Duration(durations_[index_]), StartImmediately::Yes);
	}
}

Texture Animation::currentTexture() const
{
	if (frames_.isEmpty()) return Texture{};
	const int safe = Clamp(index_, 0, static_cast<int>(frames_.size()) - 1);
	return frames_[safe];
}
