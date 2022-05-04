// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void MenuSetUp(int32 NumberOfPublicConnections = 4,FString TypeOfMatch=FString(TEXT("FreeForAll")));
protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;
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
	
	class UMultiPlayerSessionsSubSystem* MultiPlayerSessionsSubSystem;

	int32 NumPublicConnections{4};

	FString MatchType{TEXT("FreeForAll")};

	float test1;
	
};
