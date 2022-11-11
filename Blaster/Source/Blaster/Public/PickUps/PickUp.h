// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

UCLASS()
class BLASTER_API APickUp : public AActor
{
	GENERATED_BODY()
	
public:	

	APickUp();

	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* OverLapSphere;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere)
	class USoundCue* PickUpSound;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickUpMesh;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* PickUPEffectComponent;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickUpEffect;

	FTimerHandle BindOvelapTimer;

	float BindOverlapTime = 0.25f;

	void BindOverLapTimerFinish();

public:

	


};
