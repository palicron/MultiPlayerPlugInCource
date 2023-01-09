#pragma once

UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccupied"),
	ECS_Reloading UMETA(DisplayName="Reloading"),
	ECS_ThrowingGrenade UMETA(DisplayName="Throwing Grenede"),
	ECS_SwappingWeapones UMETA(DisplayName="Sawpping Weapons"),
	
	ECS_Max UMETA(DisplayName="Default Max")
};