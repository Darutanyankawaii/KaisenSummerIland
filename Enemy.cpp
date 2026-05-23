#include "Enemy.hpp"
#include "Game.hpp"

Animation Enemy::loadAnimation(String path_dir) {
	String file = U"image/" + path_dir;
	const CSV csv(file + U".csv");
	if (not csv)throw Error(U"Failed to load {}.csv"_fmt(file));
	Array<float> layout;
	for (size_t i = 0; i < csv.columns(0); ++i)
	{
		layout.push_back(Parse<float>(csv[0][i]));
	}
	Array<Texture> frames;
	for (size_t i = 0; i < layout.size(); ++i) {
		frames.push_back(Texture(file + U"_" + Format(i) + U".png"));
	}
	return Animation(frames, layout, loop);
}

Enemy::Enemy(const Vec2& pos)
	: pos_(pos)
{
}

Enemy::Enemy(const Vec2& pos, const Vec2 size, const int hp, const int id, const bool loop)
	: pos_(pos), SIZE(size), hp_(hp), ID(id), loop(loop)
{
}


void Enemy::update()
{

}

void Enemy::moveX() {
	if (dir) {
		speed_.x = WALK_SPEED;
	}
	else {
		speed_.x = -1 * WALK_SPEED;
	}
	pos_.x += speed_.x;
}

void Enemy::moveY() {
	accel_.y = 0;
	accel_.y += gravity;
	speed_.y += accel_.y;
	pos_.y += speed_.y;
}

void Enemy::getArea(Array<Vec2>& bossArea_)
{
	//ここでプレイヤーから取得したい情報を定義する。
	bossArea_ = bossArea;
}

Curage::Curage(const Vec2& _pos)
	:Enemy(_pos)
{
	animation = loadAnimation(U"jellyfish/jellyfish");
	animation.start();
	this->accel_.y += this->gravity;
}

void Curage::update()
{
	animation.update();
}

void Curage::draw() const {
	animation.getTexture().draw(pos_);
}

Kani::Kani(const Vec2& _pos)
	:Enemy(_pos)
{
	animation = loadAnimation(U"crab/crab");
	animation.start();
	this->accel_.y += this->gravity;
	this->SIZE = Vec2(80, 80);
}

void Kani::update()
{
	animation.update();
}

void Kani::moveX() {
	dir = playerDir_ > 0 ? true : false;

	Enemy::moveX();
}

void Kani::draw() const {
	animation.getTexture().draw(pos_);
#ifdef DEBUGGING
	RectF(pos, size).drawFrame(1.0, Palette::Green);
#endif
}

Tako::Tako(const Vec2& _pos)
	:Enemy(_pos)
{
	accumulator = 0.0;
	flag_ = true;
}

void Tako::update()
{
	//ここに５秒に一回動作する処理を書く
	accumulator_iPi += Scene::DeltaTime();
	if (accumulator_iPi > spawnTime_iPi)
	{
		bullets.push_back(std::make_unique<BGas>(pos_, Vec2(-100, pos_.y)));
		accumulator_iPi = 0;
	}
	for (auto& bullet : bullets)
	{
		Vec2{ -Scene::DeltaTime() * 100 ,0 };
		bullet->additionPos(Vec2{ -Scene::DeltaTime() * 100 ,0 });
		bullet->LifeSpan -= Scene::DeltaTime();
		/*if (playerRect_.intersects(bullet->getCircle()))
		{
			player_.receiveDamage(1);
			bullet->hit = true;
		}*/
	}
	bullets.remove_if([](const std::unique_ptr<Bullet>& b) { return b->LifeSpan < 0 || b->hit; });
}

void Tako::moveX()
{
	accumulator += Scene::DeltaTime();

	if (accumulator > spawnTime)
	{
		accumulator = 0.0;

		if (flag_)
		{
			//パターン１
			this->speed_.x = 160;
			this->speed_.y = -4;
			flag_ = false;
			spawnTime = 0.8;
		}
		else
		{
			//パターン２
			flag_ = true;
			spawnTime = 3.0;
		}
	}
	this->speed_.x *= 0.98;
	this->speed_.y -= Scene::DeltaTime() * 3.9;
	this->pos_ += this->speed_ * Scene::DeltaTime();
}

void Tako::draw() const
{
	for (const auto& bullet : bullets)
	{
		TextureAsset(U"bullet").drawAt(bullet->getPos());
	}
	if (!flag_)TextureAsset(U"OCTOPUS_1").draw(pos_);
	else TextureAsset(U"OCTOPUS_2").draw(pos_);
}

Maguro::Maguro(const Vec2& _pos)
	:Enemy(_pos, Vec2(80, 80), BOSS_HP, BOSS_ID, false)
{
	goPos = _pos;
	hp_ = BOSS_HP;//何故かここでHP設定しないとHPが反映されない

	for (int i = 0; i < IMAGE_NUM; i++) {
		animation << loadAnimation(U"boss/Act{}/boss"_fmt(i + 1));
		animation[i].start();
	}
}

void Maguro::update()
{
	//球の移動
	for (auto& bullet : bullets)
	{
		//bullets.pos += bullets[i]->dir_ * 5;
		bullet->additionPos(bullet->getDir() * 5);
	}
}

void Maguro::draw() const {
	if (this->dir) {
		if (!animeFlag) {
			TextureAsset(U"MAGURO_1").draw(pos_);
		}
		else
		{
			animation[animeDir].getTexture().draw(pos_);
		}
	}
	else {
		if (!animeFlag) {
			TextureAsset(U"MAGURO_2").draw(pos_);
		}
		else {
			animation[animeDir].getTexture().draw(pos_);
		}
	}

	for (const auto& bullet : bullets)
	{
		TextureAsset(U"bullet2").drawAt(bullet->getPos());
	}
}

void Maguro::moveX()
{

	//時間計測（ボスが行動していない場合）
	if (this->endedAct)
	{
		accumulator += Scene::DeltaTime();
	}

	//攻撃機能管理
	if (accumulator > spawnTime)
	{
		accumulator = 0.0;

		switch (Random(2))
		{
		case 0:
			//突進攻撃の初期化
			moveAttack();
			rushFlag = true;
			animeFlag = true;
			animeDir = dir ? 0 : 1; //方向決定
			gravity = 0;
			endedAct = false;
			break;
		case 1:
			//拡散弾攻撃の初期化
			spreadAttack();
			bulletFlag = true;
			endedAct = false;
			break;
		default:
			break;
		}
	}

	//突進攻撃
	if (rushFlag)
	{
		//機能が開始されてからの時間を計測
		if (animeFlag)
		{
			rushAccumulator += Scene::DeltaTime();
		}

		animation[animeDir].update();

		//攻撃動作開始
		if (rushAccumulator > 1)
		{
			rushAccumulator = 0;
			this->speed_ = setDir(pos_, this->goPos) * 12;
			this->hitbox = false;
			animeFlag = false;
			animeEnd = false;
		}

		//突進攻撃の終了判定
		if (Circle{ this->goPos + this->SIZE / 2, 10 }.intersects(Circle{ pos_ + this->SIZE / 2, 5 }))
		{
			pos_.y -= 5.0;
			rushFlag = false;
			this->hitbox = true;
			this->endedAct = true;
			this->gravity = 0.1;
			this->speed_ = { 0,0 };
			animation[animeDir].init();
		}
	}

	//拡散弾攻撃
	if (bulletFlag)
	{
		//玉が発射されてからの時間を計測
		diffusionAccumulator += Scene::DeltaTime();

		//玉の削除
		if (diffusionAccumulator > SPAWN_DIFFUSION)
		{
			for (auto it = bullets.begin(); it != bullets.end();)
			{
				it = bullets.erase(it);
			}
			diffusionAccumulator = 0;
			bulletFlag = false;
			endedAct = true;
		}
	}

	dir = playerPos_.x < pos_.x ? true : false;
	pos_.x += speed_.x;
}

void Maguro::moveY() {
	this->accel_.y = 0;
	this->accel_.y += this->gravity;
	this->speed_.y += this->accel_.y;
	this->pos_.y += this->speed_.y;
}

void Maguro::moveAttack() {
	int _num;

	do {
		_num = Random(bossArea.size() - 1);
	} while (num == _num);

	num = _num;

	this->goPos = bossArea[num];
}

void Maguro::spreadAttack() {
	Vec2 startPos = pos_ + SIZE / 2;

	bullets.push_back(std::make_unique<Bullet>(startPos, setAngle(startPos, 15, false)));
	bullets.push_back(std::make_unique<Bullet>(startPos, setAngle(startPos, -15, false)));
	bullets.push_back(std::make_unique<Bullet>(startPos, setAngle(startPos, 0, false)));
	bullets.push_back(std::make_unique<Bullet>(startPos, setAngle(startPos, 15, true)));
	bullets.push_back(std::make_unique<Bullet>(startPos, setAngle(startPos, -15, true)));
	bullets.push_back(std::make_unique<Bullet>(startPos, setAngle(startPos, 0, true)));
}

Vec2 Maguro::setDir(const Vec2& _pos, const Vec2& _cur) {
	// 三角関数を利用して進行方向を調整
	int x = (_cur.x > _pos.x ? 1 : -1);
	int y = (_cur.y > _pos.y ? 1 : -1);

	double tryZ = sqrt(std::pow(Abs(_cur.y - _pos.y), 2.0) + std::pow(Abs(_cur.x - _pos.x), 2.0));
	double tryX = x * sqrt(std::pow(Abs(_cur.x - _pos.x), 2.0)) / tryZ;
	double tryY = y * sqrt(std::pow(Abs(_cur.y - _pos.y), 2.0)) / tryZ;


	return Vec2{ tryX, tryY };
}

Vec2 Maguro::setAngle(const Vec2& _pos, const double _angle, const bool _dir) {
	//１，中心の座標、２，角度、３，向き
	//中心の座標を中心に向きがTRUEなら右向き、FALSEなら左向きに
	//角度をとって玉を発射する
	double angle;

	if (_angle == 0) {
		if (_dir) {
			return _pos - Vec2{ -1, 0 };
		}
		else {
			return _pos - Vec2{ 1, 0 };
		}
	}

	if (_angle == 90) {
		return _pos - Vec2{ 0, -1 };
	}

	if (_angle == -90) {
		return _pos - Vec2{ 0, 1 };
	}

	if (_dir) {
		angle = (90 - _angle) * 1_deg;
	}
	else {
		angle = (_angle - 90) * 1_deg;
	}


	return OffsetCircular(_pos, 1, angle);
}
