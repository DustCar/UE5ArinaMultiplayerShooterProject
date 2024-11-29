// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ArinaAnimNotify_InsertShell.generated.h"

/**
 *	Anim notify to call the combat component's update shell values function
 */
UCLASS(meta = (DisplayName = "Insert Shell"))
class ARINA_API UArinaAnimNotify_InsertShell : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
