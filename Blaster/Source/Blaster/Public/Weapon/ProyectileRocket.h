// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ProyectileBulled.h"
#include "ProyectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProyectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProyectileRocket();
	virtual void Destroyed() override;
protected:
	virtual void OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginPlay() override;

	



	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UAudioComponent* ProjectileLoopComponent;
	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAtten;
	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent;
	
private:




	
};
