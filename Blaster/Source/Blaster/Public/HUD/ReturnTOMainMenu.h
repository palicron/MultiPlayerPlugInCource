// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnTOMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UReturnTOMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void MenuSetUp();
	
	UFUNCTION()
	void MenuTearDown();

protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();

private:

	UPROPERTY(meta=(BindWidget))
	class UButton* ReturnButton;

	UFUNCTION()
	void ReturnButtonClick();

	UPROPERTY()
	class UMultiPlayerSessionsSubSystem* MultiPlayerSessionsSubSystem;

	class APlayerController*  PlayerCtr;

	
};
