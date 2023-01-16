// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"

#include "BlasterPlayerCtr/BlasterPlayerController.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameState/BlasterGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/BlasterPlayerState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}
ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}



void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmUpTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime<=0.f)
		{
			StartMatch();
		}
	}
	else if( MatchState == MatchState::InProgress)
	{
		CountDownTime = WarmUpTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime<=0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if(MatchState == MatchState::Cooldown)
	{
		CountDownTime = CountDownTime + WarmUpTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountDownTime<=0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();It;It++)
	{
		ABlasterPlayerController* BlasterPLayer = Cast<ABlasterPlayerController>(*It);
		if(BlasterPLayer)
		{
			BlasterPLayer->OnMatchStateSet(MatchState);
		}
	}
}
void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimCharacter, ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController?Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController?Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if(AttackerPlayerState && AttackerPlayerState!= VictimPlayerState && BlasterGameState)
	{
		
		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);
		
	}
	if(VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
	if(ElimCharacter)
	{
		ElimCharacter->Elim(false);
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimCharacter, AController* ElimControler)
{
	if(ElimCharacter)
	{
		ElimCharacter->Reset();
		ElimCharacter->Destroy();
	}
	if(ElimControler)
	{
		TArray<AActor*> playerStats;
		UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(),playerStats);
		int32 Selection = FMath::RandRange(0,playerStats.Num()-1);
		RestartPlayerAtPlayerStart(ElimControler,playerStats[Selection]);
	}
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState* PlayerLeaving)
{
	if(PlayerLeaving == nullptr)
		return;
	
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if(BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	ABlasterCharacter* CharacterLeaving =  Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());

	if(CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
	
}

