#pragma once
#include "IWeapon.hpp"
#include "Bullet.hpp"

class WeaponFactory
{
public:
	static std::unique_ptr<IWeapon> create(WeaponName name);
	static std::unique_ptr<IWeapon> create(int weaponId);
};
