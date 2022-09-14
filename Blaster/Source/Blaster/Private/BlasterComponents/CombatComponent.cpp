// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/CombatComponent.h"

#include "Blaster/Weapon/Weapon.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "blaster/Public/BlasterPlayerCtr/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;


}



void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if(Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if(Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
			
	}

	
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if(Character==nullptr || EquippedWeapon == nullptr) return;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming?AimWalkSpeed : BaseWalkSpeed;
	}
	if(Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType()==EWeaponType::EWT_SniperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
	
}
void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if(Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming?AimWalkSpeed : BaseWalkSpeed;
	}	
}
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponeState(EWeaponState::EWS_Equipped);
		AttachActorToRightHand(EquippedWeapon);
		PlayEquippedWeaponSound();
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

	}
}

void UCombatComponent::Fire()
{
	if(CanFire())
	{
		
		ServerFire(HitTarget);
		if(EquippedWeapon)
		{
			CrossHairShootingFactor = 0.75f;
		}
		StartFireTimer();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MultiCastFire(TraceHitTarget);
}

void UCombatComponent::FireButtonPressed(bool ButtonPress)
{
	bFireButtonPress=ButtonPress;
    
	if(bFireButtonPress)
	{
		Fire();
	}
	

}

void UCombatComponent::ShotGunShellReload()
{
	if(Character && Character->HasAuthority())
	{
			UpdateShotgunAmmoValues();
	}

}


void UCombatComponent::StartFireTimer()
{
	if(EquippedWeapon == nullptr || Character == nullptr) return;
	bCanFire = false;
	Character->GetWorldTimerManager().SetTimer(FireTimer,this,&UCombatComponent::FireTimerFinish,EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimerFinish()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if(bFireButtonPress && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	ReloadEmptyWeapone();
}

bool UCombatComponent::CanFire() const
{
	if(EquippedWeapon == nullptr)
	{
		return  false;
	}
	if(!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Reloading
		&& EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun)
	{
		return true;
	}
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::TraceUnderCrossHair(FHitResult& TraceHitResult)
{
	FVector2d ViewPortSize;
	if(GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	FVector2d CrossHairLocation(ViewPortSize.X/2.f,ViewPortSize.Y/2.f);
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this,0),
		CrossHairLocation,CrossHairWorldPosition,CrossHairWorldDirection);
	if(bScreenToWorld)
	{
		FVector Start = CrossHairWorldPosition;
		if(Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation()-Start).Size();
			Start += CrossHairWorldDirection * (DistanceToCharacter + 50.f);
		}
		FVector End = Start + CrossHairWorldDirection * TRACE_LENGHT;

		if(!GetWorld()->LineTraceSingleByChannel(TraceHitResult,Start,End,ECollisionChannel::ECC_Visibility))
		{
			FHitResult NohitResult;
			NohitResult.ImpactPoint = End;
			TraceHitResult = NohitResult;
		}
		if(TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrossHairsInterface>())
		{
			HUDPackage.CrossHairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrossHairsColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::MultiCastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if(EquippedWeapon==nullptr) return;
	if(Character && CombatState==ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType()==EWeaponType::EWT_ShotGun)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if(Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}



void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	if (Character && Character->IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
		FHitResult HitResult;
		TraceUnderCrossHair(HitResult);
		HitTarget = HitResult.ImpactPoint;
		InterpFOV(DeltaTime);
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(Character==nullptr || Character->Controller== nullptr)return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()):BlasterHUD;
		if(BlasterHUD)
		{
		
			if(EquippedWeapon)
			{
			
				HUDPackage.CrossHairCenter= EquippedWeapon->CrossHairCenter;
				HUDPackage.CrossHairDown= EquippedWeapon->CrossHairDown;
				HUDPackage.CrossHairLeft= EquippedWeapon->CrossHairLeft;
				HUDPackage.CrossHairRight= EquippedWeapon->CrossHairRight;
				HUDPackage.CrossHairUp= EquippedWeapon->CrossHairUp;
				
			}
			else
			{
				HUDPackage.CrossHairCenter= nullptr;
				HUDPackage.CrossHairDown= nullptr;
				HUDPackage.CrossHairLeft= nullptr;
				HUDPackage.CrossHairRight= nullptr;
				HUDPackage.CrossHairUp= nullptr;
				
			}
			//Calculate CrossHair Spread
			FVector2d WalkSpeedRange(0.f,Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2d VelocityMultiplayerRange(0.f,1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			CrossHairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange,VelocityMultiplayerRange,Velocity.Size());
			if(Character->GetCharacterMovement()->IsFalling())
			{
				CroosHairInAirFactor = FMath::FInterpTo(CroosHairInAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CroosHairInAirFactor = FMath::FInterpTo(CroosHairInAirFactor,0.f,DeltaTime,30.f);
			}
			if(bAiming)
			{
				CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor,0.58f,DeltaTime,30.f);
			}
			else
			{
				CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor,0.f,DeltaTime,30.f);
			}
			CrossHairShootingFactor =  FMath::FInterpTo(CrossHairShootingFactor,0.f,DeltaTime,40.f);
			HUDPackage.CrossHairSpread = 0.5f + CrossHairVelocityFactor + CroosHairInAirFactor
			+ CrossHairShootingFactor - CrossHairAimFactor;
			BlasterHUD->SetHUDPackage(HUDPackage);
		
		}
	}
}



void UCombatComponent::InterpFOV(float DeltaTime)
{
	if(EquippedWeapon == nullptr) return;

	if(bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,EquippedWeapon->GetZoomedFOV(),DeltaTime,EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV,DefaultFOV,DeltaTime,ZoomInterpSeed);
	}
	if(Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}


void UCombatComponent::DropEquippedWeapon()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UCombatComponent::UpdateCarryAmmo()
{
	if(EquippedWeapon == nullptr) return;
	
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	
	Controller = Controller ==nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
	
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::PlayEquippedWeaponSound()
{
	if (EquippedWeapon && Character)
	{
	
		if(EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this,EquippedWeapon->EquipSound,Character->GetActorLocation());
		}
		if(EquippedWeapon->IsEmpty())
		{
			Reload();
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::ReloadEmptyWeapone()
{
	if (EquippedWeapon && Character)
	{
		if(EquippedWeapon->IsEmpty())
		{
			Reload();
		}
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip==nullptr) return;
	if(CombatState != ECombatState::ECS_Unoccupied) return;
	
	DropEquippedWeapon();

	EquippedWeapon = WeaponToEquip;

	EquippedWeapon->SetWeaponeState(EWeaponState::EWS_Equipped);
	
	AttachActorToRightHand(EquippedWeapon);
	
	EquippedWeapon->SetOwner(Character);
	
	EquippedWeapon->SetHUDAmmo();
	
	UpdateCarryAmmo();

	PlayEquippedWeaponSound();

	ReloadEmptyWeapone();
	

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	

}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if(Character==nullptr || Character->GetMesh()==nullptr || ActorToAttach == nullptr)
	{
		return;
	}
	const USkeletalMeshSocket* HandSocket =   Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if(HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach,Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	if(Character==nullptr || Character->GetMesh()==nullptr || ActorToAttach == nullptr || EquippedWeapon == nullptr)
	{
		return;
	}
	bool bUsePistolSocket = EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SubMachineGun;
	FName SocketName = bUsePistolSocket ? FName("PistolSocket"):FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket =   Character->GetMesh()->GetSocketByName(SocketName);
	if(HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach,Character->GetMesh());
	}
}

void UCombatComponent::Reload()
{
	if(CarriedAmmo>0 && CombatState  == ECombatState::ECS_Unoccupied )
	{
		ServerReload();
	}
}

void UCombatComponent::FinishReloading()
{
	if(Character == nullptr) return;
	if(Character->HasAuthority())
	{
		UpdateAmmoValues();
		CombatState = ECombatState::ECS_Unoccupied;
	}
	if(bFireButtonPress)
	{
		Fire();
	}
}

int32 UCombatComponent::AmountToreload()
{
	if(EquippedWeapon)
	{
		int32 RoomINMag = EquippedWeapon->GetMaxCapacity() - EquippedWeapon->GetAmmo();
		if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
		{
			int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
			int32 Least = FMath::Min(RoomINMag,AmountCarried);

			return FMath::Clamp(RoomINMag,0,Least);
		}
	}
	return 0;
}

void UCombatComponent::ServerReload_Implementation()
{
    if(Character==nullptr && EquippedWeapon == nullptr) return;


	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState) {
	case ECombatState::ECS_Unoccupied:
		if(bFireButtonPress)
		{
			Fire();
		}
		break;
	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if(Character && !Character->IsLocallyControlled())
		{
			Character->PlayTrowGrenadeMontage();
			AttachActorToLeftHand(EquippedWeapon);
		}
		break;
	case ECombatState::ECS_Max: break;
	default: ;
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if(Character==nullptr && EquippedWeapon == nullptr) return;
	
	int32 ReloadAmount = AmountToreload();
	
	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if(Character==nullptr && EquippedWeapon == nullptr) return;

	if(CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(-1);
	bCanFire = true;
	if(EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		// Jump to ShotgunEnd Section
		JumpToShotGunEnd();
	}
}



void UCombatComponent::JumpToShotGunEnd()
{
	UAnimInstance* AnimInstance= Character->GetMesh()->GetAnimInstance();
	if(AnimInstance && Character->GetReloadMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
	}
		
}

void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToRightHand(EquippedWeapon);
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller ==nullptr ? Cast<ABlasterPlayerController>(Character->Controller):Controller;
	if(Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	bool bJumpTOShotgunEnd = CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_ShotGun && CarriedAmmo == 0;
	if(bJumpTOShotgunEnd)
	{
		JumpToShotGunEnd();
	}
}


void UCombatComponent::HandleReload()
{
	Character->PlayReLoadMontage();
}

void UCombatComponent::ThrowGrenade()
{
	if(CombatState != ECombatState::ECS_Unoccupied) return;
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if(Character)
	{
		Character->PlayTrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
	}
	if(Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();
	}

}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if(Character )
	{
		Character->PlayTrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle,StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher,StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol,StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubMachineGun,StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_ShotGun,StartingShotGunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle,StartingSniperAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher,StartingGrenadeLauncherAmmo);
}



void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent,EquippedWeapon);
	DOREPLIFETIME(UCombatComponent,bAiming);
	DOREPLIFETIME(UCombatComponent,CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent,CarriedAmmo,COND_OwnerOnly);
	
}


