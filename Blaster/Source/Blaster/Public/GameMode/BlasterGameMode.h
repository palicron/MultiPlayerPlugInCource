// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PlayerEliminated(class ABlasterCharacter* ElimCharacter, class ABlasterPlayerController*  VictimController,ABlasterPlayerController* AttackerController);

	virtual void RequestRespawn(ACharacter* ElimCharacter,AController* ElimControler);

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 180.f;
	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime = 10.f;

	float LevelStartingTime =0.f;

private:
	float CountDownTime = 0.f;

protected:
	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;
};
