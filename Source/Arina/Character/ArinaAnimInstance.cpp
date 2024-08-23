// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaAnimInstance.h"
#include "ArinaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}
