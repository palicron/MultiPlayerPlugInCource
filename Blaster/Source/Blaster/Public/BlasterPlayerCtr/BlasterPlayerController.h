// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelgate,bool,bPingTooHigh);
/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	float SingleTripTime = 0.f;

	FHighPingDelgate HighPingDelegate; 
	
    void SetHUDHealth(float Health,float MaxHealth);
	void SetHUDShield(float Shield,float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTIme);
	void SetHUDAnnoucementCountDown(float CountDownTIme);
	void SetHUDGrenades(int32 Grenades);
	virtual void OnPossess(APawn* InPawn) override;
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HanldeMatchHasStarted();
	void HanldeCooldown();
protected:
	virtual  void BeginPlay() override;
	void CheckPing(float DeltaSeconds);

	void CheckTimeSync(float DeltaSeconds);

	void SetHUDTime();

	virtual void Tick(float DeltaSeconds) override;
	
	/**
	 * Sync Time between client and server
	 */

	//Rquest server time passing client Times 
	UFUNCTION(Server,Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClientRequest,float TimeServerReceiveClientRequest);

	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnywhere,Category= "Time")
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.0f;

	void PollInit();
	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();
	UFUNCTION(Client,Reliable)
	void ClientJoinMidGame(FName StateOfMatch,float Warmup,float Match, float CoolDown, float StartingTime);

	void HighPingWarning();

	void StopHighPingWarning();
	
private:
	class ABlasterHUD* BlasterHUD;

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	float MatchTime = 0.f;
	float WarmupTime =0.f;
	float CoolDownTime = 0.f;
	float LevelStartingTime =0.f;
	uint32 CdInt = 0;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	
	bool BInitHealth = false;
	float HUDHealth;
	float HUDMaxHealth;
	bool BInitShield = false;
	float HUDShield;
	float HUDMaxShield;
	bool BInitScore = false;
	float HUDScore;
	bool BInitDefeats = false;
	int32 HUDDefeats;
	bool BInitDGrenades = false;
	int32 HUDGrenades;
	bool BInitCarryAmmo = false;
	float HUDCarryAmmo;
	bool BInitWeaponAmmo = false;
	float HUDWeaponAmmo;

	
	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;

	UFUNCTION(Server,Reliable)
	void ServerReportPingStatus(bool bHightPing);
	
	float AnimationPingTime = 0.f;
	
	UPROPERTY(EditAnywhere)
	float HighPingThresHold = 50.f;
};
