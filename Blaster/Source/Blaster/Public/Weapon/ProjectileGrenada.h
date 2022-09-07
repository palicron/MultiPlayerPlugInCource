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


protected:
	virtual void BeginPlay() override;
	
};
