// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverHeadWidget.h"

#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(FString ToDisplay)
{
	if(DisplayText)
	{
		DisplayText->SetText(FText::FromString(ToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_None:
		Role=FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role=FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		Role=FString("Autonomous Proxy");
		break;
	case ROLE_Authority:
		Role=FString("Authority");
		break;
	case ROLE_MAX:
		break;
	default: ;
	}
	FString RemoteRoleString = FString::Printf(TEXT("Remote role: %s"),*Role);
	SetDisplayText(RemoteRoleString);
}

void UOverHeadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
	
}
