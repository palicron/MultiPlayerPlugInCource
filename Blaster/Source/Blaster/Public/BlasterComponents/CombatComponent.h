// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "HUD/BlasterHUD.h"
#include "Weapon/WeaponTypes.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"
#define TRACE_LENGHT  80000.f;
class ABlasterCharacter;
class AWeapon;
class ABlasterPlayerController;
class ABlasterHUD;
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
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	int32 AmountToreload();

	void FireButtonPressed(bool ButtonPress);
protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);
	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();



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

	UFUNCTION(Server,Reliable)
	void ServerReload();

	UFUNCTION()
	void HandleReload();

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

	FHUDPackage HUDPackage;
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

	/**
	 *Auto Fire
	 ***/
	FTimerHandle FireTimer;
	
	bool bCanFire = true;

	void StartFireTimer();
	
	void FireTimerFinish();

	bool CanFire() const;

	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo = 0;
	
	TMap<EWeaponType,int32> CarriedAmmoMap;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 25.f;
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
};
