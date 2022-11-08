// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps/PickUp.h"
#include "ShieldPickUp.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AShieldPickUp : public APickUp
{
	GENERATED_BODY()

public:
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor,
		UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


	UPROPERTY(EditAnywhere)
	float ShieldAmount = 100.f;

	UPROPERTY(EditAnywhere)
	float ShieldTime = 5.f;
	
};
