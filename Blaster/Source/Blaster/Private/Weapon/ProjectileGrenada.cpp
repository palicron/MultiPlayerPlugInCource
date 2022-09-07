// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileGrenada.h"

AProjectileGrenada::AProjectileGrenada()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
}

void AProjectileGrenada::BeginPlay()
{
	AActor::BeginPlay();

	StartDestroyTimer();

	void SpawnTrailSystem();
	
	void StartDestroyTimer();

	
}
