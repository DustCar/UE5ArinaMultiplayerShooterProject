// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAnimNotify_TossGrenade.h"

#include "Arina/Character/ArinaCharacter.h"

void UArinaAnimNotify_TossGrenade::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	AArinaCharacter* Owner = Cast<AArinaCharacter>(MeshComp->GetOwner());
	if (Owner && Owner->GetCombatComponent())
	{
		Owner->GetCombatComponent()->TossGrenade();
	}
}
