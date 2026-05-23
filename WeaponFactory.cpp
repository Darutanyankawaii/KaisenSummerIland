#include "WeaponFactory.hpp"
#include "WaterGunWeapon.hpp"
#include "StarfishWeapon.hpp"
#include "ShotgunWeapon.hpp"
#include "MachineGunWeapon.hpp"
#include "BucketWeapon.hpp"

std::unique_ptr<IWeapon> WeaponFactory::create(WeaponName name)
{
	switch (name)
	{
	case WeaponName::Water_Gun:   return std::make_unique<WaterGunWeapon>();
	case WeaponName::Starfish:    return std::make_unique<StarfishWeapon>();
	case WeaponName::Shotgun:     return std::make_unique<ShotgunWeapon>();
	case WeaponName::Machine_Gun: return std::make_unique<MachineGunWeapon>();
	case WeaponName::Bucket:      return std::make_unique<BucketWeapon>();
	}
	return std::make_unique<WaterGunWeapon>();
}

std::unique_ptr<IWeapon> WeaponFactory::create(int weaponId)
{
	return create(static_cast<WeaponName>(weaponId));
}
