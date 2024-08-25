// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInput/Public//EnhancedInputComponent.h"
#include "Arina/ArinaInputConfigData.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

// Called when the game starts or when spawned
void AArinaCharacter::BeginPlay()
{
	Super::BeginPlay();

	//ServerSetPlayerName(ThisPlayerName);
}


// Called every frame
void AArinaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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