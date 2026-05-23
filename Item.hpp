#pragma once
#include "Basic.hpp"

class Player;

// アイテム番号 (0 オリジン、マップデータに対応)
enum class ItemName
{
	Shaved_Ice,  // 0
	Water_Gun,   // 1
	Starfish,    // 2
	Shotgun,     // 3
	Machine_Gun, // 4
	Bucket,      // 5
};

class Item
{
public:
	explicit Item(const Vec2& region);
	virtual ~Item() = default;

	const RectF& get_rect() const;

	virtual void get_item(std::unique_ptr<Player>& player) const = 0;
	virtual void draw() const = 0;

private:
	RectF rect_;
};

class Shaved_Ice : public Item
{
public:
	explicit Shaved_Ice(const Vec2& region);
	void get_item(std::unique_ptr<Player>& player) const override;
	void draw() const override;
};

class Water_Gun : public Item
{
public:
	explicit Water_Gun(const Vec2& region);
	void get_item(std::unique_ptr<Player>& player) const override;
	void draw() const override;
};

class Starfish : public Item
{
public:
	explicit Starfish(const Vec2& region);
	void get_item(std::unique_ptr<Player>& player) const override;
	void draw() const override;
};

class Shotgun : public Item
{
public:
	explicit Shotgun(const Vec2& region);
	void get_item(std::unique_ptr<Player>& player) const override;
	void draw() const override;
};

class Machine_Gun : public Item
{
public:
	explicit Machine_Gun(const Vec2& region);
	void get_item(std::unique_ptr<Player>& player) const override;
	void draw() const override;
};

class Bucket : public Item
{
public:
	explicit Bucket(const Vec2& region);
	void get_item(std::unique_ptr<Player>& player) const override;
	void draw() const override;
};
