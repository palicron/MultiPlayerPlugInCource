// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/PickUp.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


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

}

void APickUp::Destroyed()
{
	Super::Destroyed();
	if(IsValid(PickUpSound) )
	{
		UGameplayStatics::PlaySoundAtLocation(this,PickUpSound,GetActorLocation());
	}
}

