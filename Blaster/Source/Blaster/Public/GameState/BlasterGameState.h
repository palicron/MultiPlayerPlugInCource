// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"
#include "BlasterGameState.generated.h"
class ABlasterPlayerState;
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()

public:

	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;
	void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);

	float TopScore;

	private:
	
	


	
};
