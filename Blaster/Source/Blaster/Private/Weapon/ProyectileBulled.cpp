// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProyectileBulled.h"

#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AProyectileBulled::AProyectileBulled()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementCOmponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}
#if WITH_EDITOR
void AProyectileBulled::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	if(PropertyChangedEvent.Property!=nullptr &&
		PropertyChangedEvent.Property->GetFName().IsEqual(GET_MEMBER_NAME_CHECKED(AProyectileBulled,InitialSpeed)))
	{
		if(ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	
	}
}
#endif

void AProyectileBulled::BeginPlay()
{
	Super::BeginPlay();

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithChannel = true;
	PathParams.bTraceWithCollision = true;
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	PathParams.MaxSimTime = 4.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 30.f;
	PathParams.StartLocation = GetActorLocation();
	PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	PathParams.ActorsToIgnore.Add(this);
	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this,PathParams,PathResult);
}

void AProyectileBulled::OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
   ACharacter* OwnerCharacter =  Cast<ACharacter>(GetOwner()) ;
	if(OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if(OwnerController)
		{
			UGameplayStatics::ApplyDamage(Other,Damage,OwnerController,this,UDamageType::StaticClass());
		}
		
	}
   

	
	Super::OnHit(MyComp, Other, OtherComp, NormalImpulse, Hit);
}

