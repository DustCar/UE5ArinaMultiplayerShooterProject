// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCombatComponent.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Arina/HUD/ArinaHUD.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UArinaCombatComponent::UArinaCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UArinaCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeapon);
	DOREPLIFETIME(ThisClass, bAiming);
}

void UArinaCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ArinaCharacter)
	{
		ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (ArinaCharacter->GetFollowCamera())
		{
			DefaultFOV = ArinaCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UArinaCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ArinaCharacter && ArinaCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		if (EquippedWeapon)
		{
			SetHUDCrosshairs(DeltaTime);
		}
		InterpFOV(DeltaTime);
	}
	
	
}

void UArinaCombatComponent::EquipWeapon(AArinaBaseWeapon* WeaponToEquip)
{
	if (ArinaCharacter == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket = ArinaCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(WeaponToEquip, ArinaCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(ArinaCharacter);
	ArinaCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	ArinaCharacter->bUseControllerRotationYaw = true;
}

// notifies clients to set rotation back to camera
void UArinaCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && ArinaCharacter)
	{
		ArinaCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		ArinaCharacter->bUseControllerRotationYaw = true;
	}
}

// local setting
void UArinaCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (ArinaCharacter && EquippedWeapon)
	{
		ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
		float LookSpeedMultiplier = bIsAiming ? AimSensitivityMultiplier * BaseSensitivity : BaseSensitivity;
		ArinaCharacter->SetRotationLookSpeedMultiplier(LookSpeedMultiplier);
	}
}

// calls to the server to implement changes server wide
void UArinaCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (ArinaCharacter && EquippedWeapon)
	{
		ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
		float LookSpeedMultiplier = bIsAiming ? AimSensitivityMultiplier * BaseSensitivity : BaseSensitivity;
		ArinaCharacter->SetRotationLookSpeedMultiplier(LookSpeedMultiplier);
	}
}

void UArinaCombatComponent::StartFireTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&UArinaCombatComponent::FireTimerFinished,
			EquippedWeapon->GetFireDelay()
		);
	}
}

void UArinaCombatComponent::FireTimerFinished()
{
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->IsAutomatic())
	{
		Fire();
	}
}

void UArinaCombatComponent::Fire()
{
	if (bCanFire)
	{
		bCanFire = false;
		ServerFire(HitTarget);
		StartFireTimer();
	}
	
}

void UArinaCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UArinaCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	if (ArinaCharacter)
	{
		ArinaCharacter->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UArinaCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UArinaCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize = FVector2D::ZeroVector;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPos;
	FVector CrosshairWorldDir;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPos,
		CrosshairWorldDir
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPos;

		if (ArinaCharacter)
		{
			float DistanceToCharacter = (ArinaCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDir * (DistanceToCharacter + 30.f);
		}
		
		FVector End = Start + CrosshairWorldDir * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);

		if (TraceHitResult.bBlockingHit == false)
		{
			TraceHitResult.ImpactPoint = End;
		}

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UArinaCrosshairInteractionInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
			bEnemyInSight = true;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
			bEnemyInSight = false;
		}
	}
}

void UArinaCombatComponent::CalculateCrosshairFactors(float DeltaTime)
{
	if (ArinaCharacter->GetCharacterMovement()->IsFalling())
	{
		CrosshairAirborneFactor = FMath::FInterpTo(CrosshairAirborneFactor, 1.5f, DeltaTime, 2.5f);
	}
	else
	{
		CrosshairAirborneFactor = FMath::FInterpTo(CrosshairAirborneFactor, 0.f, DeltaTime, 40.f);
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.48f, DeltaTime, 30.f);	
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	if (EquippedWeapon && bFireButtonPressed)
	{
		CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.8f, DeltaTime, 20.f);
	}
	else
	{
		CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 10.f);
	}

	if (bEnemyInSight)
	{
		if (bAiming)
		{
			CrosshairEnemyFactor = 0.f;
		}
		else
		{
			CrosshairEnemyFactor = FMath::FInterpTo(CrosshairEnemyFactor, 0.28f, DeltaTime, 30.f);
		}
	}
	else
	{
		CrosshairEnemyFactor = FMath::FInterpTo(CrosshairEnemyFactor, 0.f, DeltaTime, 30.f);
	}

	HUDPackage.CrosshairSpread = CrosshairBaseSpread +
		CrosshairVelocityFactor +
		CrosshairAirborneFactor -
		CrosshairAimFactor +
		CrosshairShootFactor -
		CrosshairEnemyFactor;
	
}

void UArinaCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (ArinaCharacter == nullptr || ArinaCharacter->Controller == nullptr) { return; }

	ArinaController = ArinaController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->Controller) : ArinaController;

	if (ArinaController)
	{
		ArinaHUD = ArinaHUD == nullptr ? Cast<AArinaHUD>(ArinaController->GetHUD()) : ArinaHUD;

		if (ArinaHUD)
		{
			HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;
			HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
			HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
			HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
			HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;

			// calculate crosshair spread

			FVector2D WalkSpeedRange;
			FVector2D VelocityMultiplierRange;

			if (ArinaCharacter->bIsCrouched)
			{
				WalkSpeedRange = FVector2D(0.f, ArinaCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched);
				VelocityMultiplierRange =  FVector2D(0.f, 0.5f);
			}
			else
			{
				WalkSpeedRange = FVector2D(0.f, ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed);
				VelocityMultiplierRange =  FVector2D(0.f, 0.8f);
			}
			
			FVector Velocity = ArinaCharacter->GetVelocity();
			Velocity.Z = 0.f;
			
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
				WalkSpeedRange,
				VelocityMultiplierRange,
				Velocity.Size()
			);

			CalculateCrosshairFactors(DeltaTime);
			
			ArinaHUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UArinaCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) { return; }

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, UnZoomInterpSpeed);
	}

	if (ArinaCharacter && ArinaCharacter->GetFollowCamera())
	{
		ArinaCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}
