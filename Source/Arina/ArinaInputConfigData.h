// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArinaInputConfigData.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class ARINA_API UArinaInputConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputMoveForward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputMoveRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputLookUp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputLookRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputEquipItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputThrowGrenade;
};
