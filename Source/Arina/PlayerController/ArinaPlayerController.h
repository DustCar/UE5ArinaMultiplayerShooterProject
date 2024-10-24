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
	void SetHUDHealth(const float& CurrentHealth, const float& MaxHealth);
	void SetHUDScore(const float& Score);
	void SetHUDDeaths(const int32& Defeats);
	void SetHUDWeaponAmmo(const int32& WeaponAmmo);
	void SetHUDCarryAmmo(const int32& CarryAmmo);
	void SetHUDWeaponType(const FString& WeaponType);
	
	void DisplayKilledByMessage(const FString& KillerName);
	void CollapseKilledByMessage();

	UFUNCTION(Client, Reliable)
	void ClientCollapseKilledByMessage();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	AArinaHUD* ArinaHUD;
	
};
