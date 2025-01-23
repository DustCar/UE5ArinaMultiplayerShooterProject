// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCombatComponent.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Arina/HUD/ArinaHUD.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"
#include "Arina/Weapon/ArinaProjectile.h"
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
	DOREPLIFETIME_CONDITION(ThisClass, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(ThisClass, CombatState);
	DOREPLIFETIME(ThisClass, GrenadesHeld);
}

void UArinaCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ArinaCharacter)
	{
		BaseWalkSpeed = InitialBaseWalkSpeed = ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed;

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

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

void UArinaCombatComponent::DropEquippedWeapon() const
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UArinaCombatComponent::AttachActorToRightHand(AActor* ActorToAttach) const
{
	if (ArinaCharacter == nullptr || ArinaCharacter->GetMesh() == nullptr || ActorToAttach == nullptr) { return; }

	const USkeletalMeshSocket* HandSocket = ArinaCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, ArinaCharacter->GetMesh());
	}
}

void UArinaCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach) const
{
	if (ArinaCharacter == nullptr || ArinaCharacter->GetMesh() == nullptr || ActorToAttach == nullptr || EquippedWeapon == nullptr) { return; }
	bool bOtherSocket =
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SubmachineGun;

	FName SocketName = bOtherSocket ? FName("PistolLHandSocket") : FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket = ArinaCharacter->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, ArinaCharacter->GetMesh());
	}
}

void UArinaCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	SetHUDCarriedAmmo();
}

void UArinaCombatComponent::EquipWeapon(AArinaBaseWeapon* WeaponToEquip)
{
	if (ArinaCharacter == nullptr || WeaponToEquip == nullptr) { return; }
	if (CombatState != ECombatState::ECS_Unoccupied) { return; }

	DropEquippedWeapon();

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, ArinaCharacter->GetActorLocation());
	}
	
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetOwner(ArinaCharacter);
	EquippedWeapon->SetHUDAmmo();
	UpdateHUDWeaponType();
	
	UpdateCarriedAmmo();
	
	ArinaCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	ArinaCharacter->bUseControllerRotationYaw = true;
}

// notifies clients to set rotation back to camera
void UArinaCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && ArinaCharacter)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		if (EquippedWeapon->EquipSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipSound, ArinaCharacter->GetActorLocation());
		}
		
		AttachActorToRightHand(EquippedWeapon);
		UpdateHUDWeaponType();

		ArinaCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		ArinaCharacter->bUseControllerRotationYaw = true;
	}
}

void UArinaCombatComponent::SetWalkSpeeds(const float& SpeedMultiplier)
{
	AimWalkSpeed = InitialAimWalkSpeed * SpeedMultiplier;
	BaseWalkSpeed = InitialBaseWalkSpeed * SpeedMultiplier;
}

void UArinaCombatComponent::UpdateWalkSpeed()
{
	if (ArinaCharacter && EquippedWeapon)
	{
		float Sensitivity = bAiming && CanAim() ? BaseSensitivity * AimSensitivityMultiplier : BaseSensitivity;
		ArinaCharacter->GetCharacterMovement()->MaxWalkSpeed = bAiming && CanAim() ? AimWalkSpeed : BaseWalkSpeed;
		ArinaCharacter->SetRotationLookSpeedMultiplier(Sensitivity);
	}
}

void UArinaCombatComponent::AimButtonPressed(bool bPressed)
{
	bAimButtonPressed = bPressed;
	SetAiming(bPressed);
}

// local setting
void UArinaCombatComponent::SetAiming(bool bIsAiming)
{
	if (ArinaCharacter == nullptr || EquippedWeapon == nullptr) { return; }
	bAiming = bIsAiming;
	
    ServerSetAiming(bIsAiming);
    UpdateWalkSpeed();

	if (ArinaCharacter->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && !bScopeDisabled)
	{
		ArinaController = ArinaController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->GetController()) : ArinaController;
		ArinaController->SetHUDSniperScope(bIsAiming);
		if (bIsAiming)
		{
			UGameplayStatics::PlaySound2D(this, ZoomInScopeCue);
		}
		else
		{
			UGameplayStatics::PlaySound2D(this, ZoomOutScopeCue);
		}
	}
}

// calls to the server to implement changes server wide
void UArinaCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	UpdateWalkSpeed();
}

bool UArinaCombatComponent::CanAim() const
{
	if (EquippedWeapon == nullptr)
	{
		return false;
	}

	return CombatState == ECombatState::ECS_Unoccupied;
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

bool UArinaCombatComponent::CanFire() const
{
	if (EquippedWeapon == nullptr)
	{
		return false;
	}
	if (EquippedWeapon && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun && bCanFire && CombatState == ECombatState::ECS_Reloading)
	{
		return true;
	}

	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UArinaCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	if (ArinaCharacter && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
	{
		ArinaCharacter->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}
	if (ArinaCharacter && CombatState == ECombatState::ECS_Unoccupied)
	{
		ArinaCharacter->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}

	// Rocket Launch?? using LaunchCharacter()
}

void UArinaCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UArinaCombatComponent::Fire()
{
	if (EquippedWeapon == nullptr) { return; }
	
	if (EquippedWeapon->IsEmpty())
	{
		ReloadWeapon();
		return;
	}
	if (CanFire())
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

void UArinaCombatComponent::ShotgunShellReload()
{
	if (ArinaCharacter && ArinaCharacter->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}
}

void UArinaCombatComponent::ReloadWeapon()
{
	if (EquippedWeapon == nullptr || EquippedWeapon->MagIsFull()) { return; }
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied)
	{
		// block of code to deal with reloading while aiming down a scope
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
		{
			if (bAimButtonPressed)
			{
				SetAiming(false);
			}
			bScopeDisabled = true;
		}
		
		ServerReload();
	}
}

void UArinaCombatComponent::ServerReload_Implementation()
{
	if (ArinaCharacter == nullptr)
	{
		return;
	}
	CombatState = ECombatState::ECS_Reloading;
	HandleReload();
	UpdateWalkSpeed();
}

void UArinaCombatComponent::HandleReload() const
{
	ArinaCharacter->PlayReloadMontage();
}

int32 UArinaCombatComponent::AmountToReload()
{
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetWeaponAmmoAmount();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmmoInBag = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 AmmoNeeded = RoomInMag > AmmoInBag ? AmmoInBag : RoomInMag;
		return AmmoNeeded;
	}
	
	return 0;
}

void UArinaCombatComponent::ShowGrenadeMesh(bool bShow) const
{
	if (ArinaCharacter && ArinaCharacter->GetGrenadeMesh())
	{
		ArinaCharacter->GetGrenadeMesh()->SetVisibility(bShow);
	}
}

void UArinaCombatComponent::ThrowGrenade()
{
	if (GrenadesHeld == 0) return;
	if (EquippedWeapon == nullptr || CombatState != ECombatState::ECS_Unoccupied) return;

	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (ArinaCharacter)
	{
		ArinaCharacter->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowGrenadeMesh(true);
	}
	// call server rpc when not host
	if (ArinaCharacter && !ArinaCharacter->HasAuthority())
	{
		ServerThrowGrenade();
	}
	if (ArinaCharacter && ArinaCharacter->HasAuthority())
	{
		GrenadesHeld = FMath::Clamp(GrenadesHeld - 1, 0, MaxGrenadesHeld);
		UpdateHUDGrenadesHeld();
	}
}

void UArinaCombatComponent::ServerThrowGrenade_Implementation()
{
	if (GrenadesHeld == 0) return;
	
	CombatState = ECombatState::ECS_ThrowingGrenade;
	if (ArinaCharacter)
	{
		ArinaCharacter->PlayThrowGrenadeMontage();
		AttachActorToLeftHand(EquippedWeapon);
		ShowGrenadeMesh(true);
	}

	GrenadesHeld = FMath::Clamp(GrenadesHeld - 1, 0, MaxGrenadesHeld);
	UpdateHUDGrenadesHeld();
}

void UArinaCombatComponent::OnRep_GrenadesHeld()
{
	UpdateHUDGrenadesHeld();
}

void UArinaCombatComponent::UpdateHUDGrenadesHeld()
{
	ArinaController = ArinaController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->GetController()) : ArinaController;
	if (ArinaController)
	{
		ArinaController->SetHUDGrenades(GrenadesHeld);
	}
}

void UArinaCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	AttachActorToRightHand(EquippedWeapon);
}

// Function that is used for an AnimNotify called the same name
void UArinaCombatComponent::TossGrenade()
{
	ShowGrenadeMesh(false);
	if (ArinaCharacter && ArinaCharacter->IsLocallyControlled())
	{
		ServerTossGrenade(HitTarget);
	}
}

void UArinaCombatComponent::ServerTossGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (ArinaCharacter && GrenadeClass && ArinaCharacter->GetGrenadeMesh())
	{
		const FVector StartLocation = ArinaCharacter->GetGrenadeMesh()->GetComponentLocation();
		FVector ToTarget = Target - StartLocation;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = ArinaCharacter;
		SpawnParams.Instigator = ArinaCharacter;

		FVector UpdatedStartLocation = StartLocation;
		if (ArinaCharacter->GetVelocity() != FVector::ZeroVector)
		{
			UpdatedStartLocation = StartLocation + (ToTarget.GetSafeNormal() * GrenadeThrowAdjustment);
		}

		UWorld* World = GetWorld();
		if (World == nullptr) { return; }
		World->SpawnActor<AArinaProjectile>(
			GrenadeClass,
			UpdatedStartLocation,
			ToTarget.Rotation(),
			SpawnParams
		);
	}
}

void UArinaCombatComponent::UpdateHUDWeaponType()
{
	ArinaController = ArinaController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->GetController()) : ArinaController;
	if (ArinaController)
	{
		ArinaController->SetHUDWeaponType(EquippedWeapon->GetWeaponName());
	}
}

void UArinaCombatComponent::UpdateAmmoValues()
{
	if (ArinaCharacter == nullptr || EquippedWeapon == nullptr) { return; }
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	SetHUDCarriedAmmo();
	EquippedWeapon->AddToAmmoCount(ReloadAmount);
}

void UArinaCombatComponent::UpdateShotgunAmmoValues()
{
	if (ArinaCharacter == nullptr || EquippedWeapon == nullptr) { return; }

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()]--;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	SetHUDCarriedAmmo();
	EquippedWeapon->AddToAmmoCount(1);

	bCanFire = true;
	if (EquippedWeapon->MagIsFull() || CarriedAmmo == 0)
	{
		JumpToShotgunEnd();
	}
}

void UArinaCombatComponent::JumpToShotgunEnd() const
{
	UAnimInstance* AnimInstance = ArinaCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && ArinaCharacter->GetReloadMontage())
	{
		AnimInstance->Montage_Play(ArinaCharacter->GetReloadMontage());

		AnimInstance->Montage_JumpToSection(FName("Shotgun End"));
	}
}

void UArinaCombatComponent::FinishedReloading()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	
	if (ArinaCharacter->HasAuthority() && CombatState == ECombatState::ECS_Reloading)
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}

	if (bFireButtonPressed && EquippedWeapon->IsAutomatic())
	{
		Fire();
	}

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		bScopeDisabled = false;
		if (bAimButtonPressed)
		{
			SetAiming(true);
		}
	}
	
	UpdateWalkSpeed();
}

void UArinaCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandleReload();
		UpdateWalkSpeed();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed && EquippedWeapon->IsAutomatic())
		{
			Fire();
		}
		UpdateWalkSpeed();
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if (ArinaCharacter && !ArinaCharacter->IsLocallyControlled())
		{
			ArinaCharacter->PlayThrowGrenadeMontage();
			AttachActorToLeftHand(EquippedWeapon);
			ShowGrenadeMesh(true);
		}
	default:
		break;
	}
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

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(ArinaCharacter);

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility, QueryParams);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor() != ArinaCharacter && TraceHitResult.GetActor()->Implements<UArinaCrosshairInteractionInterface>())
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

	if (bAiming && CanAim())
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.48f, DeltaTime, 30.f);	
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	if (EquippedWeapon && !EquippedWeapon->IsEmpty() && bFireButtonPressed && CombatState != ECombatState::ECS_Reloading)
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
		if (ArinaHUD == nullptr) { return; }
		
		if (EquippedWeapon && ((EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && !bAiming && CanAim()) || EquippedWeapon->GetWeaponType() != EWeaponType::EWT_SniperRifle))
		{
			HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairCenter;
			HUDPackage.CrosshairRight = EquippedWeapon->CrosshairRight;
			HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairLeft;
			HUDPackage.CrosshairTop = EquippedWeapon->CrosshairTop;
			HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairBottom;
		}
		else
		{
			HUDPackage.CrosshairCenter = nullptr;
			HUDPackage.CrosshairRight = nullptr;
			HUDPackage.CrosshairLeft = nullptr;
			HUDPackage.CrosshairTop = nullptr;
			HUDPackage.CrosshairBottom = nullptr;
		}

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

void UArinaCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) { return; }

	if (bAiming && CanAim())
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

void UArinaCombatComponent::SetHUDCarriedAmmo()
{
	ArinaController = ArinaController == nullptr ? Cast<AArinaPlayerController>(ArinaCharacter->GetController()) : ArinaController;
	if (ArinaController)
	{
		ArinaController->SetHUDCarryAmmo(CarriedAmmo);
	}
}

void UArinaCombatComponent::OnRep_CarriedAmmo()
{
	bool bIsShotgunAndNoAmmo =
		EquippedWeapon &&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun &&
		CombatState == ECombatState::ECS_Reloading &&
		CarriedAmmo == 0;
	if (bIsShotgunAndNoAmmo)
	{
		JumpToShotgunEnd();
	}
	SetHUDCarriedAmmo();
}

void UArinaCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoToPickup)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoToPickup, 0, 999);
		UpdateCarriedAmmo();
	}

	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		ReloadWeapon();
	}
}


