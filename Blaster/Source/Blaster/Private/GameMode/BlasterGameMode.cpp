// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/BlasterGameMode.h"

#include "BlasterPlayerCtr/BlasterPlayerController.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimCharacter, ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController)
{
	if(ElimCharacter)
	{
		ElimCharacter->Elim();
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
