// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UBuffComponent();
	friend class ABlasterCharacter;

protected:

	virtual void BeginPlay() override;

	void HealRampUp(float DeltaTime);

	void ShieldRampUp(float DeltaTime);

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Heal(float HealAmount, float HealingTime);

	void ReplenishShield(float ShieldAmount, float ShieldTime);

	void BuffSpeed(float BuffBaseSpeed,float BuffCrouchSpeed,float BuffSpeedTime);
	
	void BuffJump(float BuffJumpVelocity,float BuffTime);

	void SetInitialSpeed(float BaseSpeed,float CrouchSpeed);

	void SetInitialJumpVelocity(float Velocity);
	
	
private:
	UPROPERTY()
	class ABlasterCharacter* Character;

	/**
	 *Heal buff
	 **/
	

	bool bHealing = false;

	bool bSheling = false;
	
	float HealingRate = 0.f;
	
	float AmountToHeal = 0.f;

	float ShieldRate = 0.f;
	
	float AmountToShield = 0.f;

	
	/**
	 *Speed buff
	 **/

	FTimerHandle SpeedBuffTimer;

	float InitialBaseSpeed;

	float InitialCrouchSpeed;

	void ResetSpeeds();

	UFUNCTION(NetMulticast,Reliable)
	void MultiCastSpeedBuff(float BuffBaseSpeed,float BuffCrouchSpeed);

	/**
	*Jump buff
	**/

	FTimerHandle JumpBuffTimer;


	

	void ResetJump();

	float InitialJumpVelocity;

	UFUNCTION(NetMulticast,Reliable)
	void MultiCastJumpBuff(float BuffJumpVelocity);
};
