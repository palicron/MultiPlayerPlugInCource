// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"

#include "Blaster/Blaster.h"
#include "Blaster/Weapon/Weapon.h"
#include "BlasterComponents/BuffComponent.h"
#include "BlasterComponents/LagCompensationComponent.h"
#include "BlasterPlayerCtr/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Character/BlasterAnimInstance.h"
#include "Components/BoxComponent.h"
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

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("Buff COmponent"));
	Buff->SetIsReplicated(true);

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

	AttachGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attache Grenade"));
	AttachGrenade->SetupAttachment(GetMesh(),FName("GrenadeSocket"));
	AttachGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("Lag Compensation Component"));
	
	/**
	* Server Side rewind
	*/

	Head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	Head->SetupAttachment(GetMesh(),FName("head"));
	Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("head"),Head);
	
	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(),FName("pelvis"));
	pelvis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("pelvis"),pelvis);
	
	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(),FName("spine_02"));
	spine_02->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_02"),spine_02);
	
	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(),FName("spine_03"));
	spine_03->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("spine_03"),spine_03);
	
	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(),FName("upperarm_l"));
	upperarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_l"),upperarm_l);
	
	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(),FName("upperarm_r"));
	upperarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("upperarm_r"),upperarm_r);
	
	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(),FName("lowerarm_l"));
	lowerarm_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_l"),lowerarm_l);
	
	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(),FName("lowerarm_r"));
	lowerarm_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("lowerarm_r"),lowerarm_r);
	
	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(),FName("hand_l"));
	hand_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_l"),hand_l);
	
	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(),FName("hand_r"));
	hand_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("hand_r"),hand_r);
	
	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(),FName("backpack"));
	backpack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("backpack"),backpack);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(),FName("backpack"));
	blanket->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("blanket"),blanket);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(),FName("thigh_l"));
	thigh_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_l"),thigh_l);
	
	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(),FName("thigh_r"));
	thigh_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("thigh_r"),thigh_r);
	
	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(),FName("calf_l"));
	calf_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_l"),calf_l);
	
	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(),FName("calf_r"));
	calf_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("calf_r"),calf_r);
	
	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(),FName("foot_l"));
	foot_l->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_l"),foot_l);
	
	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(),FName("foot_r"));
	foot_r->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCollisionBoxes.Add(FName("foot_r"),foot_r);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter,OverLappingWeapon,COND_OwnerOnly);
    DOREPLIFETIME(ABlasterCharacter,Health);
	DOREPLIFETIME(ABlasterCharacter,Shield);
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

void ABlasterCharacter::UpdateHudShield()
{
	BlastertPlayerCtr = BlastertPlayerCtr==nullptr ?Cast<ABlasterPlayerController>(Controller):BlastertPlayerCtr;
	if(BlastertPlayerCtr)
	{
		BlastertPlayerCtr->SetHUDShield(Shield,MaxShield);
	}
}

void ABlasterCharacter::UpdateHUDAmo()
{
	BlastertPlayerCtr = BlastertPlayerCtr==nullptr ?Cast<ABlasterPlayerController>(Controller):BlastertPlayerCtr;
	if(BlastertPlayerCtr && Combat && Combat->EquippedWeapon)
	{
		BlastertPlayerCtr->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		BlastertPlayerCtr->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
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

	SpawnDefaultWeapon();

	UpdateHudHealth();
	UpdateHudShield();
	UpdateHUDAmo();
	
	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this,&ABlasterCharacter::ReceiveDamage);
	}
	if(AttachGrenade)
	{
		AttachGrenade->SetVisibility(false);
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

	PlayerInputComponent->BindAction("ThrowGrenade",IE_Pressed,this,&ABlasterCharacter::GrenadeButtonPress);
	
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
	if(Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeed(GetCharacterMovement()->MaxWalkSpeed,GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if(LagCompensation)
	{
		LagCompensation->Character = this;
		if(Controller)
		{
			LagCompensation->Controller = Cast<ABlasterPlayerController>(Controller);
		}
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
		case EWeaponType::EWT_AssaultRifle: 
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubMachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_ShotGun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
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

void ABlasterCharacter::PlayTrowGrenadeMontage()
{
	UAnimInstance* AnimInstance= GetMesh()->GetAnimInstance();
	if(AnimInstance && TrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(TrowGrenadeMontage);
	}
}

void ABlasterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if(Weapon==nullptr) return;
	if(Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void ABlasterCharacter::DropOrDestroyWeapons()
{
	if(Combat)
	{
		if(Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if(Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}
}

void ABlasterCharacter::Elim()
{
	DropOrDestroyWeapons();
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
	if(IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		ShowSniperScopeWidget(false);
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

void ABlasterCharacter::GrenadeButtonPress()
{
	if(Combat)
	{
		Combat->ThrowGrenade();
	}
}

void ABlasterCharacter::ReceiveDamage(AActor* DamgeActor, float Damage, const UDamageType* DamageType,
                                      AController* InstigatorController, AActor* DamageCauser)
{

	if(bElim) return;

	float DamageToHealth = Damage;

	if(Shield>0.f)
	{
		if(Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield -Damage,0.f,MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			Shield = 0.f;
			DamageToHealth -= FMath::Clamp(DamageToHealth-Shield,0.f,Damage);
		
		}
		UpdateHudShield();
	}
	
	Health = FMath::Clamp(Health-DamageToHealth,0.f,MaxHealth);

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
		ServerEquipButtonPressed();
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

void ABlasterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHudHealth();
	if(Health<LastHealth)
	{
		PlayHitReactMontage();
	}
	
}

void ABlasterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHudShield();
	if(Shield<LastShield)
	{
		PlayHitReactMontage();
	}
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

void ABlasterCharacter::SpawnDefaultWeapon()
{
	ABlasterGameMode* BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if(BlasterGameMode && World && !bElim && DefaultWeapon)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeapon);
		
		StartingWeapon->bDestroyWeapon = true;

		if(Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
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
		if(OverLappingWeapon)
		{
			Combat->EquipWeapon(OverLappingWeapon);
		}
		else if(Combat->ShouldSwapWeapons())
		{
			Combat->SawpWeapons();
		}
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

bool ABlasterCharacter::IsLocallyreloaded()
{
	if(Combat == nullptr) return false;
	 return Combat->bLocallyReloading;
}


