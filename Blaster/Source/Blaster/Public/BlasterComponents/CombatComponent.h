// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
class ABlasterCharacter;
class AWeapon;
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"
#define TRACE_LENGHT  80000.f;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class ABlasterCharacter;

	void EquipWeapon(AWeapon* WeaponToEquip);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);
	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool ButtonPress);

	bool bFireButtonPress;

	UFUNCTION(Server,Reliable)
	void ServerFire();
	UFUNCTION(NetMulticast,Reliable)
	void MultiCastFire();

	void TraceUnderCrossHair(FHitResult& TraceHitResult);

private:
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	
	ABlasterCharacter* Character;
	
    UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed =600.f;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed = 450.f;
	

	

		
};
