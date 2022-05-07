// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "MultiPlayerSessionsSubSystem.h"
#include "Components/Button.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

void UMenu::MenuSetUp(int32 NumberOfPublicConnections ,FString TypeOfMatch)
{
	NumPublicConnections=NumberOfPublicConnections;
	MatchType=TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
    
	UWorld* World = GetWorld();

	if(World)
	{
		APlayerController* PLayerController = World->GetFirstPlayerController();
		if(PLayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PLayerController->SetInputMode(InputModeData);
			PLayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstace = GetGameInstance();
	if(GameInstace)
	{
		MultiPlayerSessionsSubSystem = GameInstace->GetSubsystem<UMultiPlayerSessionsSubSystem>();
	}
	if(MultiPlayerSessionsSubSystem)
	{
		MultiPlayerSessionsSubSystem->MultiplayerOnCreateSessionComplete.AddDynamic(this,&ThisClass::OnCreateSession);
		MultiPlayerSessionsSubSystem->MultiplayerOnFindSessionsComplete.AddUObject(this,&UMenu::OnFindSession);
		MultiPlayerSessionsSubSystem->MultiplayerOnJoinSessionComplete.AddUObject(this,&UMenu::OnJoinSession);
		MultiPlayerSessionsSubSystem->MultiplayerOnDestroySessionComplete.AddDynamic(this,&ThisClass::OnDestroySession);
		MultiPlayerSessionsSubSystem->MultiplayerOnStartSessionComplete.AddDynamic(this,&ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
    if(!Super::Initialize())
     {
     return false;
     }
   
     
       
	if(Host_Button)
	{
		Host_Button->OnClicked.AddDynamic(this,&ThisClass::HostButtonClicked);
	}
	if(Join_Button)
	{
		Join_Button->OnClicked.AddDynamic(this,&ThisClass::JoinButtonClicked);
	}
	return true;
	
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
	
}


void UMenu::HostButtonClicked()
{
	if(MultiPlayerSessionsSubSystem)
	{
		MultiPlayerSessionsSubSystem->CreateSession(NumPublicConnections,MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
   if(MultiPlayerSessionsSubSystem)
   {
   	 MultiPlayerSessionsSubSystem->FindSessions(10000);
   }
}

void UMenu::MenuTearDown()
{

	RemoveFromParent();
	UWorld* world1 = GetWorld();
	if(world1)
	{
		APlayerController* PlayerCtr = world1->GetFirstPlayerController();
		if(PlayerCtr)
		{
			FInputModeGameOnly InputModeData;
			PlayerCtr->SetInputMode(InputModeData);
			PlayerCtr->SetShowMouseCursor(false);
		}
	}
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Yellow,FString(TEXT("Session Create Correct")));
		}
		UWorld* World1 = GetWorld();	
		if(World1)
		{
			World1->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
			
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Red,FString(TEXT("Session Failed to create")));
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if(MultiPlayerSessionsSubSystem==nullptr)
		return;
	
	for(auto Result : SessionResults)
	{
		FString SettingValue;
		Result.Session.SessionSettings.Get(FName("MatchType"),SettingValue);
		if(SettingValue == MatchType)
		{
			MultiPlayerSessionsSubSystem->JoinSession(Result);
			return;
		}
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession,Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if(PlayerController)
			{
				PlayerController->ClientTravel(Address,ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

