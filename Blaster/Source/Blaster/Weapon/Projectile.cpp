// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AProjectile::AProjectile()
{

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);

	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility,ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh,ECollisionResponse::ECR_Block);
	
	
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if(Tracer)
	{
		TraceComponent = UGameplayStatics::SpawnEmitterAttached(Tracer,CollisionBox,FName(),
			GetActorLocation(),GetActorRotation(),EAttachLocation::KeepWorldPosition);
	}

	if(HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this,&ThisClass::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	Destroy();
}



void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectile::SpawnTrailSystem()
{
	if(TrailSystem)
	{
		TrailSystemcomp =  UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem,GetRootComponent(),FName(),GetActorLocation(),GetActorRotation(),
		EAttachLocation::KeepWorldPosition,false);
	}
}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(DestroyTimer,this,&ThisClass::DestroyTimerFinish,DestroyTime);
}

void AProjectile::DestroyTimerFinish()
{
	Destroy();
}

void AProjectile::Destroyed()
{
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
	}
	Super::Destroyed();

}


