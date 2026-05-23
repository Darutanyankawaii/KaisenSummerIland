#pragma once
# include "Basic.hpp"

//アイテム番号(0オリジン、マップデータに対応)
enum class ItemName
{
	Shaved_Ice,  //0
	Water_Gun,   //1
	Starfish,    //2
	Shotgun,     //3
	Machine_Gun, //4
	Bucket,
};

//アイテムクラス、獲得時に効果をもたらすもの
class Item
{
public:
	Item(const Vec2& region_);
	virtual ~Item();
	//rect（アイテムの座標、サイズ）を返す
	const RectF& get_rect() const;
	//このアイテムを獲得した時の処理
	virtual void get_item(std::unique_ptr<Player>& player_) const = 0;
	//アイテムの描画
	virtual void draw() const = 0;
private:
	//アイテムの座標、サイズ
	RectF rect;
};

//かき氷、獲得時プレイヤーのHP回復
class Shaved_Ice : public Item
{
public:
	Shaved_Ice(const Vec2& region_);
	~Shaved_Ice();
	//獲得時HP回復
	void get_item(std::unique_ptr<Player>& player_) const override;
	void draw() const override;
};

//以下は武器、獲得時プレイヤーの武器を変更
//水鉄砲
class Water_Gun : public Item
{
public:
	Water_Gun(const Vec2& region_);
	~Water_Gun();
	void get_item(std::unique_ptr<Player>& player_) const override;
	void draw() const override;
};
//ヒトデ
class Starfish : public Item
{
public:
	Starfish(const Vec2& region_);
	~Starfish();
	void get_item(std::unique_ptr<Player>& player_) const override;
	void draw() const override;
};
//散弾銃
class Shotgun : public Item
{
public:
	Shotgun(const Vec2& region_);
	~Shotgun();
	void get_item(std::unique_ptr<Player>& player_) const override;
	void draw() const override;
};
//機関銃
class Machine_Gun : public Item
{
public:
	Machine_Gun(const Vec2& region_);
	~Machine_Gun();
	void get_item(std::unique_ptr<Player>& player_) const override;
	void draw() const override;
};
//バケツ
class Bucket : public Item
{
public:
	Bucket(const Vec2& region_);
	~Bucket();
	void get_item(std::unique_ptr<Player>& player_) const override;
	void draw() const override;
};
