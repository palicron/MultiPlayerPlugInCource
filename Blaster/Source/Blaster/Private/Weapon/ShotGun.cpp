// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ShotGun.h"

#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


void AShotGun::FirShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr) return;
	AController* InstigatorCtr = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		TMap<ABlasterCharacter*,uint32> HitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start,HitTarget,FireHit);

			
			ABlasterCharacter* Character = Cast<ABlasterCharacter>(FireHit.GetActor());
			if(Character)
			{
				if(HitMap.Contains(Character))
				{
					HitMap[Character]++;
				}
				else
				{
					HitMap.Emplace(Character,1);
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

void AShotGun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket == nullptr) return;
	
		
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart  = SocketTransform.GetLocation();
		
	
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();

	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	
	for(uint32 i =0;i<NumberOfPellets;i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f,SphereRadius);
		
		const FVector EndLoc = SphereCenter + RandVec;

		const FVector ToEndLoc = EndLoc - TraceStart;

		HitTargets.Add(	FVector(TraceStart + ToEndLoc * 80000.f / ToEndLoc.Size()));
	}
}
