// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"
class USphereComponent;
class UWidgetComponent;
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

protected:
	
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlarp(UPrimitiveComponent* Overlap,AActor* OtherActor,UPrimitiveComponent* otherComp,
		int32 OtherBodyIndex);

private:
	
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

	UPROPERTY(EditAnywhere,Category="Weapon propertis")
	class UAnimationAsset* FireAnimation;
public:
	 void SetWeaponeState(EWeaponState State);

	FORCEINLINE USphereComponent* GetAreSphere(){return AreaSphere;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
    
	

};
