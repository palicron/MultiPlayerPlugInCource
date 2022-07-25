// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterComponents/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Public/Interfaces/InteractWithCrossHairsInterface.h"
#include "Components/TimelineComponent.h"


#include "BlasterCharacter.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter,public IInteractWithCrossHairsInterface
{
	GENERATED_BODY()

public:

	ABlasterCharacter();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;
	
	virtual void OnRep_ReplicatedMovement() override;

	void PlayFireMontage(bool bAiming);
	void PlayReLoadMontage();
	void PLayElimMontage();
	void Elim();
	UFUNCTION(NetMulticast,Reliable)
	void MulticasElim();

	virtual void Destroyed() override;

protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPress();
	void AimButtonPressed();
	void AimButtonReleased();
	void Calculate_Pitch();
	void AimOffSet(float DeltaTime);
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonRealese();
	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamgeActor,float Damage,const UDamageType* DamageType,class AController* InstigatorController,AActor* DamageCauser);

	void UpdateHudHealth();

	void PollInit();
	
private:
	UPROPERTY(VisibleAnywhere,Category= Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere,Category= Camera)
	UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess="true"))
	UWidgetComponent* OverHeadWidget;
	UPROPERTY(ReplicatedUsing= OnRep_OverlappingWeapon)
	AWeapon* OverLappingWeapon;
    UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();

	UPROPERTY(VisibleAnywhere)
	UCombatComponent* Combat;

	float AO_Yaw;

	float InterpAO_Yaw;

	float AO_Pitch;
	
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere,Category="Combat",meta = (AllowPrivateAccess="true"))
	class UAnimMontage* FireWeaponeMontage;
	UPROPERTY(EditAnywhere,Category="Combat",meta = (AllowPrivateAccess="true"))
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere,Category="Combat",meta = (AllowPrivateAccess="true"))
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere,Category="Combat",meta = (AllowPrivateAccess="true"))
	UAnimMontage* ElimMontage;


	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraTreshold=200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovement;

	float CalculateSpeed();
	/**
	 * Player Health
	 */

	UPROPERTY(EditAnywhere, Category="Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing=OnRep_Health,VisibleAnywhere,Category="Player States")
	float Health = 100.f;

	class ABlasterPlayerController* BlastertPlayerCtr;

	bool bElim=false;
	UFUNCTION()
	void OnRep_Health();

	FTimerHandle EliminTimer;
	
	void ElimTimerFinish();
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.0f;
	/**
	 * Disolve Affect
	 ***/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolbeTimeline;

	FOnTimelineFloat DissolverTrack;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(EditAnywhere)
	UCurveFloat* DisolveCurve;

	UPROPERTY(VisibleAnywhere,Category="Elim")
	UMaterialInstanceDynamic* DynamicDesolveMaterialIntance;

	UPROPERTY(EditAnywhere,Category="Elim")
	UMaterialInstance* DesolveMaterialIntance;

	/*
	 * Elim Bot
	 */

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;
	
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponet;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;
	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
public:
	 void SetOverlappingWeapon(AWeapon* weapon);

	bool IsWeaponEquipped();

	bool IsAiming();
	

	AWeapon* GetEquippedWeapone() const;

	FVector GetHitTarget() const;


	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}

	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}

	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	
	FORCEINLINE bool IsElimmed() {return bElim;};

	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}

	FORCEINLINE bool ShouldRotateRootBone() const {return bRotateRootBone;}

	FORCEINLINE float GetHealth() const {return Health;}

	FORCEINLINE float GetMaxHealth() const {return MaxHealth;}

};





