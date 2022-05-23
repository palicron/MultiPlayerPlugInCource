// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class AWeapon;
UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABlasterCharacter();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	
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

public:
	 void SetOverlappingWeapon(AWeapon* weapon);

	

};


