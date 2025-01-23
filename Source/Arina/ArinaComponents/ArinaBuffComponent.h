// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArinaBuffComponent.generated.h"


class AArinaCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARINA_API UArinaBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArinaBuffComponent();
	friend class AArinaCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Heal(const float& HealAmount, const float& HealTime);
	void BuffSpeed(const float& SpeedMultiplier, const float& BuffTime);
	void SetInitialSpeeds(const float& BaseSpeed, const float& CrouchSpeed);
	
protected:
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);

private:
	UPROPERTY()
	AArinaCharacter* ArinaCharacter;

	/**
	*	Health Buff 
	*/
	
	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;

	/**
	*	Speed Buff 
	*/

	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateSpeed(const float& Multiplier = 1.f);
	
public:
	
};

