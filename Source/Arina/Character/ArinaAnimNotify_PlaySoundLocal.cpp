// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAnimNotify_PlaySoundLocal.h"

#include "Kismet/GameplayStatics.h"

void UArinaAnimNotify_PlaySoundLocal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp == nullptr)
	{
		return;
	}
	if (AActor* Owner = MeshComp->GetOwner())
	{
// copied from AnimNotify_PlaySound.cpp just for editor preview
#if WITH_EDITORONLY_DATA
		UWorld* World = MeshComp->GetWorld();
		if (World && World->WorldType == EWorldType::EditorPreview)
		{
			UGameplayStatics::PlaySound2D(World, Sound, VolumeMultiplier, PitchMultiplier);
		}
		else
#endif
		{
			// checks if player is locally controlled then calls notify to play the sound
			AController* Controller = Owner->GetInstigatorController();
			if (Controller && Controller->IsLocalPlayerController())
			{
				Super::Notify(MeshComp, Animation, EventReference);
			}
		}
	}
}
