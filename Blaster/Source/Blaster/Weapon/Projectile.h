// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"
class UBoxComponent;
UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectile();
	
	virtual void Tick(float DeltaTime) override;

	/**
	 * Server sdie rewind
	 */

	bool bUseServerSideRewind = false;

	FVector_NetQuantize TraceStart;

	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000;
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void Destroyed() override;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemcomp;

	FTimerHandle DestroyTimer;
	
	UPROPERTY(EditAnywhere)
	float DestroyTime =3.f;

	UFUNCTION()
	void SpawnTrailSystem();

	UFUNCTION()
	void ExplodeDamage();
	

	void StartDestroyTimer();
	UFUNCTION()
	void DestroyTimerFinish();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rocket Config")
	float ExploitationInnerRadius = 200.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rocket Config")
	float ExploitationOuterRadius = 500.f;

	
private:
	
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent* TraceComponent;


public:	
   


};
