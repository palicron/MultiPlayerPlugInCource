// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUps/PickUp.h"
#include "HealthPickUp.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHealthPickUp : public APickUp
{
	GENERATED_BODY()

public:
	
	AHealthPickUp();

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor,
		UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere)
	float HealAmount = 100.f;

	UPROPERTY(EditAnywhere)
	float HealingTime = 5.f;


	
	
};
