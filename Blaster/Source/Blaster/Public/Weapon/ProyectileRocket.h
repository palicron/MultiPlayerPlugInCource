// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ProyectileBulled.h"
#include "ProyectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProyectileRocket : public AProjectile
{
	GENERATED_BODY()
public:
	AProyectileRocket();
protected:
	virtual void OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rocket Config")
	float ExploitationInnerRadius = 200.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Rocket Config")
	float ExploitationOuterRadius = 500.f;
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
};
