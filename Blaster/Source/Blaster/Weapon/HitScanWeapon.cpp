// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

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
		FVector End = Start + (HitTarget-Start) * 1.25f;

		FHitResult FireHit;

		const UWorld* World = GetWorld();
		if(World)
		{
			World->LineTraceSingleByChannel(
			FireHit, Start,End,ECollisionChannel::ECC_Visibility
				);
		}
		FVector BeamEnd = End;
		if(FireHit.bBlockingHit)
		{
			ABlasterCharacter* Character = Cast<ABlasterCharacter>(FireHit.GetActor());
			if(Character && InstigatorCtr && HasAuthority() )
			{
				BeamEnd = FireHit.ImpactPoint;
				UGameplayStatics::ApplyDamage(
			Character,Damage,InstigatorCtr,this,UDamageType::StaticClass());	
				
			}
			if(ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World,ImpactParticles,FireHit.ImpactPoint,FireHit.ImpactNormal.Rotation());
			}
			if(HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this,HitSound,FireHit.ImpactPoint);
			}
		}
		if(BeamParticles)
		{
			
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World,BeamParticles,SocketTransform.GetLocation());
			Beam->SetVectorParameter(FName("Target"),BeamEnd);
		}

		if(MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World,MuzzleFlash,SocketTransform.GetLocation());
		}
		if(FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,FireSound,GetActorLocation());
		}
	}
}
