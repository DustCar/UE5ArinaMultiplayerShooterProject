// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInput/Public//EnhancedInputComponent.h"
#include "Arina/ArinaInputConfigData.h"
#include "Arina/ArinaComponents/ArinaCombatComponent.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AArinaCharacter::AArinaCharacter()
{
	bReplicates = true;
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

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// Remove collision between character and camera to not bug out camera
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void AArinaCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, OverlappingWeapon, COND_OwnerOnly);
}

// Runs after all components of actor have been initialized
void AArinaCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->Character = this;
	}
}

// Called when the game starts or when spawned
void AArinaCharacter::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void AArinaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
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

}

void AArinaCharacter::MoveForward(const FInputActionValue& Value)
{
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
			AddControllerPitchInput(LookUpValue);
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
			AddControllerYawInput(LookRightValue);
		}
	}
}

void AArinaCharacter::CrouchPlayer()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AArinaCharacter::AimIn(const FInputActionValue& Value)
{
	if (CombatComp)
	{
		CombatComp->SetAiming(Value.Get<bool>());
	}
}

void AArinaCharacter::AimOffset(float DeltaTime)
{
	if (CombatComp && CombatComp->EquippedWeapon == nullptr)
	{
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

void AArinaCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AArinaCharacter::Fire(const FInputActionValue& Value)
{
	if (CombatComp)
	{
		CombatComp->FireButtonPressed(Value.Get<bool>());
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

void AArinaCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
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
	return (CombatComp && CombatComp->EquippedWeapon);
}

bool AArinaCharacter::IsAiming()
{
	return (CombatComp && CombatComp->bAiming);
}

AArinaBaseWeapon* AArinaCharacter::GetEquippedWeapon() const
{
	if (CombatComp == nullptr)
	{
		return nullptr;
	}

	return CombatComp->EquippedWeapon;
}
