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
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	APawn* FiringPawn = GetInstigator();

	if(FiringPawn && HasAuthority()) 
	{
		AController* FiringController = FiringPawn->GetController();
		if(FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this,
				Damage,
				10.f,
				GetActorLocation(),
				ExploitationInnerRadius,
				ExploitationOuterRadius,
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				FiringController
				);
		}
	}
	GetWorldTimerManager().SetTimer(DestroyTimer,this,&ThisClass::DestroyTimerFinish,DestroyTime);
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactParticles,GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation());
	}
	if(RocketMesh)
	{
		RocketMesh->SetVisibility(false);
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
	if(TrailSystem)
	{
	TrailSystemcomp = 	UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem,GetRootComponent(),FName(),GetActorLocation(),GetActorRotation(),
			EAttachLocation::KeepWorldPosition,false);
	}
	if(ProjectileLoop && LoopingSoundAtten)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop,GetRootComponent(),FName(),GetActorLocation(),EAttachLocation::KeepWorldPosition
			,false,1.f,1.f,0.f,LoopingSoundAtten,(USoundConcurrency*)nullptr,false);
	}
}

void AProyectileRocket::DestroyTimerFinish()
{
	Destroy();
}
void AProyectileRocket::Destroyed()
{
	
}
