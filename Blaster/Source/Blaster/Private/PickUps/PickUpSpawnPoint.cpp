// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/PickUpSpawnPoint.h"

#include "PickUps/PickUp.h"


APickUpSpawnPoint::APickUpSpawnPoint()
{
 
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

}


void APickUpSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	StartPickUPTimer();
}

void APickUpSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUpSpawnPoint::SpawnPickup()
{
	int32 NumPickUPClases = PickUpClasses.Num();
	if(NumPickUPClases>0)
	{
		int32 Selection = FMath::RandRange(0,NumPickUPClases-1);
		SpawnPickUp = GetWorld()->SpawnActor<APickUp>(PickUpClasses[Selection],GetActorTransform());
		if(HasAuthority() && SpawnPickUp)
		{
			SpawnPickUp->OnDestroyed.AddDynamic(this,&APickUpSpawnPoint::StartPickUPTimer);
		}
	}
}

void APickUpSpawnPoint::SpawnPickUpTimerFInish()
{
	if(HasAuthority())
	{
		SpawnPickup();
	}
}

void APickUpSpawnPoint::StartPickUPTimer(AActor* Actor )
{
	const float SpawnTime = FMath::FRandRange(SpawnPickUPTimeMin,SpawnPickUPTimeMax);
	GetWorld()->GetTimerManager().SetTimer(SpawnPickUpTimer,this,&APickUpSpawnPoint::SpawnPickUpTimerFInish,SpawnTime);
}




