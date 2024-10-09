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

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AArinaHUD* ArinaHUD;
	
};
