// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Weapon/Projectile.h"
#include "ProjectileGrenada.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileGrenada : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileGrenada();

	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	
private:
	UPROPERTY(EditAnywhere)
	 USoundCue* BounceSound;
	
};
