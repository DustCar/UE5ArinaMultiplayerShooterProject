// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ArinaAnimNotify_ReloadFinished.generated.h"

/**
 *	Anim notify to call the combat component's reload finished function
 */
UCLASS(meta = (DisplayName = "Reload Finished"))
class ARINA_API UArinaAnimNotify_ReloadFinished : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
