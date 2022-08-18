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
	UFUNCTION()
	void DestroyTimerFinish();
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rocket Config")
	float ExploitationInnerRadius = 200.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rocket Config")
	float ExploitationOuterRadius = 500.f;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;
	UPROPERTY(EditAnywhere)
	float DestroyTime =3.f;
	UPROPERTY()
	class UNiagaraComponent* TrailSystemcomp;
	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop;

	UAudioComponent* ProjectileLoopComponent;
	UPROPERTY(EditAnywhere)
	USoundAttenuation* LoopingSoundAtten;
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTimer;

	
};
