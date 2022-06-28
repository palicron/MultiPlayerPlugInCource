// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterComponents/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Public/Interfaces/InteractWithCrossHairsInterface.h"
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

	void PlayFireMontage(bool bAiming);

	virtual void OnRep_ReplicatedMovement() override;
	


	UFUNCTION(NetMulticast,Unreliable)
	void MultiCastHit();
protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void Calculate_Pitch();
	void AimOffSet(float DeltaTime);
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonRealese();
	void PlayHitReactMontage();
	
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
	 UAnimMontage* HitReactMontage;
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
public:
	 void SetOverlappingWeapon(AWeapon* weapon);

	bool IsWeaponEquipped();

	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}

	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}

	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	
	AWeapon* GetEquippedWeapone() const;

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}

	FORCEINLINE bool ShouldRotateRootBone() const {return bRotateRootBone;}

};





