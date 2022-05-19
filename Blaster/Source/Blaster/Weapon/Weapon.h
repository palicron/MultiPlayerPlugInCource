// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Drop UMETA(DisplayName="Drop"),
	EWS_MAX UMETA(DisplayName="DefaultMax")
};
UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon();
	
	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	USphereComponent* AreaSphere;

	


	

};
