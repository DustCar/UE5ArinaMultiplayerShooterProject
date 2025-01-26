// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Arina/Arina.h"
#include "EnhancedInput/Public//EnhancedInputComponent.h"
#include "Arina/ArinaInputConfigData.h"
#include "Arina/ArinaComponents/ArinaBuffComponent.h"
#include "Arina/ArinaComponents/ArinaCombatComponent.h"
#include "Arina/GameMode/ArinaGameMode.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"

AArinaCharacter::AArinaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetMesh());
	CameraSpringArm->TargetArmLength = 600.f;
	CameraSpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.f, 0.f);

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComp"));
	OverHeadWidget->SetupAttachment(RootComponent);

	CombatComp = CreateDefaultSubobject<UArinaCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetIsReplicated(true);

	BuffComp = CreateDefaultSubobject<UArinaBuffComponent>(TEXT("BuffComponent"));
	BuffComp->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

	// Remove collision between character and camera to not bug out camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 120.f;
	MinNetUpdateFrequency = 60.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AArinaCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ThisClass, CurrentHealth);
	DOREPLIFETIME(ThisClass, CurrentShield);
	DOREPLIFETIME(ThisClass, bDisableGameplay);
}

// Runs after all components of actor have been initialized
void AArinaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->ArinaCharacter = this;
	}
	if (BuffComp)
	{
		BuffComp->ArinaCharacter = this;
		BuffComp->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched);
		BuffComp->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
}

void AArinaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
	UpdateHUDShield();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::AArinaCharacter::ReceiveDamage);
	}

	if (GrenadeMesh)
	{
		GrenadeMesh->SetVisibility(false);
	}
}

void AArinaCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	AArinaGameMode* ArinaGameMode = Cast<AArinaGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = ArinaGameMode && ArinaGameMode->GetMatchState() != MatchState::InProgress;
	if (CombatComp && CombatComp->EquippedWeapon && bMatchNotInProgress)
	{
		CombatComp->EquippedWeapon->Destroy();
	}
}

void AArinaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AArinaPlayerController* PlayerController = Cast<AArinaPlayerController>(NewController);
	if (PlayerController)
	{
		UpdateHUDHealth();
		UpdateHUDShield();
	}
}

void AArinaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
	HideCharacterIfCameraClose();
}

// Called to bind functionality to input
void AArinaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* LocalSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		LocalSubsystem->ClearAllMappings();
		LocalSubsystem->AddMappingContext(InputMapping, 0);
	}

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	PEI->BindAction(InputActions->InputMoveForward, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
	PEI->BindAction(InputActions->InputMoveRight, ETriggerEvent::Triggered, this, &ThisClass::MoveRight);
	PEI->BindAction(InputActions->InputLookUp, ETriggerEvent::Triggered, this, &ThisClass::LookUp);
	PEI->BindAction(InputActions->InputLookRight, ETriggerEvent::Triggered, this, &ThisClass::LookRight);
	PEI->BindAction(InputActions->InputEquipItem, ETriggerEvent::Triggered, this, &ThisClass::EquipItem);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &ThisClass::CrouchPlayer);
	PEI->BindAction(InputActions->InputAim, ETriggerEvent::Triggered, this, &ThisClass::AimIn);
	PEI->BindAction(InputActions->InputFire, ETriggerEvent::Triggered, this, &ThisClass::Fire);
	PEI->BindAction(InputActions->InputReload, ETriggerEvent::Triggered, this, &ThisClass::Reload);
	PEI->BindAction(InputActions->InputThrowGrenade, ETriggerEvent::Triggered, this, &ThisClass::ThrowGrenadePressed);
}

void AArinaCharacter::MoveForward(const FInputActionValue& Value)
{
	if (bDisableGameplay) { return; }
	
	if (Controller != nullptr)
	{
		float MoveForwardValue = Value.Get<float>();

		if (MoveForwardValue != 0.f)
		{
			const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
			const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
			AddMovementInput(Direction, MoveForwardValue);
		}
	}
}

void AArinaCharacter::MoveRight(const FInputActionValue& Value)
{
	if (bDisableGameplay) { return; }

	if (Controller != nullptr)
	{
		double MoveRightValue = Value.Get<float>();

		if (MoveRightValue != 0.f)
		{
			const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
			const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
			AddMovementInput(Direction, MoveRightValue);
		}
	}
}

void AArinaCharacter::LookUp(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		double LookUpValue = Value.Get<FVector2D>().Y;

		if (LookUpValue != 0.f)
		{
			AddControllerPitchInput(LookUpValue * RotationLookSpeedMultiplier);
		}
	}
}

void AArinaCharacter::LookRight(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		double LookRightValue = Value.Get<FVector2D>().X;

		if (LookRightValue != 0.f)
		{
			AddControllerYawInput(LookRightValue * RotationLookSpeedMultiplier);
		}
	}
}

void AArinaCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 45.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -45.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	// Turns the character to where the camera is pointing by resetting Aim Offset yaw to 0
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AArinaCharacter::CrouchPlayer()
{
	if (bDisableGameplay) { return; }

	if (bSpaceBarUncrouch)
	{
		bSpaceBarUncrouch = false;
		return;
	}
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AArinaCharacter::Jump()
{
	if (bDisableGameplay) { return; }

	if (bIsCrouched)
	{
		bSpaceBarUncrouch = true;
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AArinaCharacter::AimIn(const FInputActionValue& Value)
{
	if (bDisableGameplay) { return; }

	if (CombatComp && CombatComp->EquippedWeapon)
	{
		CombatComp->AimButtonPressed(Value.Get<bool>());
	}
}

void AArinaCharacter::AimOffset(float DeltaTime)
{
	if (CombatComp && CombatComp->EquippedWeapon == nullptr)
	{
		return;
	}
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		
		return;
	}
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float CharacterSpeed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// obtain yaw of the character for aim offsets when not moving. aim and move based on top half
	// standing still and not jumping
	if (CharacterSpeed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		
		bUseControllerRotationYaw = true; // rotates character but gets cancelled when rotating bone in ABP
		TurnInPlace(DeltaTime);
	}
	// aim and move whole character
	// running or jumping
	if (CharacterSpeed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;

	// handles issue of AO_Pitch becoming unsigned from yaw and pitch compression in the Character Movement Component class
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90,0) for pawns that are not locally controlled
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AArinaCharacter::Fire(const FInputActionValue& Value)
{
	if (bDisableGameplay) { return; }

	if (CombatComp && CombatComp->EquippedWeapon)
	{
		CombatComp->FireButtonPressed(Value.Get<bool>());
	}
}

void AArinaCharacter::Reload()
{
	if (bDisableGameplay) { return; }

	if (CombatComp && CombatComp->EquippedWeapon)
	{
		CombatComp->ReloadWeapon();
	}
}

void AArinaCharacter::ThrowGrenadePressed()
{
	if (bDisableGameplay) { return; }

	if (CombatComp)
	{
		CombatComp->ThrowGrenade();
	}
}

void AArinaCharacter::PlayFireMontage(bool bAiming)
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AArinaCharacter::PlayReloadMontage()
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName = FName(CombatComp->EquippedWeapon->GetWeaponName());

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AArinaCharacter::PlayEliminatedMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EliminatedMontage)
	{
		AnimInstance->Montage_Play(EliminatedMontage);
	}
}

void AArinaCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void AArinaCharacter::Eliminated()
{
	if (CombatComp && CombatComp->EquippedWeapon)
	{
		CombatComp->EquippedWeapon->Dropped();
	}
	MulticastEliminated();
	GetWorldTimerManager().SetTimer(
		EliminatedTimer,
		this,
		&ThisClass::EliminatedTimerFinished,
		EliminatedDelay
	);
}

void AArinaCharacter::MulticastEliminated_Implementation()
{
	bEliminated = true;

	if (ArinaPlayerController)
	{
		ArinaPlayerController->SetHUDWeaponAmmo(0);
		ArinaPlayerController->SetHUDCarryAmmo(0);
		ArinaPlayerController->SetHUDWeaponType("Unequipped");
	}

	// Creating dynamic material instance for dissolve effect and starting it
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);

		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 150.f);
	}
	StartDissolve();

	// Disable character movement and input
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	bDisableGameplay = true;
	if (CombatComp) { CombatComp->FireButtonPressed(false); }

	// Disable collision
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);
	}

	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}

	bool bScopedIn = IsLocallyControlled() &&
		CombatComp &&
		CombatComp->EquippedWeapon &&
		CombatComp->EquippedWeapon &&
		CombatComp->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle &&
		CombatComp->bAiming;

	if (bScopedIn)
	{
		CombatComp->SetAiming(false);
	}
}

void AArinaCharacter::EliminatedTimerFinished()
{
	AArinaGameMode* ArinaGameMode = GetWorld()->GetAuthGameMode<AArinaGameMode>();
	if (ArinaGameMode)
	{
		ArinaGameMode->RequestRespawn(this, Controller);
	}
}

void AArinaCharacter::PlayHitReactMontage()
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(FName("FromFront"));
	}
}

void AArinaCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorBy, AActor* DamageCauser)
{
	if (bEliminated) return;

	AArinaGameMode* ArinaGameMode = GetWorld()->GetAuthGameMode<AArinaGameMode>();
	float DamageToHealth = Damage;
	
	if (CurrentShield > 0.f)
	{
		if (CurrentShield >= Damage)
		{
			CurrentShield = FMath::Clamp(CurrentShield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
			// TODO: if hit play shield hit animation.
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - CurrentShield, 0.f, Damage);
			CurrentShield = 0.f;
		}
	}
	UpdateHUDShield();
	
	if (CurrentHealth > 0.f)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - DamageToHealth, 0.f, MaxHealth);
		UpdateHUDHealth();
		PlayHitReactMontage();
	}
	
	if (CurrentHealth <= 0.f)
	{
		if (ArinaGameMode)
		{
			ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(Controller) : ArinaPlayerController;
			AArinaPlayerController* AttackerController = Cast<AArinaPlayerController>(InstigatorBy);
			ArinaGameMode->PlayerEliminated(this, ArinaPlayerController, AttackerController);
		}
	}
}

void AArinaCharacter::HideCharacterIfCameraClose()
{
	if (!IsLocallyControlled()) { return; }

	bool bMakeVisible = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() >= CameraThreshold;
	
	GetMesh()->SetVisibility(bMakeVisible);
	if (CombatComp && CombatComp->EquippedWeapon && CombatComp->EquippedWeapon->GetWeaponMesh())
	{
		CombatComp->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = !bMakeVisible;
	}
}

void AArinaCharacter::UpdateHUDHealth()
{
	ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(Controller) : ArinaPlayerController;
	if (ArinaPlayerController)
	{
		ArinaPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

void AArinaCharacter::OnRep_CurrentHealth(float LastHealth)
{
	UpdateHUDHealth();
	if (CurrentHealth > 0.f && CurrentHealth < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void AArinaCharacter::UpdateHUDShield()
{
	ArinaPlayerController = ArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(Controller) : ArinaPlayerController;
	if (ArinaPlayerController)
	{
		ArinaPlayerController->SetHUDShield(CurrentShield, MaxShield);
	}
}

void AArinaCharacter::OnRep_CurrentShield(float LastShield)
{
	UpdateHUDShield();
	// TODO: if hit play shield hit animation.
}

void AArinaCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

void AArinaCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void AArinaCharacter::SetOverlappingWeapon(AArinaBaseWeapon* Weapon)
{
	// Check for the character that is owned by the server machine, since OnRep_OverlappingWeapon will not run for the
	// server because replication does not occur for the server machine
	// Without check, widget would disappear for server if the client walks away from weapon, despite the server overlapping
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}
	
	OverlappingWeapon = Weapon;

	// Same check as before, keeps it server-side only 
	// Without check, server would see the widget if a Client is overlapping a weapon
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AArinaCharacter::OnRep_OverlappingWeapon(AArinaBaseWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AArinaCharacter::EquipItem()
{
	if (bDisableGameplay) { return; }

	if (CombatComp)
	{
		// if server machine, directly call the combat comp EquipWeapon() function
		if (HasAuthority())
		{
			CombatComp->EquipWeapon(OverlappingWeapon);
		}
		// if client machine, call the Server RPC version of EquipItem()
		else
		{
			ServerEquipItem();
		}
	}
}

void AArinaCharacter::ServerEquipItem_Implementation()
{
	if (CombatComp)
	{
		CombatComp->EquipWeapon(OverlappingWeapon);
	}
}

bool AArinaCharacter::IsWeaponEquipped()
{
	return CombatComp && CombatComp->EquippedWeapon;
}

bool AArinaCharacter::IsAiming()
{
	return CombatComp && CombatComp->bAiming && CombatComp->CanAim();
}

AArinaBaseWeapon* AArinaCharacter::GetEquippedWeapon() const
{
	if (CombatComp == nullptr)
	{
		return nullptr;
	}

	return CombatComp->EquippedWeapon;
}

FVector AArinaCharacter::GetHitTarget()
{
	if (CombatComp == nullptr)
	{
		return FVector();
	}

	return CombatComp->HitTarget;
}

ECombatState AArinaCharacter::GetCombatState() const
{
	if (CombatComp == nullptr)
	{
		return ECombatState::ECS_MAX;
	}

	return CombatComp->CombatState;
}
