// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"

#include "Blaster/Blaster.h"
#include "Blaster/Weapon/Weapon.h"
#include "BlasterPlayerCtr/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Character/BlasterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Sound/SoundCue.h"


ABlasterCharacter::ABlasterCharacter()
{
 
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidegt"));
	OverHeadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera,ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	GetCharacterMovement()->RotationRate = FRotator(0.f,0.f,800.f);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolbeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DessolveTImeLineComponent"));
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter,OverLappingWeapon,COND_OwnerOnly);
    DOREPLIFETIME(ABlasterCharacter,Health);
	DOREPLIFETIME(ABlasterCharacter,bDisableGamePlay);
}


void ABlasterCharacter::UpdateHudHealth()
{
	BlastertPlayerCtr = BlastertPlayerCtr==nullptr ?Cast<ABlasterPlayerController>(Controller):BlastertPlayerCtr;
	if(BlastertPlayerCtr)
	{
		BlastertPlayerCtr->SetHUDHealth(Health,MaxHealth);
	}
}

void ABlasterCharacter::PollInit()
{
	if(BlasterPlayerState == nullptr)
	{
		BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
		if(BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();
	if(ElimBotComponet)
	{
		ElimBotComponet->DestroyComponent();
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHudHealth();

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this,&ABlasterCharacter::ReceiveDamage);
	}
	
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip",IE_Pressed,this,&ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim",IE_Pressed,this,&ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim",IE_Released,this,&ABlasterCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ABlasterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire",IE_Released,this,&ABlasterCharacter::FireButtonRealese);

	PlayerInputComponent->BindAction("Reload",IE_Pressed,this,&ABlasterCharacter::ReloadButtonPress);
	
	PlayerInputComponent->BindAxis("MoveForward",this,&ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn",this,&ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp",this,&ABlasterCharacter::LookUp);

	
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->Character = this;
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if(Combat==nullptr || Combat->EquippedWeapon==nullptr) return;

	UAnimInstance* AnimInstance= GetMesh()->GetAnimInstance();
	if(AnimInstance && FireWeaponeMontage)
	{
		AnimInstance->Montage_Play(FireWeaponeMontage);
		FName SectionName;
		SectionName = bAiming?FName("AimFire"):FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayReLoadMontage()
{
	if(Combat==nullptr || Combat->EquippedWeapon==nullptr) return;

	UAnimInstance* AnimInstance= GetMesh()->GetAnimInstance();
	if(AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle: break;
			SectionName = FName("Rifle");
		case EWeaponType::EWT_MAX: break;
		default: ;
		}
	
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PLayElimMontage()
{


	UAnimInstance* AnimInstance= GetMesh()->GetAnimInstance();
	if(AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::Elim()
{
	if(Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	MulticasElim();
	GetWorldTimerManager().SetTimer(EliminTimer,this,&ABlasterCharacter::ElimTimerFinish,ElimDelay);
}

void ABlasterCharacter::MulticasElim_Implementation()
{
	if(BlastertPlayerCtr)
	{
		BlastertPlayerCtr->SetHUDWeaponAmmo(0);
	}
	bElim = true;
	PLayElimMontage();

	if(DesolveMaterialIntance)
	{
		DynamicDesolveMaterialIntance = UMaterialInstanceDynamic::Create(DesolveMaterialIntance,this);
		GetMesh()->SetMaterial(0,DynamicDesolveMaterialIntance);
		DynamicDesolveMaterialIntance->SetScalarParameterValue(TEXT("Disolve"),-0.55f);
		DynamicDesolveMaterialIntance->SetScalarParameterValue(TEXT("Glow"),150.f);
	}
	StartDissolve();

	//Disable Character movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGamePlay = true;
	if(Combat )
	{
		Combat->FireButtonPressed(false);
	}
	//Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Spawn Elim Bot
	if(ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X,GetActorLocation().Y,GetActorLocation().Z+200.f);
		ElimBotComponet = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ElimBotEffect,ElimBotSpawnPoint,GetActorRotation());
		
	}
	if(ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this,ElimBotSound,GetActorLocation());
	}
}

void ABlasterCharacter::ElimTimerFinish()
{
	ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
	if(BlasterGameMode)
	{
		BlasterGameMode->RequestRespawn(this,Controller);
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovement = 0.f;
}


void ABlasterCharacter::PlayHitReactMontage()
{
	if(Combat==nullptr || Combat->EquippedWeapon==nullptr) return;

	UAnimInstance* AnimInstance= GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamgeActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health-Damage,0.f,MaxHealth);

	PlayHitReactMontage();
	UpdateHudHealth();

	if(Health==0.f)
	{
		ABlasterGameMode* BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
		if(BlasterGameMode)
		{
			BlastertPlayerCtr = BlastertPlayerCtr==nullptr?Cast<ABlasterPlayerController>(Controller):BlastertPlayerCtr;
			ABlasterPlayerController* attackerCtr = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGameMode->PlayerEliminated(this,BlastertPlayerCtr,attackerCtr);
		}
	}

}




void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
    RotateInPlace(DeltaTime);
	
	HideCameraIfCharacterClose();
	PollInit();
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if(bDisableGamePlay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	if(GetLocalRole()>ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffSet(DeltaTime);
	}
	else
	{
		TimeSinceLastMovement += DeltaTime;
		if(TimeSinceLastMovement> 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		Calculate_Pitch();
	}
}

void ABlasterCharacter::MoveForward(float Value)
{
	if(bDisableGamePlay) return;
	if(Controller!=nullptr && Value!=0.0f )
	{
		const FRotator YawRotation(0.0f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction,Value);
	}
}

void ABlasterCharacter::MoveRight(float Value)
{
	if(bDisableGamePlay) return;
	if(Controller!=nullptr && Value!=0.0f)
	{
		const FRotator YawRotation(0.0f,Controller->GetControlRotation().Yaw,0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction,Value);
	}
}

void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipButtonPressed()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		if(HasAuthority())
		{
			Combat->EquipWeapon(OverLappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if(bDisableGamePlay) return;
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}

}

void ABlasterCharacter::ReloadButtonPress()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->Reload();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::Calculate_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch>90.f && !IsLocallyControlled())
	{
		//Map pithc from [270,360) to [-90,0)
		FVector2d InRange(270.f,360.f);
		FVector2d OutRange(-90.f,0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange,OutRange,AO_Pitch);
	}
}

void ABlasterCharacter::AimOffSet(float DeltaTime)
{
	if(Combat && Combat->EquippedWeapon ==nullptr) return;

	
   float Speed = CalculateSpeed();

	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if(Speed==0.0f && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation,StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace ==ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed>0.f || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	Calculate_Pitch();
}

void ABlasterCharacter::SimProxiesTurn()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	float speed = CalculateSpeed();
	bRotateRootBone = false;
	if(speed>0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}


	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
    ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation,ProxyRotationLastFrame).Yaw;

	if(FMath::Abs(ProxyYaw)>TurnThreshold)
	{
		if(ProxyYaw>TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw<TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::Jump()
{
	if(bDisableGamePlay) return;
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}

}

void ABlasterCharacter::FireButtonPressed()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABlasterCharacter::FireButtonRealese()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw>90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw<-90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if(TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw,0.f,DeltaTime,5.f);
		AO_Yaw = InterpAO_Yaw;
		if(FMath::Abs(AO_Yaw)<15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f,GetBaseAimRotation().Yaw,0.f);
		}
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if(!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation()-GetActorLocation()).Size()<CameraTreshold)
	{
		GetMesh()->SetVisibility(false);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z =0.f;
	return Velocity.Size();
}

void ABlasterCharacter::OnRep_Health()
{
	PlayHitReactMontage();
	UpdateHudHealth();
}


void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if(DynamicDesolveMaterialIntance)
	{
		DynamicDesolveMaterialIntance->SetScalarParameterValue(TEXT("Disolve"),DissolveValue);
	}
}

void ABlasterCharacter::StartDissolve()
{
	DissolverTrack.BindDynamic(this,&ABlasterCharacter::UpdateDissolveMaterial);
	if(DisolveCurve && DissolbeTimeline)
	{
		DissolbeTimeline->AddInterpFloat(DisolveCurve,DissolverTrack);
		DissolbeTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* weapon)
{
	if(OverLappingWeapon)
	{
		OverLappingWeapon->ShowPickUpWidget(false);
	}
	OverLappingWeapon = weapon;
	if(IsLocallyControlled())
	{
		if(OverLappingWeapon)
		{
			OverLappingWeapon->ShowPickUpWidget(true);
		}
	}
}


void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if(OverLappingWeapon)
	{
		OverLappingWeapon->ShowPickUpWidget(true);
	}
	else if(LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if(Combat)
	{
		Combat->EquipWeapon(OverLappingWeapon);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapone() const
{
	if(Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if(Combat == nullptr) return ECombatState::ECS_Max;

	return  Combat->CombatState;
}


