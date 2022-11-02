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



public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void Heal(float HealAmount, float HealingTime);

	void BuffSpeed(float BuffBaseSpeed,float BuffCrouchSpeed,float BuffSpeedTime);

	void SetInitialSpeed(float BaseSpeed,float CrouchSpeed);
	
private:
	UPROPERTY()
	class ABlasterCharacter* Character;

	/**
	 *Heal buff
	 **/
	

	bool bHealing = false;
	
	float HealingRate = 0.f;
	
	float AmountToHeal = 0.f;

	
	/**
	 *Speed buff
	 **/

	FTimerHandle SpeedBuffTimer;

	float InitialBaseSpeed;

	float InitialCrouchSpeed;

	void ResetSpeeds();

	UFUNCTION(NetMulticast,Reliable)
	void MultiCastSpeedBuff(float BuffBaseSpeed,float BuffCrouchSpeed);
};
