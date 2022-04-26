// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiPlayerSessionsSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiPlayerSessionsSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiPlayerSessionsSubSystem();

protected:

private:
	IOnlineSessionPtr SessionInterface;
	
};
