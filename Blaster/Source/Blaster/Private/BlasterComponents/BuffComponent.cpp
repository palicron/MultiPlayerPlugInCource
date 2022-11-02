// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/BuffComponent.h"

#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




// Called every frame
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;

	HealingRate = HealAmount/ HealingTime;
	AmountToHeal += HealAmount;

	
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if(!bHealing || Character == nullptr || Character->IsElimmed()) return;

	const float HealThisFrame = HealingRate *DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth()+HealThisFrame,0,Character->GetMaxHealth()));
	Character->UpdateHudHealth();
	AmountToHeal-= HealThisFrame;

	if(AmountToHeal <= 0.f || AmountToHeal >=Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}


void UBuffComponent::SetInitialSpeed(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}


void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffSpeedTime)
{
	if(Character == nullptr) return;
	
	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer,this,&UBuffComponent::ResetSpeeds,BuffSpeedTime);

	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
		MultiCastSpeedBuff(BuffBaseSpeed,BuffCrouchSpeed);
	}
	
	
}


void UBuffComponent::ResetSpeeds()
{
	if(Character == nullptr) return;
	
	
	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
		MultiCastSpeedBuff(InitialBaseSpeed,InitialCrouchSpeed);
	}
	
}

void UBuffComponent::MultiCastSpeedBuff_Implementation(float BuffBaseSpeed, float BuffCrouchSpeed)
{
	if(Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
}
