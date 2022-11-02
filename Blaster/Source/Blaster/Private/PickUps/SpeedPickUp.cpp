// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/SpeedPickUp.h"

#include "BlasterComponents/BuffComponent.h"
#include "Character/BlasterCharacter.h"

void ASpeedPickUp::OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(Overlap, OtherActor, otherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	
	if(BlasterCharacter)
	{
		UBuffComponent* Buff  = BlasterCharacter->GetBuff();
		if(Buff)
		{
			Buff->BuffSpeed(BaseSpeedBuff,CrouchSpeedBuff,SpeedBuffTime);
		}
	}
	
	Destroy();
}
