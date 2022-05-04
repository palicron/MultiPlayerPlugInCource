// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayerSessionsSubSystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

UMultiPlayerSessionsSubSystem::UMultiPlayerSessionsSubSystem():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this,&ThisClass::OnFindSessionComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnJoinsSessionComplete)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this,&ThisClass::OnDestroySessionComplete)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}
/////////////////// Funtions ////////////////////////////

void UMultiPlayerSessionsSubSystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if(!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession !=nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
	//Store de delegate in a handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings);
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName()=="Null"? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("MatchType"),MatchType,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!SessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(),NAME_GameSession,*LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		//Brodcast

		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
	
	
}

void UMultiPlayerSessionsSubSystem::FindSessions(int32 MacSearchResults)
{
}

void UMultiPlayerSessionsSubSystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
}

void UMultiPlayerSessionsSubSystem::DestroySession()
{
}

void UMultiPlayerSessionsSubSystem::StartSessions()
{
}
/////////////////// Delegate callbacks ////////////////////////////
void UMultiPlayerSessionsSubSystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiPlayerSessionsSubSystem::OnFindSessionComplete(bool bWasSuccessful)
{
}

void UMultiPlayerSessionsSubSystem::OnJoinsSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}

void UMultiPlayerSessionsSubSystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMultiPlayerSessionsSubSystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}


