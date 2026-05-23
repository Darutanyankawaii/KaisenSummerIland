#include "Player.hpp"
#include "Bullet.hpp"

Player::Player()
	: isGround_(false), speed_{ SET_SPPED }, accel_{ SET_ACCEL }
{
	initAnimations();
	attackDir_ = (Cursor::Pos() - pos_).normalized();
}

Player::Player(const Vec2 pos)
	: isGround_(false), speed_{ SET_SPPED }, accel_{ SET_ACCEL }
{
	setPos(pos);
	initAnimations();
	attackDir_ = (Cursor::Pos() - pos_).normalized();
}

Player::~Player()
{
	AudioAsset(U"walk").stop();
}

void Player::updateX() {

	if (shotNow)
	{
		shotTime += Scene::DeltaTime();
		if (shotTime > 0.5)
		{
			shotTime = 0;
			shotNow = false;
			state = State::stand;
		}
		else
		{
			state = State::aiming;
		}
	}

	if (state == State::aiming) {
		if ((KeyD | KeyRight | KeyA | KeyLeft).down())
			state = State::walk;
	}
	else {
		if (isGround_)
			state = State::stand;

		//Dか＞ボタンで右に移動
		if ((KeyD | KeyRight).pressed() && !aimFlag) {
			playerDir_ = 1;
			speed_.x = playerDir_ * WALK_SPEED;
			state = State::walk;
		}
		//Aか＜ボタンで右に移動
		else if ((KeyA | KeyLeft).pressed() && !aimFlag) {
			playerDir_ = -1;
			speed_.x = playerDir_ * WALK_SPEED;
			state = State::walk;
		}
		else {
			speed_.x = 0.0f;
		}
	}

	//ノックバック処理
	//isKnockbackがfalseなら通常のキー移動の移動が発生
	//isKnockbackがtrueならノックバックが発生　＆　通常のキー移動が不可能に
	if (isKnockback == true) {
		//敵にぶつかった際、プレイヤーの移動ができなくなり、ノックバックする
		pos_.x += knockBackDir * 5;
		if (collisionalTimer.reachedZero() == true) {
			isKnockback = false;
		}
	}
	else {
		// x方向の移動
		pos_.x += speed_.x;
	}

	//無敵時間が切れた時、無敵のフラグを下げる。
	if (invincibleTimer.reachedZero() == true) {
		isInvincible = false;
	}
}

void Player::updateY() {
	accel_.y = gravity;		//落下スピードの初期化（これによって重力が加算される）

	// ジャンプ処理
	if (KeySpace.down() && isGround_) {
		accel_.y -= 18.0f;
		state = State::jump;
	}

	// y方向の移動
	speed_.y += accel_.y;
	pos_.y += speed_.y;

	// 地面にいないとき
	if (!isGround_ && state != State::aiming) {
		state = State::fall;
	}
}

void Player::lastUpdate(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_) {
	// 攻撃処理
	attack(camera, playerBullets_);

	//if (MouseR.up()) {
	//	state = State::stand;
	//}

	// アニメーションの更新
	if (prevState != state) {
		animation = animations.at(state);
		animation->start();
	}
	animation->update();

	// サウンド処理
	playSound();

	// 状態の更新
	prevState = state;
}

void Player::draw() const {

	if (state == State::aiming) {
		animation->getTexture().draw(pos_);
	}
	else {
		if (playerDir_ == 1)
			animation->getTexture().draw(pos_);
		else if (playerDir_ == -1)
			animation->getTexture().mirrored().draw(pos_);
	}
#ifdef DEBUGGING
	if (state == State::aiming) {
		animation->getTexture().draw(pos).drawFrame(1.0, Palette::Green);
	}
	else {
		if (direction == 1)
			animation->getTexture().draw(pos).drawFrame(1.0, Palette::Green);
		else if (direction == -1)
			animation->getTexture().mirrored().draw(pos).drawFrame(1.0, Palette::Green);
	}
	Print << U"state: {}"_fmt(stateToString(state));
	Print << U"anim index: {}"_fmt(animation->getIndex());
#endif
}

void Player::receiveDamage(int damage)
{
	hp_ -= damage;
}

void Player::knockBackToEnemy(Array<std::unique_ptr<Enemy>>& enemies)
{
	for (auto& enemy : enemies)
	{
		if (!isInvincible && this->getRectF().intersects(enemy->getRectF()))
		{
			collisionalTimer.restart();
			invincibleTimer.restart();
			receiveDamage(1);
			// ノックバックと無敵時間のフラグを立てる
			isKnockback = true;
			isInvincible = true;
			if (pos_.x < enemy->pos_.x)
			{
				// 左に移動
				knockBackDir = -1;
			}
			else
			{
				// 右に移動
				knockBackDir = 1;
			}
		}
	}
}

void Player::recoverDamage(int damage)
{
	hp_ += damage;
}

void Player::initAnimations() {
	animations[State::stand] = loadAnimation(State::stand, true);
	animations[State::walk] = loadAnimation(State::walk, true);

	// Todo: 素材が完成したら正しいものを読み込む
	animations[State::jump] = loadAnimation(State::stand, true);
	animations[State::fall] = loadAnimation(State::stand, true);
	animations[State::aiming] = loadAnimation(State::aiming, true);

	// 最初のアニメーションを設定
	animation = animations[state];
	animation->start();
}

IAnimation* Player::loadAnimation(State state, bool isLoop) {
	String stateName = stateToString(state);
	String dirPath = U"image/player/{}"_fmt(stateName);

	if (state == State::aiming)
		return (IAnimation*)(new AimingAnimation(attackDir_, weapon_, dirPath, isLoop));
	else if (state == State::stand)
		return (IAnimation*)(new StandAnimation(weapon_, dirPath, isLoop));

	return (IAnimation*)(new Animation(dirPath, isLoop));
}

String Player::stateToString(State state) const {
	switch (state) {
	case State::stand:
		return U"stand";
	case State::walk:
		return U"walk";
	case State::jump:
		return U"jump";
	case State::fall:
		return U"fall";
	case State::aiming:
		return U"aiming";
	}
	// 通常時
	return U"stand";
}

void Player::playSound() {
	// State遷移があったとき
	if (prevState != state) {
		// 前の状態
		switch (prevState) {
		case State::walk:
			AudioAsset(U"walk").stop();
		}
		// 新しい状態
		switch (state) {
		case State::walk:
			if (isGround_) {
				AudioAsset(U"walk").setLoop(true);
				AudioAsset(U"walk").play();
			}
			break;
		}
	}
}

void Player::attack(const CustomCamera2D& camera, Array<std::unique_ptr<Bullet>>& playerBullets_) {
	const auto t = camera.createTransformer();
	Vec2 startPos = pos_ + Vec2{ 32, 32 };
	Vec2 dir = Cursor::Pos();
	aimFlag = false;

	//攻撃
	if (cooltime <= 0.0 && MouseL.down()) {
		//エイムしながらだと打ちにくいので一度停止
		//if (state == State::aiming && cooltime <= 0.0 && MouseL.down())
		speed_.x = 0;
		aimFlag = true;
		//ロック機能復活した場合削除
		attackDir_ = (Cursor::Pos() - getCenter()).normalize();
		state = State::aiming;

		shotNow = true;

		//単発攻撃
		switch (weapon_)
		{
		case static_cast<int>(WeaponName::Water_Gun):
			playerBullets_.push_back(std::make_unique<BWater_Gun>(startPos, dir));
			cooltime += 0.5;
			goto default_case;
		case static_cast<int>(WeaponName::Starfish):
			playerBullets_.push_back(std::make_unique<BStarfish_Gun>(startPos, dir));
			cooltime += 1.0;
			goto default_case;
		case static_cast<int>(WeaponName::Shotgun):
		{
			Vec2 target = dir - startPos;
			//角度の散らばり
			constexpr double spread_theta = 10 * Math::Pi / 180;
			//速度の散らばり
			constexpr double spread_speed = 0.1;
			auto f = [&](Vec2 v)
				{
					double theta = Random(-spread_theta, spread_theta);
					double speed_ = Random(1 - spread_speed, 1 + spread_speed);
					return Vec2(v.x * Cos(theta) - v.y * Sin(theta), v.x * Sin(theta) + v.y * Cos(theta)) * speed_;
				};
			for (size_t i = 0; i < 3; ++i)
			{
				playerBullets_.push_back(std::make_unique<BShot_Gun>(startPos, f(target) + startPos));
			}
			cooltime += 0.75;
			goto default_case;
		}
		case static_cast<int>(WeaponName::Bucket):
		{
			Vec2 target = dir - startPos;
			//角度の散らばり
			constexpr double spread_theta = 50 * Math::Pi / 180;
			//速度の散らばり
			constexpr double spread_speed = 0.1;
			auto f = [&](Vec2 v)
				{
					double theta = Random(-spread_theta, spread_theta);
					double speed_ = Random(1 - spread_speed, 1 + spread_speed);
					return Vec2(v.x * Cos(theta) - v.y * Sin(theta), v.x * Sin(theta) + v.y * Cos(theta)) * speed_;
				};
			for (size_t i = 0; i < 5; ++i)
			{
				playerBullets_.push_back(std::make_unique<BBucket_Gun>(startPos, f(target) + startPos));
			}
			cooltime += 1.0;
			goto default_case;
		}
	default_case:
		AudioAsset(U"shot").playOneShot();
		}
	}
	else if (state == State::aiming && cooltime <= 0 && MouseL.pressed())
	{
		shotNow = true;
		//連射攻撃
		switch (weapon_)
		{
		case static_cast<int>(WeaponName::Machine_Gun):
			playerBullets_.push_back(std::make_unique<BMachine_Gun>(startPos, dir));
			cooltime += 0.2;
			goto default_case2;
		default_case2:
			AudioAsset(U"shot").playOneShot();
		}
	}
	else
	{
		//クールタイム減少
		cooltime -= Scene::DeltaTime();
		if (cooltime < 0.0) cooltime = 0.0;
	}
}
