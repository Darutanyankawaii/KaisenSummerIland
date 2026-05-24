#include "StageElipse.hpp"

StageElipse::StageElipse()
{
	elipseList_.push_back(ElipseUtil(Vec2( 210, 120), Vec2( 43, 16), ElipseUtil::State::NotAttention));
	elipseList_.push_back(ElipseUtil(Vec2( 120, 210), Vec2( 85, 30), ElipseUtil::State::NotAttention));
	elipseList_.push_back(ElipseUtil(Vec2( 360, 380), Vec2( 130, 50), ElipseUtil::State::Attention));
	elipseList_.push_back(ElipseUtil(Vec2( 600, 210), Vec2( 85, 30), ElipseUtil::State::NotAttention));
	elipseList_.push_back(ElipseUtil(Vec2( 510, 120), Vec2( 43, 16), ElipseUtil::State::NotAttention));
}

StageElipse::~StageElipse()
{
}

void StageElipse::draw() const
{
	for (const auto& elipse : elipseList_)
	{
		elipse.draw();
	}

	TextureAsset(U"Thumbnail_{}"_fmt(senterArea_)).scaled(0.2).drawAt(Scene::Center() + Vec2{ 0, 100});
}

void StageElipse::readSelectedStage(const size_t startArea, const size_t endArea, const size_t senterArea)
{
	senterArea_ = senterArea + 1;
	for (auto& elipse : elipseList_)
	{
		elipse.changeState(ElipseUtil::State::None);
	}

	for (size_t i = startArea; i < endArea; ++i)
	{
		elipseList_[i].changeState(ElipseUtil::State::NotAttention);
	}

	elipseList_[2].changeState(ElipseUtil::State::Attention);

}
