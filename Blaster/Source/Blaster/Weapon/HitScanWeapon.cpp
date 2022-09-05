// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr) return;
	AController* InstigatorCtr = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket)
	{
		
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;

		WeaponTraceHit(Start,HitTarget,FireHit);
		
		ABlasterCharacter* Character = Cast<ABlasterCharacter>(FireHit.GetActor());
		if(Character && InstigatorCtr && HasAuthority() )
		{
			UGameplayStatics::ApplyDamage(
		Character,Damage,InstigatorCtr,this,UDamageType::StaticClass());	
				
		}
		if(ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,FireHit.ImpactPoint,FireHit.ImpactNormal.Rotation());
		}
		if(HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint);
		}

		
		if(MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),MuzzleFlash,SocketTransform.GetLocation());
		}
		if(FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,FireSound,GetActorLocation());
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart,const FVector& HitTarget,FHitResult& OutHit)
{

	const UWorld* World = GetWorld();
	if(World)
	{
		FVector End = bUseScatter? TraceEndWithScatter(TraceStart,HitTarget) :TraceStart + (HitTarget-TraceStart) * 1.25f;
		
		World->LineTraceSingleByChannel(
			OutHit, TraceStart,End,ECollisionChannel::ECC_Visibility
				);

		FVector BeamEnd = End;
		if(OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
			if(BeamParticles)
			{
			
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World,BeamParticles,
					TraceStart,FRotator::ZeroRotator,true);
				Beam->SetVectorParameter(FName("Target"),BeamEnd);
			}
			
		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();

	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f,SphereRadius);

	FVector EndLoc = SphereCenter + RandVec;

	FVector ToEndLoc = EndLoc - TraceStart;
	

	FVector NewVect = FVector(TraceStart + ToEndLoc * 80000.f / ToEndLoc.Size());


	return NewVect;
}


