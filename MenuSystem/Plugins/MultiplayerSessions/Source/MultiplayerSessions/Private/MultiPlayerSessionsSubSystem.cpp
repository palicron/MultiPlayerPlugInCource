// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiPlayerSessionsSubSystem.h"

#include "OnlineSubsystem.h"

UMultiPlayerSessionsSubSystem::UMultiPlayerSessionsSubSystem()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}
