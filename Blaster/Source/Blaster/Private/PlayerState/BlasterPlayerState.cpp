// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BlasterPlayerState.h"

#include "BlasterPlayerCtr/BlasterPlayerController.h"
#include "Character/BlasterCharacter.h"

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	
	SetScore(GetScore() + ScoreAmount);
	Character =  Character==nullptr?Cast<ABlasterCharacter>( GetPawn()):Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	Character =  Character==nullptr?Cast<ABlasterCharacter>( GetPawn()):Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}


