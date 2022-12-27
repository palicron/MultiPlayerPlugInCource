// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/LagCompensationComponent.h"

#include "Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Blaster/Weapon/Weapon.h"
#include "Kismet/GameplayStatics.h"


ULagCompensationComponent::ULagCompensationComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}




void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ULagCompensationComponent::SaveFramePackage()
{
	if(Character == nullptr || !Character->HasAuthority()) return;
	if(FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLenght = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLenght > MaxRecordTIme)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			
			HistoryLenght = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}

		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);

		//	ShowFramePackage(ThisFrame,FColor::Red);
		
	}
	
}


void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFramePackage();
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwner()):Character;
	if(Character)
	{
		Package.Character = Character;
		
		Package.Time = GetWorld()->GetTimeSeconds();
		
		for (const auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key,BoxInformation);
		}
	}
}


FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame,
	const FFramePackage& YoungerFrame, float HitTime)
{

	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp ((HitTime - OlderFrame.Time)/ Distance,0.f,1.f);

	FFramePackage InterFramePackage;
	
	InterFramePackage.Time = HitTime;
	
	for(const auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;
		
		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];

		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpBoxInfo;

		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location,YoungerBox.Location,1.f,InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation,YoungerBox.Rotation,1.f,InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;

		InterFramePackage.HitBoxInfo.Add(BoxInfoName,InterpBoxInfo);
	}

	return InterFramePackage;
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunServerSideRewind(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart
	,const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	TArray<FFramePackage> FramesToCheck;
	for (ABlasterCharacter* HitCharacter : HitCharacters)
	{
		FramesToCheck.Add(GetFrameToCheck(HitCharacter,HitTime));
	}

	return ShotgunConfirmHit(FramesToCheck,TraceStart,HitLocations);
}


FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunConfirmHit(const TArray<FFramePackage>& FramesPackages,
	const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& Hitlocations)
{
	FShotgunServerSideRewindResult ShotGunResult;

	TArray<FFramePackage> CurrentFrames;
	
	for (const FFramePackage& Frame : FramesPackages)
	{
		if(Frame.Character)
			return FShotgunServerSideRewindResult();
		
		FFramePackage CurrentFrame;
		
		CurrentFrame.Character = Frame.Character;
		
		CacheBoxPosition(Frame.Character,CurrentFrame);
	
		MoveBoxes(Frame.Character,Frame);

		EnableCharacterMeshCollision(Frame.Character,ECollisionEnabled::NoCollision);

		CurrentFrames.Add(CurrentFrame);

		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HeadBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECR_Block);
		
	}
	UWorld* World = GetWorld();
	for (const FVector_NetQuantize& Hitlocation : Hitlocations)
	{
		FHitResult ConfirmHitResult;
		
		const FVector TraceEnd = TraceStart + (Hitlocation - TraceStart) * 1.25f;

		if(World)
		{
			World->LineTraceSingleByChannel(ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility);
			
			ABlasterCharacter* Character = Cast<ABlasterCharacter>(ConfirmHitResult.GetActor());
			if(Character)
			{
				if(ShotGunResult.HeadShots.Contains(Character))
				{
					ShotGunResult.HeadShots[Character]++;
				}
				else
				{
					ShotGunResult.HeadShots.Emplace(Character,1);
				}
				
			}
		}
	}

	for (const FFramePackage& Frame : FramesPackages)
	{
		for (const auto& HitBoxPair : Frame.Character->HitCollisionBoxes)
		{
			if(HitBoxPair.Value)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
			}
			
			UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
			HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
		}
	}

	for (const FVector_NetQuantize& Hitlocation : Hitlocations)
	{
		FHitResult ConfirmHitResult;
		
		const FVector TraceEnd = TraceStart + (Hitlocation - TraceStart) * 1.25f;

		if(World)
		{
			World->LineTraceSingleByChannel(ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility);
			
			ABlasterCharacter* Character = Cast<ABlasterCharacter>(ConfirmHitResult.GetActor());
			if(Character)
			{
				if(ShotGunResult.BodyShots.Contains(Character))
				{
					ShotGunResult.BodyShots[Character]++;
				}
				else
				{
					ShotGunResult.BodyShots.Emplace(Character,1);
				}
				
			}
		}
	}

	for (const auto& CurrentFrame : CurrentFrames)
	{
		ResetHitBoxes(CurrentFrame.Character,CurrentFrame);
		EnableCharacterMeshCollision(CurrentFrame.Character,ECollisionEnabled::QueryAndPhysics);
	}
	
	return ShotGunResult;
	
}
FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package,
                                                              ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if(HitCharacter == nullptr) return FServerSideRewindResult();

	FFramePackage CurrentFrame;

	CacheBoxPosition(HitCharacter,CurrentFrame);
	
	MoveBoxes(HitCharacter,Package);

	EnableCharacterMeshCollision(HitCharacter,ECollisionEnabled::NoCollision);
	//Enable Collision For the HEad Firts

	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECR_Block);

	FHitResult ConfirmHitResult;

	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;

	UWorld* World = GetWorld();
	if(World)
	{
		World->LineTraceSingleByChannel(ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility);
		if(ConfirmHitResult.bBlockingHit)
		{
			ResetHitBoxes(HitCharacter,CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter,ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{true,true};
		}
		else
		{
			for (const auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
			{
				if(HitBoxPair.Value)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
				}

				World->LineTraceSingleByChannel(ConfirmHitResult,
					TraceStart,
						TraceEnd,
				ECC_Visibility);
				if(ConfirmHitResult.bBlockingHit)
				{
					ResetHitBoxes(HitCharacter,CurrentFrame);
					EnableCharacterMeshCollision(HitCharacter,ECollisionEnabled::QueryAndPhysics);
					return FServerSideRewindResult{true,false};
				}
		}
	}
	}
		ResetHitBoxes(HitCharacter,CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter,ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{false,false};
}

void ULagCompensationComponent::CacheBoxPosition(ABlasterCharacter* HitCharacter, FFramePackage& FramePackage)
{
	if(HitCharacter == nullptr) return;

	for (const auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInfo;

			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();

			FramePackage.HitBoxInfo.Add(HitBoxPair.Key,BoxInfo);
		}
	}
}

void ULagCompensationComponent::MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& OutFramePackage)
{
	if(HitCharacter == nullptr) return;
	for (const auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(OutFramePackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(OutFramePackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(OutFramePackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);

		}
	}
	
}

void ULagCompensationComponent::ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& OutFramePackage)
{
	if(HitCharacter == nullptr) return;
	for (const auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if(HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(OutFramePackage.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(OutFramePackage.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(OutFramePackage.HitBoxInfo[HitBoxPair.Key].BoxExtent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
		}
	}
}

void ULagCompensationComponent::EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter,
	ECollisionEnabled::Type CollisionEnable)
{
	if(HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnable);
	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color) const
{
	for (const auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,false,
			4.f);
		
	}
}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize& HitLocation, float HitTime)
{
	const FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter,HitTime);
	
	return ConfirmHit(FrameToCheck,HitCharacter,TraceStart,HitLocation);
}

void ULagCompensationComponent::ServerScoreRequest_Implementation(ABlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* DamageCauser)
{
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter,TraceStart,HitLocation,HitTime);

	if(Character && HitCharacter && DamageCauser && Confirm.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(HitCharacter,DamageCauser->GetDamage(),
			Character->Controller,DamageCauser,UDamageType::StaticClass());
	}
}
void ULagCompensationComponent::ShotgunServerScoreRequest_Implementation(
	const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart,
	const TArray<FVector_NetQuantize>& HitLocation, float HitTime, class AWeapon* DamageCauser)
{
	FShotgunServerSideRewindResult Confirm = ShotgunServerSideRewind(HitCharacters,TraceStart,HitLocation,HitTime);

	for (auto& HitCharacter : HitCharacters)
	{
		if(HitCharacter == nullptr || DamageCauser==nullptr) continue;
		float TotalDamage = 0.f;
		if(Confirm.HeadShots.Contains(HitCharacter))
		{
			const float HeadShotDamage = Confirm.HeadShots[HitCharacter] * DamageCauser->GetDamage();
			TotalDamage += HeadShotDamage;
		}
		if(Confirm.BodyShots.Contains(HitCharacter))
		{
			const float BodyShotDamage = Confirm.BodyShots[HitCharacter] * DamageCauser->GetDamage();
			TotalDamage += BodyShotDamage;
		}

		UGameplayStatics::ApplyDamage(HitCharacter,TotalDamage,
		Character->Controller,DamageCauser,UDamageType::StaticClass());
	}
}
FFramePackage ULagCompensationComponent::GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime)
{
	bool bReturn = HitCharacter == nullptr ||
	HitCharacter->GetLagCompensation() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
			HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;

	if(bReturn) return FFramePackage();

	FFramePackage FrameToCheck;
	
	FrameToCheck.Time = 0.f;
	
	bool bShouldInterpolate = true;
	
	//Frame Histoy of the Character
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTIme = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;

	if(OldestHistoryTIme>HitTime)
	{
		return FFramePackage();
	}
	
	if(OldestHistoryTIme == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}
	if(NewestHistoryTime<= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;

	while(Older->GetValue().Time>HitTime)
	{
		if(Older->GetNextNode() == nullptr) break;
		
		Older = Older->GetNextNode();
		
		if(Older->GetValue().Time>HitTime)
		{
			Younger = Older;
		}
	}
	
	if(Older->GetValue().Time == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}

	if(bShouldInterpolate)
	{
		FrameToCheck = InterpBetweenFrames(Older->GetValue(),Younger->GetValue(),HitTime);
	}
	return FrameToCheck;
}


