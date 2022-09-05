// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ShotGun.h"

#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotGun::Fire(const FVector& HitTarget)
{

	AWeapon::Fire(HitTarget);
	 
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr) return;
	AController* InstigatorCtr = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		for(uint32 i =0;i< NumberOfPellets;i++)
		{
			FVector End = TraceEndWithScatter(Start,HitTarget);
		}
	

		

		
	}
}
