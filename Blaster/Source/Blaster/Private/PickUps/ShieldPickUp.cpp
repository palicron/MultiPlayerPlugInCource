// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/ShieldPickUp.h"

#include "BlasterComponents/BuffComponent.h"
#include "Character/BlasterCharacter.h"

void AShieldPickUp::OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
                                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(Overlap, OtherActor, otherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	
	if(BlasterCharacter)
	{
		UBuffComponent* Buff  = BlasterCharacter->GetBuff();
		if(Buff)
		{
			Buff->ReplenishShield(ShieldAmount,ShieldTime);
		}
	}
	
	Destroy();
}
