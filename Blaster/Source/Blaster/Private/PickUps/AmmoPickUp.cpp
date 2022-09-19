// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/AmmoPickUp.h"
#include "Blaster/Public/BlasterComponents/CombatComponent.h"
#include "Character/BlasterCharacter.h"

void AAmmoPickUp::OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(Overlap, OtherActor, otherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCaracter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCaracter)
	{
		UCombatComponent* Combat = BlasterCaracter->GetCombat();
		if(IsValid(Combat))
		{
			Combat->PickUpAmmo(WeaponType,AmmoAmount);
		}
	}
	Destroy();
}
