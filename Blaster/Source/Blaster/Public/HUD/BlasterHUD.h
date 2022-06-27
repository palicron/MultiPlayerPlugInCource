// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category= CrossHair)
	class UTexture2D*  CrossHairCenter;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairLeft;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairRight;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairUp;
	UPROPERTY(EditAnywhere,Category= CrossHair)
	UTexture2D*  CrossHairDown;
};
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackage;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage Package) {HUDPackage=Package;}
	
};
