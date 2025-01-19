// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaBuffComponent.h"

#include "Arina/Character/ArinaCharacter.h"


UArinaBuffComponent::UArinaBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UArinaBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UArinaBuffComponent::Heal(float HealAmount, float HealTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealTime;
	AmountToHeal += HealAmount;
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
		bHealing = false;
		AmountToHeal = 0.f;
	}
}


void UArinaBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	HealRampUp(DeltaTime);
}


