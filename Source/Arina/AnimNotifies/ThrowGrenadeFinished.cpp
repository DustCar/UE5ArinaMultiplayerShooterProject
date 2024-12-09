// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowGrenadeFinished.h"

#include "Arina/Character/ArinaCharacter.h"

void UThrowGrenadeFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	AArinaCharacter* Owner = Cast<AArinaCharacter>(MeshComp->GetOwner());
	if (Owner && Owner->GetCombatComponent() && Owner->GetEquippedWeapon())
	{
		Owner->GetCombatComponent()->ThrowGrenadeFinished();
	}
}