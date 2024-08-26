// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAnimInstance.h"
#include "ArinaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UArinaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ArinaCharacter = Cast<AArinaCharacter>(TryGetPawnOwner());
}

void UArinaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ArinaCharacter == nullptr)
	{
		ArinaCharacter = Cast<AArinaCharacter>(TryGetPawnOwner());
	}

	if (ArinaCharacter == nullptr)
	{
		return;
	}

	// get velocity and zero out z so that velocity is focused just on movement on the horizontal plane
	FVector Velocity = ArinaCharacter->GetVelocity();
	Velocity.Z = 0.f;
	CharacterSpeed = Velocity.Size();

	bInAir = ArinaCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = ArinaCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;

	bWeaponEquipped = ArinaCharacter->IsWeaponEquipped();
	bIsCrouched = ArinaCharacter->bIsCrouched;
	bAiming = ArinaCharacter->IsAiming();

	// Offset Yaw for strafing
	FRotator AimRotation = ArinaCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ArinaCharacter->GetVelocity());
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	// Calculate lean using character's previous rotation and current rotation
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ArinaCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	// scales value up and makes value frame rate independent
	const float Target = Delta.Yaw / DeltaSeconds;
	const float InterpTarget = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(InterpTarget, -90.f, 90.f);
}
