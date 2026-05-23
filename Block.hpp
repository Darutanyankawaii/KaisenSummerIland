#pragma once
# include "Basic.hpp"

class Block
{
public:
	Block(const Vec2& pos, const Vec2& size, const TextureRegion image, int32 function) :
		pos_(pos),
		size_(size),
		function_(function),
		image_(image)
	{
	}

	int32 getFunction() const { return function_; };
	Vec2 getPos() const { return pos_; };
	Vec2 getSize() const { return size_; };
	RectF getRectF() const { return RectF{ pos_, size_ }; };
	TextureRegion getImage() const { return image_; };

private:
	Vec2 pos_;
	Vec2 size_;
	int32 function_;
	TextureRegion image_;
};
