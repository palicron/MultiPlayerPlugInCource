// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;
	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;


	/**
	 * Trace end whit Scatter
	 **/

	UPROPERTY(EditAnywhere,Category="Weapone Scatter")
	bool bUseScatter = false;
	
	UPROPERTY(EditAnywhere,Category="Weapone Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere,Category="Weapone Scatter")
	float SphereRadius = 75.f;


	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

	void WeaponTraceHit(const FVector& TraceStart,const FVector& HitTarget,FHitResult& OutHit);
	
};
