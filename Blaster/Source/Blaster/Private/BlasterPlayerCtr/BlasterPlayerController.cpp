// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerCtr/BlasterPlayerController.h"

#include "Character/BlasterCharacter.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "GameMode/BlasterGameMode.h"
#include "GameState/BlasterGameState.h"
#include "HUD/Annoucement.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/BlasterPlayerState.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	BlasterHUD =  Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();


}


void ABlasterPlayerController::CheckPing(float DeltaSeconds)
{
	HighPingRunningTime += DeltaSeconds;
	if(HighPingRunningTime>CheckPingFrequency)
	{
		PlayerState = PlayerState ==nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if(PlayerState)
		{
			if((PlayerState->GetPing() * 4)> HighPingThresHold ) //Ping is compress it ping/4
			{
				HighPingWarning();
				AnimationPingTime = 0.f;
			}
		}
		HighPingRunningTime = 0.f;
	}

	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HigPingAnim &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HigPingAnim))
	{
		AnimationPingTime += DeltaSeconds;
		if(AnimationPingTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	SetHUDTime();

	CheckTimeSync(DeltaSeconds);

	PollInit();

	CheckPing(DeltaSeconds);
}


void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;

	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HealthBar
		&& BlasterHUD->CharacterOverlay->HealthText)
	{
		const float HealthPercent = Health/MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealtText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Health),FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealtText));
	}
	else
	{
		BInitHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;

	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ShieldBar
		&& BlasterHUD->CharacterOverlay->ShieldText)
	{
		const float ShieldPercent = Shield/MaxShield;
		BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Shield),FMath::CeilToInt(MaxShield));
		BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		BInitShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}


void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if(BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(),BlasterCharacter->GetMaxHealth());
	}
}



void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoreAmount)
	{
		const FString ScorText = FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScorText));
	}
	else
	{
		BInitScore = true;
		HUDScore= Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmount)
	{
		const FString ScoredefeatText = FString::Printf(TEXT("%d"),Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(ScoredefeatText));
	}
	else
	{
		BInitDefeats = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		const FString WeaponAmmoText = FString::Printf(TEXT("%d"),Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
	else
	{
		BInitWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		const FString CarriedAmmoText = FString::Printf(TEXT("%d"),Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
	else
	{
		BInitCarryAmmo = true;
		HUDCarryAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDMatchCountDown(float CountDownTIme)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->MatchCountDownText)
	{
		if(CountDownTIme<0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText());
			return;
		}
		const int32 Minutes = FMath::FloorToInt(CountDownTIme/60.f);
		int32 Seconds = CountDownTIme - Minutes* 60;
		FString CountDownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHUD->CharacterOverlay->MatchCountDownText->SetText(FText::FromString(CountDownText));
	}
}

void ABlasterPlayerController::SetHUDAnnoucementCountDown(float CountDownTIme)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->AnnouncementOverlay && BlasterHUD->AnnouncementOverlay->WarmUpTime)
	{
		if(CountDownTIme<0.f)
		{
			BlasterHUD->AnnouncementOverlay->WarmUpTime->SetText(FText());
			return;
		}
		const int32 Minutes = FMath::FloorToInt(CountDownTIme/60.f);
		int32 Seconds = CountDownTIme - Minutes* 60;
		FString CountDownText = FString::Printf(TEXT("%02d:%02d"),Minutes,Seconds);
		BlasterHUD->AnnouncementOverlay->WarmUpTime->SetText(FText::FromString(CountDownText));
		
	}
}

void ABlasterPlayerController::SetHUDGrenades(int32 Grenades)
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->GrenadesText)
	{
		const FString CarriedGrenadesText = FString::Printf(TEXT("%d"),Grenades);
		BlasterHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(CarriedGrenadesText));
	}
	else
	{
		BInitDGrenades = true;
		HUDGrenades = Grenades;
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	
	if(MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
 
	}
	else if(MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() +  LevelStartingTime;
	}
	else if(MatchState == MatchState::Cooldown)
	{
		TimeLeft =CoolDownTime +  WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
			
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if(HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)):BlasterGameMode;
		if(BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCooldownTime() + LevelStartingTime); 
		}
	}

	if(CdInt != SecondsLeft)
	{
		if(MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnoucementCountDown(TimeLeft);
		}
		if(MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountDown(TimeLeft);
		}
		
	 
	}
	CdInt = SecondsLeft;
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	const float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest,ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceiveClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceiveClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if(HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}

}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	
	if(MatchState==MatchState::InProgress)
	{
		HanldeMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HanldeCooldown();
	}
	
}



void ABlasterPlayerController::OnRep_MatchState()
{
	if(MatchState==MatchState::InProgress)
	{
		HanldeMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown )
	{
		HanldeCooldown();
	}
}

void ABlasterPlayerController::CheckTimeSync(float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;
	if(IsLocalController() && TimeSyncRunningTime>TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}
void ABlasterPlayerController::PollInit()
{
	if(CharacterOverlay == nullptr)
	{
		if(BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if(CharacterOverlay)
			{
				if(BInitHealth ) SetHUDHealth(HUDHealth,HUDMaxHealth);
				if(BInitShield )SetHUDShield(HUDShield,HUDMaxShield);
				if(BInitScore ) SetHUDScore(HUDScore);
				if(BInitDefeats )SetHUDDefeats(HUDDefeats);
				if(BInitCarryAmmo) SetHUDCarriedAmmo(HUDCarryAmmo);
				if(BInitWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if(BlasterCharacter && BlasterCharacter->GetCombat())
				{
					if(BInitDGrenades ) SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
				}
			
			}
		}
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;

	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HightPing
		&& BlasterHUD->CharacterOverlay->HigPingAnim)
	{
		BlasterHUD->CharacterOverlay->HightPing->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->HigPingAnim,0.f,5);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;

	if(BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HightPing
		&& BlasterHUD->CharacterOverlay->HigPingAnim)
	{
		BlasterHUD->CharacterOverlay->HightPing->SetOpacity(0.f);
		if(BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HigPingAnim))
		{
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HigPingAnim);
		}
		
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch,float Warmup,float Match,float CoolDown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CoolDownTime = CoolDown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if(BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if(GameMode)
	{
		WarmupTime = GameMode->WarmUpTime;
		MatchTime = GameMode->MatchTime;
		CoolDownTime = GameMode->CoolDownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState,WarmupTime,MatchTime,CoolDownTime,LevelStartingTime);

	}
}

void ABlasterPlayerController::HanldeMatchHasStarted()
{

		BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
		if(BlasterHUD)
		{
			//BlasterHUD->AddCharacterOverlay();
			if(BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();
			if(BlasterHUD->AnnouncementOverlay)
			{
				BlasterHUD->AnnouncementOverlay->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	
}

void ABlasterPlayerController::HanldeCooldown()
{
	BlasterHUD = BlasterHUD==nullptr?Cast<ABlasterHUD>(GetHUD()):BlasterHUD;
	if(BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		
		if(BlasterHUD->AnnouncementOverlay && BlasterHUD->AnnouncementOverlay->AnnouncemntText
			&& BlasterHUD->AnnouncementOverlay->InfoText)
		{
			BlasterHUD->AnnouncementOverlay->SetVisibility(ESlateVisibility::Visible);
			FString AnouncementTExt("New Match Starts In:");
			BlasterHUD->AnnouncementOverlay->AnnouncemntText->SetText(FText::FromString(AnouncementTExt));
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if(BlasterGameState && BlasterPlayerState)
			{
				FString InfoText("");
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				if(TopPlayers.Num()==0)
				{
					InfoText = FString("No Winner.");
				}
				else if(TopPlayers.Num()==1 && TopPlayers[0] == BlasterPlayerState)
				{
					InfoText = FString("Yo Are the Winner.");
				}
				else if(TopPlayers.Num()==1 )
				{
					InfoText = FString::Printf(TEXT("Winner: \n%s"),*TopPlayers[0]->GetPlayerName());
				}
				else
				{
					InfoText = FString("Winnig Players:");

					for (auto TiedPlayer : TopPlayers)
					{
						InfoText.Append(FString::Printf(TEXT("%s \n"),*TiedPlayer->GetPlayerName()));
					}
				}

				
				BlasterHUD->AnnouncementOverlay->InfoText->SetText(FText::FromString(InfoText));
			}
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if(BlasterCharacter && BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisableGamePlay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController,MatchState);
}
