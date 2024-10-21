// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArinaPlayerController.generated.h"

class AArinaHUD;
/**
 * 
 */
UCLASS()
class ARINA_API AArinaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float CurrentHealth, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDeaths(int32 Defeats);
	void SetHUDWeaponAmmo(int32 WeaponAmmo);
	void SetHUDCarryAmmo(int32 CarryAmmo);
	
	void DisplayKilledByMessage(FString KillerName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCollapseKilledByMessage();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	AArinaHUD* ArinaHUD;
	
};
