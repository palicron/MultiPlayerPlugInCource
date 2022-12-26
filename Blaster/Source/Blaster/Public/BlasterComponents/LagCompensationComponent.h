// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT()
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
	
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName,FBoxInformation> HitBoxInfo;

	UPROPERTY()
	class ABlasterCharacter* Character = nullptr;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;
	
	UPROPERTY()
	bool bHeadShot;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<class ABlasterCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap< ABlasterCharacter*, uint32> NodyShots;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	ULagCompensationComponent();

	friend class ABlasterCharacter;

	void ShowFramePackage(const FFramePackage& Package,const FColor& Color) const;

	FServerSideRewindResult ServerSideRewind(class ABlasterCharacter* HitCharacter,const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation, float HitTime);

	UFUNCTION(Server,Reliable)
	void ServerScoreRequest(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,float HitTime , class AWeapon* DamageCauser);

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void SaveFramePackage();

	void SaveFramePackage(FFramePackage& Package);

	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame,float HitTime);

	FServerSideRewindResult ConfirmHit(const FFramePackage& Package,
		ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,const FVector_NetQuantize& HitLocation);

	void CacheBoxPosition(ABlasterCharacter* HitCharacter,FFramePackage& FramePackage);

	void MoveBoxes(ABlasterCharacter* HitCharacter,const FFramePackage& OutFramePackage);

	void ResetHitBoxes(ABlasterCharacter* HitCharacter,const FFramePackage& OutFramePackage);

	void EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter,ECollisionEnabled::Type CollisionEnable);

	UPROPERTY(EditAnywhere)
	float MaxRecordTIme = 4.f;

	FFramePackage GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime);
	/**
	 * ShoutGuun 
	 */

	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<ABlasterCharacter*>& HitCharacters,const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,float HitTime);
	
	FShotgunServerSideRewindResult ShotgunConfirmHit(const TArray<FFramePackage>& FramesPackages,const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& Hitlocations);
public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	class ABlasterPlayerController* Controller;
	
	TDoubleLinkedList<FFramePackage> FrameHistory;


		
};



