#pragma once
# include "Basic.hpp"

class Object
{
public:
	Object(const Vec2& pos, const Vec2& size, const TextureRegion image) :
		pos_(pos),
		size_(size),
		image_(image)
	{
	}

	Vec2 getPos() const { return pos_; };
	Vec2 getSize() const { return size_; };
	RectF getRectF() const { return RectF{ pos_, size_ }; };
	TextureRegion getImage() const { return image_; };

private:
	Vec2 pos_;
	Vec2 size_;
	TextureRegion image_;
};
