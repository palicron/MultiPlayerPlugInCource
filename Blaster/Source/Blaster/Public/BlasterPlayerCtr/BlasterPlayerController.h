// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    void SetHUDHealth(float Health,float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTIme);
	virtual void OnPossess(APawn* InPawn) override;
	virtual float GetServerTime();
	virtual void ReceivedPlayer() override;

	void OnMatchStateSet(FName State);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual  void BeginPlay() override;
	
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
	
private:
	class ABlasterHUD* BlasterHUD;

	float MatchTime = 120.f;
	uint32 CdInt = 0;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	bool BInitializeCharacterOverlay = false;
	
	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
};
