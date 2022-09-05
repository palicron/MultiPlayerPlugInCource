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
		
		TMap<ABlasterCharacter*,uint32> HitMap;
		for(uint32 i =0;i< NumberOfPellets;i++)
		{			
			FHitResult FireHit;
			WeaponTraceHit(Start,HitTarget,FireHit);

			ABlasterCharacter* Character = Cast<ABlasterCharacter>(FireHit.GetActor());
			if(Character && InstigatorCtr && HasAuthority() )
			{
				if(HitMap.Contains(Character))
				{
					HitMap[Character]++;
				}
				else
				{
					HitMap.Emplace(Character,1);
				}
			}
			if(ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,FireHit.ImpactPoint,FireHit.ImpactNormal.Rotation());
			}
			if(HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint,
					.5f,FMath::FRandRange(-.5f,.5f));
			}
		}
	
		for(auto HitPair :HitMap)
		{
			if(HitPair.Key && InstigatorCtr && HasAuthority() )
			{
				UGameplayStatics::ApplyDamage(
			HitPair.Key,Damage * HitPair.Value,InstigatorCtr,this,UDamageType::StaticClass());	
				
			}
		}
		
	}
}
