// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"

// Sets default values
ACasing::ACasing()
{
 
	PrimaryActorTick.bCanEverTick = false;
   CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Casing Mesh"));
   SetRootComponent(CasingMesh);
}


void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
}



