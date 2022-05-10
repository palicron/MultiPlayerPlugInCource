// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetUp(int32 NumberOfPublicConnections = 4,FString TypeOfMatch=FString(TEXT("FreeForAll")),FString lobbyPath =FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));
protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//Callback for Delegates in MultipalyerSessionSubsystem
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults,bool bWasSuccessful);
	
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
private:
	UPROPERTY(meta=(BindWidget))
	class UButton* Host_Button;
	UPROPERTY(meta=(BindWidget))
	UButton* Join_Button;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();
	
	UPROPERTY()
	class UMultiPlayerSessionsSubSystem* MultiPlayerSessionsSubSystem;

	int32 NumPublicConnections{4};

	FString MatchType{TEXT("FreeForAll")};

	FString PathToLobby{TEXT("")};
	
};
