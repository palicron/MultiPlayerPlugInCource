// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Public/Character/BlasterCharacter.h"
#include "BlasterPlayerCtr/BlasterPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Casing.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget= CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
}


void AWeapon::SetHUDAmmo()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if(BlasterOwnerCharacter)
	{
		BlasterOwnerController = BlasterOwnerController== nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if(BlasterOwnerController && HasAuthority())
		{
			BlasterOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if(WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this,&ThisClass::AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this,&AWeapon::OnSphereEndOverlarp);

	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}
}



void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon,WeaponState);
	
	DOREPLIFETIME_CONDITION(AWeapon,bUseServerSideRewind,COND_OwnerOnly);

}

void AWeapon::Fire(const FVector& HitTarget)
{
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation,false);
	}
	if(CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if(AmmoEjectSocket)
		{
			FTransform SocketTransform =  AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			
	
				UWorld* world = GetWorld();
				if(world)
				{
					world->SpawnActor<ACasing>(
						CasingClass,
						SocketTransform.GetLocation(),
						SocketTransform.GetRotation().Rotator()
						);
				}
			
		
		}
	}


	SpendRound();

}

void AWeapon::Dropped()
{
	SetWeaponeState(EWeaponState::EWS_Drop);
	FDetachmentTransformRules Detache(EDetachmentRule::KeepWorld,true);
	WeaponMesh->DetachFromComponent(Detache);
	SetOwner(nullptr);
	BlasterOwnerController = nullptr;
	BlasterOwnerCharacter = nullptr;
}



void AWeapon::OnSphereOverlap(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
                              int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter )
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
	
}

void AWeapon::OnSphereEndOverlarp(UPrimitiveComponent* Overlap, AActor* OtherActor, UPrimitiveComponent* otherComp,
	int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter )
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}




void AWeapon::SetWeaponeState(EWeaponState State)
{
	WeaponState = State;
	
	OnWeaponStateSet();
}

void AWeapon::OnPingToHigh(bool bPingToHigh)
{
	bUseServerSideRewind = !bPingToHigh;
}


void AWeapon::OnEquipped()
{
	ShowPickUpWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(WeaponType == EWeaponType::EWT_SubMachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	}
	EnableCustomDepth(false);

	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if(BlasterOwnerCharacter && bUseServerSideRewind)
	{
		BlasterOwnerController = BlasterOwnerController== nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if(BlasterOwnerController && HasAuthority() && !BlasterOwnerController->HighPingDelegate.IsBound())
		{
			BlasterOwnerController->HighPingDelegate.AddDynamic(this,&AWeapon::OnPingToHigh);
		}
	}
}

void AWeapon::OnDrop()
{
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if(BlasterOwnerCharacter && bUseServerSideRewind)
	{
		BlasterOwnerController = BlasterOwnerController== nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if(BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
		{
			BlasterOwnerController->HighPingDelegate.RemoveDynamic(this,&AWeapon::OnPingToHigh);
		}
	}
}

void AWeapon::OnEquippedSecondary()
{
	ShowPickUpWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(WeaponType == EWeaponType::EWT_SubMachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	}
	EnableCustomDepth(false);
		
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	if(BlasterOwnerCharacter && bUseServerSideRewind)
	{
		BlasterOwnerController = BlasterOwnerController== nullptr ? Cast<ABlasterPlayerController>(BlasterOwnerCharacter->Controller) : BlasterOwnerController;
		if(BlasterOwnerController && HasAuthority() && BlasterOwnerController->HighPingDelegate.IsBound())
		{
			BlasterOwnerController->HighPingDelegate.RemoveDynamic(this,&AWeapon::OnPingToHigh);
		}
	}
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial: break;
	case EWeaponState::EWS_Equipped:
	
		OnEquipped();
	
		break;
	case EWeaponState::EWS_Drop:
	
		OnDrop();
	
		break;
	case EWeaponState::EWS_EquippedSecondary:

		OnEquippedSecondary();
		
		break;
	case EWeaponState::EWS_MAX: break;
	default: ;
	}
}




void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::ShowPickUpWidget(bool BShowWidget)
{
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(BShowWidget);
	}
}



void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo-1,0,MagCapacity);
	SetHUDAmmo();
	if(HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if(HasAuthority()) return;
	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd,0,MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if(HasAuthority()) return;
	
	Ammo = FMath::Clamp(Ammo + AmmoToAdd,0,MagCapacity);
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()):BlasterOwnerCharacter;
	if(BlasterOwnerCharacter && BlasterOwnerCharacter->GetCombat() && IsFull())
	{
		BlasterOwnerCharacter->GetCombat()->JumpToShotGunEnd();
	}
	SetHUDAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if(Owner == nullptr)
	{
		BlasterOwnerCharacter = nullptr;
		BlasterOwnerController = nullptr;
	}
	else
	{
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ?  Cast<ABlasterCharacter>(Owner) : BlasterOwnerCharacter;
		if(BlasterOwnerCharacter && BlasterOwnerCharacter->GetEquippedWeapone() && BlasterOwnerCharacter->GetEquippedWeapone() == this)
		{
			SetHUDAmmo();
		}
		
	}
}


FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget) const
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if(MuzzleFlashSocket == nullptr) return FVector();
	
		
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector TraceStart  = SocketTransform.GetLocation();
		
	
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();

	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f,SphereRadius);

	const FVector EndLoc = SphereCenter + RandVec;

	const FVector ToEndLoc = EndLoc - TraceStart;
	

	const FVector NewVect = FVector(TraceStart + ToEndLoc * 80000.f / ToEndLoc.Size());


	return NewVect;
}
