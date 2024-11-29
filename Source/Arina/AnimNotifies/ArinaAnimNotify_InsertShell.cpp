// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAnimNotify_InsertShell.h"

#include "Arina/Character/ArinaCharacter.h"

void UArinaAnimNotify_InsertShell::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	AArinaCharacter* Owner = Cast<AArinaCharacter>(MeshComp->GetOwner());
	if (Owner && Owner->GetCombatComponent() && Owner->GetEquippedWeapon())
	{
		Owner->GetCombatComponent()->ShotgunShellReload();
	}
}
