// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProyectileRocket.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Weapon/RocketMovementComponent.h"

AProyectileRocket::AProyectileRocket()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("Rocket Movement Component"));
	RocketMovementComponent->bRotationFollowsVelocity= true;
	RocketMovementComponent->SetIsReplicated(true);
}


void AProyectileRocket::OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	if(Other == GetOwner())
	{
		return;
	}
	ExplodeDamage();
	StartDestroyTimer();
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
	}
	if(ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if(CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(IsValid(TrailSystemcomp) && TrailSystemcomp->GetSystemInstance())
	{
		TrailSystemcomp->GetSystemInstance()->Deactivate();
	}
	if(ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}


void AProyectileRocket::BeginPlay()
{
	Super::BeginPlay();
	if(!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this,&ThisClass::OnHit);
	}
	
	SpawnTrailSystem();
	if(ProjectileLoop && LoopingSoundAtten)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop,GetRootComponent(),FName(),GetActorLocation(),EAttachLocation::KeepWorldPosition
		,false,1.f,1.f,0.f,LoopingSoundAtten,(USoundConcurrency*)nullptr,false);
	}
}


void AProyectileRocket::Destroyed()
{
	
}

#if WITH_EDITOR
void AProyectileRocket::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.Property!=nullptr &&
PropertyChangedEvent.Property->GetFName().IsEqual(GET_MEMBER_NAME_CHECKED(URocketMovementComponent,InitialSpeed)))
	{
		if(RocketMovementComponent)
		{
			RocketMovementComponent->InitialSpeed = InitialSpeed;
			RocketMovementComponent->MaxSpeed = InitialSpeed;
		}
	
	}
}
#endif
	
