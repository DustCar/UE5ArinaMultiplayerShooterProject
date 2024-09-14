// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCombatComponent.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Arina/Weapon/ArinaBaseWeapon.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UArinaCombatComponent::UArinaCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bAiming = false;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 350.f;
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

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UArinaCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult Hit;
	
	TraceUnderCrosshairs(Hit);
}

// notifies clients to set rotation back to camera
void UArinaCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

// local setting
void UArinaCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

// calls to the server to implement changes server wide
void UArinaCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UArinaCombatComponent::EquipWeapon(AArinaBaseWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr)
	{
		return;
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(WeaponToEquip, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UArinaCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		ServerFire();
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
		FVector End = Start + CrosshairWorldDir * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
		else
		{
			DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 10.f, 12, FColor::Green);
		}
	}
}

void UArinaCombatComponent::MulticastFire_Implementation()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	if (Character)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire();
	}
}

void UArinaCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}

