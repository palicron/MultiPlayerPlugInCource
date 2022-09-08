// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileGrenada.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileGrenada::AProjectileGrenada()	
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementCOmponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}


void AProjectileGrenada::BeginPlay()
{
	AActor::BeginPlay();

	StartDestroyTimer();

	SpawnTrailSystem();	
	
	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this,&ThisClass::AProjectileGrenada::OnBounce);
	
}

void AProjectileGrenada::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if(IsValid(BounceSound) )
	{
		UGameplayStatics::PlaySoundAtLocation(this,BounceSound,GetActorLocation());
	}
}

void AProjectileGrenada::Destroyed()
{

	ExplodeDamage();
	Super::Destroyed();
}
