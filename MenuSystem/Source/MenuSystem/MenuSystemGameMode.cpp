// Copyright Epic Games, Inc. All Rights Reserved.

#include "MenuSystemGameMode.h"
#include "MenuSystemCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "UObject/ConstructorHelpers.h"

AMenuSystemGameMode::AMenuSystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMenuSystemGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1,60.f,FColor::Yellow,
	        FString::Printf(TEXT("Player In game: %d"),NumberOfPlayers));
			APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
			
		    if(PlayerState)
		    {
		    	FString PlayerName = PlayerState->GetPlayerName();
		    	GEngine->AddOnScreenDebugMessage(-1,60.f,FColor::Cyan,
		FString::Printf(TEXT("%s has joined the game!!!!"),*PlayerName));
		    }
		
		}
	}
}

void AMenuSystemGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1,60.f,FColor::Yellow,
			FString::Printf(TEXT("Player In game: %d"),NumberOfPlayers-1));
			APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
			
			if(PlayerState)
			{
				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(-1,60.f,FColor::Cyan,
		FString::Printf(TEXT("%s has Exiting the game!!!!"),*PlayerName));
			}
		
		}
	}
}
