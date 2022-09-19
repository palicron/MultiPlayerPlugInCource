// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps/PickUp.h"
#include "Weapon/WeaponTypes.h"
#include "AmmoPickUp.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AAmmoPickUp : public APickUp
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent*
		otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount= 32;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	
	
};
