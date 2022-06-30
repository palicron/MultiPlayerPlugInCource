// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProyectileBulled.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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
