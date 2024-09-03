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

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidgetComp"));
	OverHeadWidget->SetupAttachment(RootComponent);

	CombatComp = CreateDefaultSubobject<UArinaCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);

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
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	PEI->BindAction(InputActions->InputMoveForward, ETriggerEvent::Triggered, this, &ThisClass::MoveForward);
	PEI->BindAction(InputActions->InputMoveRight, ETriggerEvent::Triggered, this, &ThisClass::MoveRight);
	PEI->BindAction(InputActions->InputLookUp, ETriggerEvent::Triggered, this, &ThisClass::LookUp);
	PEI->BindAction(InputActions->InputLookRight, ETriggerEvent::Triggered, this, &ThisClass::LookRight);
	PEI->BindAction(InputActions->InputEquipItem, ETriggerEvent::Triggered, this, &ThisClass::EquipItem);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &ThisClass::CrouchPlayer);
	PEI->BindAction(InputActions->InputAim, ETriggerEvent::Triggered, this, &ThisClass::AimIn);

}

void AArinaCharacter::MoveForward(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		float MoveForwardValue = Value.Get<float>();

		if (MoveForwardValue != 0.f)
		{
			AddMovementInput(GetActorForwardVector(), MoveForwardValue);
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
			AddMovementInput(GetActorRightVector(), MoveRightValue);
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

	// obtain yaw of the character for aim offsets when not moving
	// standing still and not jumping
	if (CharacterSpeed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
	}
	// running or jumping
	if (CharacterSpeed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
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
