// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "ArinaAnimNotify_PlaySoundLocal.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Play Sound Local"))
class ARINA_API UArinaAnimNotify_PlaySoundLocal : public UAnimNotify_PlaySound
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
