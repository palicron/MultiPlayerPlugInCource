// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);



    APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	UWorld* world = GetWorld();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleFlashSocket)
	{
		FTransform SocketTransform =  MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile* SpawnedProjectile = nullptr;
		if(bUseServerSideRewind)
		{
			if(InstigatorPawn->HasAuthority()) //Server
			{
				if(InstigatorPawn->IsLocallyControlled())//Server, host use repricalted projectile
				{
					SpawnedProjectile = world->SpawnActor<AProjectile>(ProjectileClass,SocketTransform.GetLocation(),
					TargetRotation,	SpawnParams	);
				
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
				}
				else //Server, Not locally control Spawn non replciate Projectin no ssr
				{
					SpawnedProjectile = world->SpawnActor<AProjectile>(ServerSideRewindProjectileClass,SocketTransform.GetLocation(),
					TargetRotation,	SpawnParams	);
				
					SpawnedProjectile->bUseServerSideRewind = false;
				}
					
			}
			else// Client, Using SSR
			{
				if(InstigatorPawn->IsLocallyControlled())//client locally control - spawn non replicated projectile, use ssr
				{
					SpawnedProjectile = world->SpawnActor<AProjectile>(ServerSideRewindProjectileClass,SocketTransform.GetLocation(),
					TargetRotation,	SpawnParams	);
		
					SpawnedProjectile->bUseServerSideRewind = true;
					SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
					SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
					SpawnedProjectile->Damage = Damage;
				}
				else // CLient, not locally controll - spawn non replciated no ssr
				{
					SpawnedProjectile = world->SpawnActor<AProjectile>(ServerSideRewindProjectileClass,SocketTransform.GetLocation(),
					TargetRotation,	SpawnParams	);
				
					SpawnedProjectile->bUseServerSideRewind = false;
				}
			}
		}
		else //weapon no using ssr
		{
			if(InstigatorPawn->HasAuthority())
			{
				SpawnedProjectile = world->SpawnActor<AProjectile>(ProjectileClass,SocketTransform.GetLocation(),
				TargetRotation,	SpawnParams	);
			
				SpawnedProjectile->bUseServerSideRewind = false;
				SpawnedProjectile->Damage = Damage;
			}
		}
		
	}
	
}
