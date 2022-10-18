// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/HealthPickUp.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/BlasterCharacter.h"
#include "Blaster/Public/BlasterComponents/BuffComponent.h"

AHealthPickUp::AHealthPickUp()
{
	bReplicates = true;
	PickUPEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickUpEffectComponent"));

	PickUPEffectComponent->SetupAttachment(RootComponent);
	
}



void AHealthPickUp::OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
	
                                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(Overlap, OtherActor, otherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	
	if(BlasterCharacter)
	{
		
	}
	
	Destroy();
}
void AHealthPickUp::Destroyed()
{

	if(PickUpEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,PickUpEffect,GetActorLocation()
			,GetActorRotation());
	}
	Super::Destroyed();
}