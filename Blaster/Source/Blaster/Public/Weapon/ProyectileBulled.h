// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Weapon/Projectile.h"
#include "ProyectileBulled.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProyectileBulled : public AProjectile
{
	GENERATED_BODY()

public:
	AProyectileBulled();
protected:
	
	virtual void OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
