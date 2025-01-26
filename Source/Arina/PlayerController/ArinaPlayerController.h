// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArinaPlayerController.generated.h"

class AArinaPlayerState;
class AArinaGameMode;
class AArinaHUD;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(const float& CurrentHealth, const float& MaxHealth);
	void SetHUDShield(const float& CurrentShield, const float& MaxShield);
	void SetHUDScore(const float& Score);
	void SetHUDDeaths(const int32& Defeats);
	void SetHUDWeaponAmmo(const int32& WeaponAmmo);
	void SetHUDCarryAmmo(const int32& CarryAmmo);
	void SetHUDWeaponType(const FString& WeaponType);
	void SetHUDMatchTimer(const float CountdownTime);
	void SetHUDAnnouncementTimer(const float CountdownTime);
	void SetHUDSniperScope(bool bIsAiming);
	void SetHUDGrenades(int32 Grenades);
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void DisplayKilledByMessage(const FString& KillerName);
	void CollapseKilledByMessage();

	UFUNCTION(Client, Reliable)
	void ClientCollapseKilledByMessage();

	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as early as possible

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	
protected:
	virtual void BeginPlay() override;
	void CheckTimeSync(float DeltaSeconds);
	void SetHUDTime();

	/**
	*	Sync time between client and server
	*/
	// Requests current server time, taking into account when client request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	// Returns the current server time to client based on ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);

	float ClientServerDelta = 0.f; // difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 3.f;
	float TimeSyncRunningTime = 0.f;
	
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);
	
private:
	UPROPERTY()
	AArinaHUD* ArinaHUD;

	UPROPERTY()
	AArinaGameMode* ArinaGameMode;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	int32 CountDownInt = 0;

	UPROPERTY(ReplicatedUsing= OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	float HUDMaxHealth = 0.f;
	float HUDCurrHealth = 0.f;
	float HUDMaxShield = 0.f;
	float HUDCurrShield = 0.f;
	int32 HUDGrenades = 0.f;
	
	void DisplayWinners();
	void DisplayTopThree(const AArinaPlayerState* CurrentPlayerState, TArray<AArinaPlayerState*>& Leaderboard, FString& InfoTextString);
	
	void StartUrgentCountdown();
	FTimerHandle BlinkerTimerHandle;
	void BlinkerTimerFinished();
	
};
