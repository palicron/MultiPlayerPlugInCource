// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps/PickUp.h"
#include "SpeedPickUp.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ASpeedPickUp : public APickUp
{
	GENERATED_BODY()

	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:

	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff = 1600.f;
	
	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 850.f;
	
	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 30.f;
	
	
};
