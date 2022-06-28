// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
class ABlasterCharacter;
class AWeapon;
class ABlasterPlayerController;
class ABlasterHUD;
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

	float CrossHairVelocityFactor;

	float CroosHairInAirFactor;

	float CrossHairAimFactor;

	float CrossHairShootingFactor;
	
	FVector HitTarget;
	
	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast,Reliable)
	void MultiCastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrossHair(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	
	ABlasterCharacter* Character;
	ABlasterPlayerController* Controller;
	ABlasterHUD* BlasterHUD;
	
	
    UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed =600.f;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed = 450.f;

	/**
	 * Aiming and FOV
	 */
	//Field of view when not aiming, set to the cameras base FOV
	float DefaultFOV;
	UPROPERTY(EditAnywhere,Category = Combat)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere,Category = Combat)
	float ZoomInterpSeed = 20.f;

	float CurrentFOV;

	void InterpFOV(float DeltaTime);
	
};
