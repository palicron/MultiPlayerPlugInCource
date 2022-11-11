// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpSpawnPoint.generated.h"

UCLASS()
class BLASTER_API APickUpSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APickUpSpawnPoint();

	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickUp>> PickUpClasses;

	FTimerHandle SpawnPickUpTimer;

	UPROPERTY(EditAnywhere)
	float SpawnPickUPTimeMin = 0.f;

	UPROPERTY(EditAnywhere)
	float SpawnPickUPTimeMax = 10.f;

	UPROPERTY()
	APickUp* SpawnPickUp;
	
	virtual void BeginPlay() override;

	void SpawnPickup();
	
	void SpawnPickUpTimerFInish();

	UFUNCTION()
	void StartPickUPTimer(AActor* Actor = nullptr);

public:	



};
