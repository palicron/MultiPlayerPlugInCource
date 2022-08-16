// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProyectileRocket.h"

#include "Kismet/GameplayStatics.h"

AProyectileRocket::AProyectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AProyectileRocket::OnHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* FiringPawn = GetInstigator();

	if(FiringPawn)
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
	Super::OnHit(MyComp, Other, OtherComp, NormalImpulse, Hit);
}
