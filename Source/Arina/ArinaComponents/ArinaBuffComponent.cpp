// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaBuffComponent.h"

#include "Arina/Character/ArinaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UArinaBuffComponent::UArinaBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UArinaBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UArinaBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRampUp(DeltaTime);
}

// NOTE: Since each buff has some section, I know that these can be refactored so that this component
// only handles applying the buffs but tbh I am having a hard time envisioning it :(
// Heal buff section
void UArinaBuffComponent::Heal(const float& HealAmount, const float& HealTime, const float& PlayerHealth)
{
	bHealing = true;
	HealingRate = HealAmount / HealTime;
	AmountToHeal += HealAmount;
	InitialHealAmount += HealAmount;
	InitialPlayerHealth = PlayerHealth;
}

void UArinaBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || ArinaCharacter == nullptr || ArinaCharacter->IsEliminated()) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	ArinaCharacter->SetHealth(FMath::Clamp(ArinaCharacter->GetHealth() + HealThisFrame, 0.f, ArinaCharacter->GetMaxHealth()));
	ArinaCharacter->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f || ArinaCharacter->GetHealth() >= ArinaCharacter->GetMaxHealth())
	{
		if (InitialHealAmount != ArinaCharacter->GetHealth() - InitialPlayerHealth)
		{
			float HealDelta = InitialHealAmount + InitialPlayerHealth - ArinaCharacter->GetHealth();
			ArinaCharacter->SetHealth(FMath::Clamp(ArinaCharacter->GetHealth() + HealDelta, 0.f, ArinaCharacter->GetMaxHealth()));
			ArinaCharacter->UpdateHUDHealth();
		}
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

// Shield buff section
void UArinaBuffComponent::ReplenishShield(const float& ShieldAmount)
{
	if (ArinaCharacter == nullptr || ArinaCharacter->IsEliminated()) return;

	ArinaCharacter->SetShield(FMath::Clamp(ArinaCharacter->GetShield() + ShieldAmount, 0.f, ArinaCharacter->GetMaxShield()));
	ArinaCharacter->UpdateHUDShield();
}

// Speed buff section
void UArinaBuffComponent::BuffSpeed(const float& SpeedMultiplier, const float& BuffTime)
{
	if (ArinaCharacter == nullptr) return;

	ArinaCharacter->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&ThisClass::ResetSpeeds,
		BuffTime
	);

	if (ArinaCharacter->GetCharacterMovement())
	{
		MulticastUpdateSpeed(SpeedMultiplier);
	}
}

void UArinaBuffComponent::SetInitialSpeeds(const float& BaseSpeed, const float& CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UArinaBuffComponent::ResetSpeeds()
{
	if (ArinaCharacter == nullptr || ArinaCharacter->GetCharacterMovement() == nullptr) return;

	MulticastUpdateSpeed();
}

void UArinaBuffComponent::MulticastUpdateSpeed_Implementation(const float& Multiplier)
{
	ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed * Multiplier;
	ArinaCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed * Multiplier;

	if (ArinaCharacter->GetCombatComponent())
	{
		ArinaCharacter->GetCombatComponent()->SetWalkSpeeds(Multiplier);
	}
}

// Jump buff section
void UArinaBuffComponent::BuffJump(const float& JumpMultiplier, const float& BuffTime)
{
	if (ArinaCharacter == nullptr) return;

	ArinaCharacter->GetWorldTimerManager().SetTimer(
		JumpBuffTimer,
		this,
		&ThisClass::ResetJump,
		BuffTime
	);

	if (ArinaCharacter->GetCharacterMovement())
	{
		MulticastUpdateJump(JumpMultiplier);
	}
}

void UArinaBuffComponent::SetInitialJumpVelocity(const float& BaseVelocity)
{
	InitialJumpVelocity = BaseVelocity;
}

void UArinaBuffComponent::ResetJump()
{
	if (ArinaCharacter == nullptr || ArinaCharacter->GetCharacterMovement() == nullptr) return;

	MulticastUpdateJump();
}

void UArinaBuffComponent::MulticastUpdateJump(const float& Multiplier)
{
	ArinaCharacter->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity * Multiplier;
}

