#pragma once

#define TRACE_LENGHT  80000.f;

UENUM(BlueprintType)
enum class EWeaponType :uint8
{
	EWT_AssaultRifle	UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher	UMETA(DisplayName = "Rocket Rifle"),
	EWT_Pistol			UMETA(DisplayName = "Pistol"),
	EWT_SubMachineGun	UMETA(DisplayName = "SubMachineGun"),
	EWT_ShotGun			UMETA(DisplayName = "ShotGun"),
	EWT_SniperRifle			UMETA(DisplayName = "SniperRifle"),
	EWT_MAX UMETA(DisplayName = "Enuma Max")
};
	