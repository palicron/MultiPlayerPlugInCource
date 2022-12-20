// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/LagCompensationComponent.h"

#include "Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"


ULagCompensationComponent::ULagCompensationComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}


void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwner()):Character;
	if(Character)
	{
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
	bool bReturn = HitCharacter == nullptr ||
		HitCharacter->GetLagCompensation() == nullptr ||
			HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
				HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;

	if(bReturn) return FServerSideRewindResult();

	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;
	
	//Frame Histoy of the Character
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTIme = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;

	if(OldestHistoryTIme>HitTime)
	{
		return FServerSideRewindResult();
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

	return ConfirmHit(FrameToCheck,HitCharacter,TraceStart,HitLocation);
}
