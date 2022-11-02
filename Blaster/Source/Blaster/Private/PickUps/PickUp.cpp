// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/PickUp.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/WeaponTypes.h"


APickUp::APickUp()
{
 
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	OverLapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap Sphere"));
	OverLapSphere->SetupAttachment(RootComponent);
	OverLapSphere->SetSphereRadius(150.f);
	OverLapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverLapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverLapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pick Up Mehs"));

	PickUpMesh->SetupAttachment(RootComponent);
	PickUpMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickUpMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	PickUpMesh->SetRenderCustomDepth(true);
	PickUpMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);

	PickUPEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickUpEffectComponent"));

	PickUPEffectComponent->SetupAttachment(RootComponent);
	
}


void APickUp::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		OverLapSphere->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::APickUp::OnSphereOverlap);
	}

	
}

void APickUp::OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}


void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(PickUpMesh)
	{
		PickUpMesh->AddLocalRotation(FRotator(0.f,BaseTurnRate*DeltaTime,0.f));
	}

}

void APickUp::Destroyed()
{
	Super::Destroyed();
	if(IsValid(PickUpSound) )
	{
		UGameplayStatics::PlaySoundAtLocation(this,PickUpSound,GetActorLocation());
	}
	
	if(PickUpEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,PickUpEffect,GetActorLocation()
			,GetActorRotation());
	}
}

