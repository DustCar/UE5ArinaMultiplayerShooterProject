// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAnimInstance.h"
#include "ArinaCharacter.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"
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

	// Get velocity and zero out z so that velocity is focused just on movement on the horizontal plane
	FVector Velocity = ArinaCharacter->GetVelocity();
	Velocity.Z = 0.f;
	CharacterSpeed = Velocity.Size();

	bInAir = ArinaCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = ArinaCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;

	bWeaponEquipped = ArinaCharacter->IsWeaponEquipped();
	EquippedWeapon = ArinaCharacter->GetEquippedWeapon();
	bIsCrouched = ArinaCharacter->bIsCrouched;
	bAiming = ArinaCharacter->IsAiming();
	TurningInPlace = ArinaCharacter->GetTurningInPlace();
	bEliminated = ArinaCharacter->IsEliminated();

	// Offset Yaw for strafing
	FRotator AimRotation = ArinaCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ArinaCharacter->GetVelocity());
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	// Calculate lean using character's previous rotation and current rotation
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ArinaCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds; // Scales value up and makes value frame rate independent
	const float InterpTarget = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(InterpTarget, -90.f, 90.f);

	AO_Yaw = ArinaCharacter->GetAO_Yaw();
	AO_Pitch = ArinaCharacter->GetAO_Pitch();

	// Assigns left hand to left hand socket position on weapon mesh. Used with FABRIK in the character ABP
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ArinaCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);

		FVector OutPos;
		FRotator OutRot;
		// Transforms left hand socket on weapon to be relative to "hand_r" bone space on character
		// We use bone space to keep numbers small
		ArinaCharacter->GetMesh()->TransformToBoneSpace(
			FName("hand_r"),
			LeftHandTransform.GetLocation(),
			FRotator::ZeroRotator,
			OutPos,
			OutRot
		);
		LeftHandTransform.SetLocation(OutPos);
		LeftHandTransform.SetRotation(FQuat(OutRot));

		if (ArinaCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = ArinaCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - ArinaCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 20.f);
		}
	}

	bUseFABRIK = ArinaCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = ArinaCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = ArinaCharacter->GetCombatState() != ECombatState::ECS_Reloading;
}
