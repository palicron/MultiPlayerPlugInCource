// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponTypes.h"
#include "Weapon.generated.h"
class USphereComponent;
class UWidgetComponent;
class UTexture2D;
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"),
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName="Equipped Secondary"),
	EWS_Drop UMETA(DisplayName="Drop"),
	EWS_MAX UMETA(DisplayName="DefaultMax")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName="HitScan Weapone"),
	EFT_Projectile UMETA(DisplayName="Projectile Weapone"),
	EFT_Shotgun UMETA(DisplayName="Shotgun Projectile"),

	EFT_MAX UMETA(DisplayName="DefaultMax")
	
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

	FVector TraceEndWithScatter(const FVector& HitTarget) const;

	void Dropped();

	void AddAmmo(int32 AmmoToAdd);
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

	UPROPERTY(EditAnywhere)
	EFireType FireType;

	
	UPROPERTY(EditAnywhere,Category="Weapone Scatter")
	bool bUseScatter = false;
	/**
	 * Automatic fire
	 */
	
	UPROPERTY(EditAnywhere,Category = Combat)
	float FireDelay = 0.15f;
	
	UPROPERTY(EditAnywhere,Category = Combat)
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

	void SetHUDAmmo();

	

	/**
	 *
	 **/

	void EnableCustomDepth(bool bEnable);
protected:
	
	virtual void BeginPlay() override;

	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDrop();
	virtual void OnEquippedSecondary();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlarp(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex);
	UPROPERTY(EditAnywhere,Category="Weapone Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere,Category="Weapone Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere)
	bool bUseServerSideRewind = false;

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;
private:

	/**
	* Zommed FOV while aiming
	**/
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterSpeed = 20.f;
	UPROPERTY( EditAnywhere)
	int32 Ammo;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	
	UPROPERTY(ReplicatedUsing= OnRep_WeaponState, VisibleAnywhere,Category= "Weapon Properties")
	EWeaponState WeaponState;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	USphereComponent* AreaSphere;
	
	UPROPERTY(VisibleAnywhere,Category= "Weapon Properties")
	UWidgetComponent* PickUpWidget;

	UFUNCTION(Client,Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client,Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);
	
	UFUNCTION()
	void OnRep_WeaponState();
	// the number of unprocessed Server Request For Ammo.
	//Increment n Spend Round Decremente in CLientUpdateAmmo;
	int32 Sequence = 0;
	UPROPERTY(EditAnywhere,Category="Weapon propertis")
	class UAnimationAsset* FireAnimation;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing> CasingClass;
	/** Tetures fro cross hairs**/

	void SpendRound();

	virtual void OnRep_Owner() override;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;




	
public:

	
	bool bDestroyWeapon = false;
	
	void SetWeaponeState(EWeaponState State);


	FORCEINLINE USphereComponent* GetAreSphere(){return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}

	FORCEINLINE float GetZoomedFOV() const {return ZoomFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterSpeed; };
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; };
	FORCEINLINE bool IsFull() const {return Ammo == MagCapacity;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE int32 GetAmmo() const { return  Ammo;}
	FORCEINLINE int32 GetMaxCapacity() const { return MagCapacity;}
	FORCEINLINE int32 GetDamage() const {return Damage;}

};
