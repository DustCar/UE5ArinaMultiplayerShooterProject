// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ArinaAnimInstance.generated.h"

class AArinaCharacter;
/**
 * 
 */
UCLASS()
class ARINA_API UArinaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = true))
	AArinaCharacter* ArinaCharacter;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float CharacterSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bInAir;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bWeaponEquipped;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
    bool bIsCrouched;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool bAiming;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float YawOffset;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float Lean;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float AO_Yaw;

	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = true))
	float AO_Pitch;
};
