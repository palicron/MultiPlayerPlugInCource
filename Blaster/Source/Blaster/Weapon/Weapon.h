// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"
class USphereComponent;
class UWidgetComponent;
class UTexture2D;
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Drop UMETA(DisplayName="Drop"),
	EWS_MAX UMETA(DisplayName="DefaultMax")
};
UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon();
	
	virtual void Tick(float DeltaTime) override;

	void ShowPickUpWidget(bool BShowWidget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	virtual void Fire(const FVector& HitTarget);

	void Dropped();
	
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairCenter;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairLeft;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairRight;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairUp;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairDown;


	/**
	 * Automatic fire
	 */
	
	UPROPERTY(EditAnywhere,Category = Combat)
	float FireDelay = 0.15f;
	
	UPROPERTY(EditAnywhere,Category = Combat)
	bool bAutomatic = true;

	void SetHUDAmmo();
protected:
	
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlarp(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex);

private:

	/**
	* Zommed FOV while aiming
	**/
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterSpeed = 20.f;
	UPROPERTY(ReplicatedUsing=OnRep_Ammo, EditAnywhere)
	int32 Ammo;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;
	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere,Category= "Weapon Properties")
	EWeaponState WeaponState;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	USphereComponent* AreaSphere;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	UWidgetComponent* PickUpWidget;

	UFUNCTION()
	void OnRep_WeaponState();
	
	UFUNCTION()
	void OnRep_Ammo();
	
	UPROPERTY(EditAnywhere,Category="Weapon propertis")
	class UAnimationAsset* FireAnimation;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
	/** Tetures fro cross hairs**/

	void SpendRound();

	virtual void OnRep_Owner() override;
	
public:
	 void SetWeaponeState(EWeaponState State);

	FORCEINLINE USphereComponent* GetAreSphere(){return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}

	FORCEINLINE float GetZoomedFOV() const {return ZoomFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterSpeed; };
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; };
	
    
	

};
