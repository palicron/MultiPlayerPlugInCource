// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ReturnTOMainMenu.h"

#include "MultiPlayerSessionsSubSystem.h"
#include "Character/BlasterCharacter.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"


bool UReturnTOMainMenu::Initialize()
{
	if(Super::Initialize())
	{
		return false;
	}


	return true;
}



void UReturnTOMainMenu::MenuSetUp()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if(World)
	{
		PlayerCtr = PlayerCtr==nullptr?World->GetFirstPlayerController():PlayerCtr;
		if(PlayerCtr)
		{
			FInputModeGameAndUI InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			PlayerCtr->SetInputMode(InputModeData);
			PlayerCtr->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstace = GetGameInstance();

	if(GameInstace)
	{
		MultiPlayerSessionsSubSystem = GameInstace->GetSubsystem<UMultiPlayerSessionsSubSystem>();
		if(MultiPlayerSessionsSubSystem)
		{
			MultiPlayerSessionsSubSystem->MultiplayerOnDestroySessionComplete.AddDynamic(this,&ThisClass::UReturnTOMainMenu::OnDestroySession);
		}
	}

	if(ReturnButton && !ReturnButton->OnClicked.IsBound()) 
	{
		ReturnButton->OnClicked.AddDynamic(this,&ThisClass::ReturnButtonClick);
	}
}

void UReturnTOMainMenu::MenuTearDown()
{
	RemoveFromParent();

	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if(World)
	{
		PlayerCtr = PlayerCtr==nullptr?World->GetFirstPlayerController():PlayerCtr;
		if(PlayerCtr)
		{
			FInputModeGameOnly InputModeData;
			PlayerCtr->SetInputMode(InputModeData);
			PlayerCtr->SetShowMouseCursor(false);
		}
	}
	if(ReturnButton && ReturnButton->OnClicked.IsBound()) 
	{
		ReturnButton->OnClicked.RemoveDynamic(this,&ThisClass::ReturnButtonClick);
	}
	if(MultiPlayerSessionsSubSystem && MultiPlayerSessionsSubSystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiPlayerSessionsSubSystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this,&UReturnTOMainMenu::OnDestroySession);
	}
}


void UReturnTOMainMenu::ReturnButtonClick()
{
	ReturnButton->SetIsEnabled(false);

	UWorld* world = GetWorld();

	if(world)
	{
		APlayerController* FirstPlayerCtr =  world->GetFirstPlayerController();

		if(FirstPlayerCtr)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerCtr->GetPawn()) ;
			if(BlasterCharacter)
			{
				BlasterCharacter->OnLeftGame.AddDynamic(this,&ThisClass::UReturnTOMainMenu::OnPlayerLeftGame);
				BlasterCharacter->ServerLeaveGame();
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}

}

void UReturnTOMainMenu::OnDestroySession(bool bWasSuccessful)
{
	if(!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}
	UWorld* World = GetWorld();
	if(World)
	{
		AGameModeBase* GameMode =  World->GetAuthGameMode<AGameModeBase>();
		if(GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			PlayerCtr = PlayerCtr==nullptr?World->GetFirstPlayerController():PlayerCtr;
			if(PlayerCtr)
			{
				PlayerCtr->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UReturnTOMainMenu::OnPlayerLeftGame()
{
	if(MultiPlayerSessionsSubSystem)
	{
		MultiPlayerSessionsSubSystem->DestroySession();
	}
}
