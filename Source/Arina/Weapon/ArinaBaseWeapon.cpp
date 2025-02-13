// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaBaseWeapon.h"

#include "ArinaCasing.h"
#include "Arina/Character/ArinaCharacter.h"
#include "Arina/PlayerController/ArinaPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AArinaBaseWeapon::AArinaBaseWeapon()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	AActor::SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	PickupArea = CreateDefaultSubobject<USphereComponent>(TEXT("PickupArea"));
	PickupArea->SetupAttachment(RootComponent);

	PickupArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AArinaBaseWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

// Called when the game starts or when spawned
void AArinaBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	// if the local machine has authority, then the collision will be enabled and Pawns will overlap on weapon
	if (HasAuthority())
	{
		PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		PickupArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		PickupArea->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

// Called every frame
void AArinaBaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArinaBaseWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WeaponState);
	DOREPLIFETIME(ThisClass, Ammo);
}

void AArinaBaseWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		OwnerArinaCharacter = nullptr;
		OwnerArinaPlayerController = nullptr;
	}
	else
	{
		SetHUDAmmo();
	}
}

void AArinaBaseWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	if (ArinaCharacter)
	{
		ArinaCharacter->SetOverlappingWeapon(this);
	}
}

void AArinaBaseWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	if (ArinaCharacter)
	{
		ArinaCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AArinaBaseWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		// Do not need to call the function since server disables weapons pickup collision which in turn disables widget
		/*ShowPickupWidget(false);*/
		PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(WeaponType == EWeaponType::EWT_SubmachineGun);
		WeaponMesh->SetCollisionEnabled(WeaponType == EWeaponType::EWT_SubmachineGun ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		EnableCustomDepth(false);
		break;
	case EWeaponState::EWS_Dropped:
		if (HasAuthority())
		{
			PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// Set collision response in case of SMG
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		
		if (DropSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
		}
		WeaponMesh->AddImpulse(WeaponMesh->GetRightVector()*700.f);
		break;
	}
	
}

// notifies client when WeaponState changes, i.e. when a weapon gets equipped
void AArinaBaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		// Do not need to call the function since server disables weapons pickup collision when character overlap ends
		// hiding the widget
		/*ShowPickupWidget(false);*/
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(WeaponType == EWeaponType::EWT_SubmachineGun);
		WeaponMesh->SetCollisionEnabled(WeaponType == EWeaponType::EWT_SubmachineGun ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
		if (WeaponType == EWeaponType::EWT_SubmachineGun)
		{
			WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		}
		EnableCustomDepth(false);
		break;
	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// Set collision response in case of SMG
		WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
		WeaponMesh->MarkRenderStateDirty();
		EnableCustomDepth(true);
		
		if (DropSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DropSound, GetActorLocation());
		}
		WeaponMesh->AddImpulse(WeaponMesh->GetRightVector()*700.f);
		break;
	}
}

void AArinaBaseWeapon::SetHUDAmmo()
{
	OwnerArinaCharacter = OwnerArinaCharacter == nullptr ? Cast<AArinaCharacter>(GetOwner()) : OwnerArinaCharacter;
	if (OwnerArinaCharacter)
	{
		OwnerArinaPlayerController = OwnerArinaPlayerController == nullptr ? Cast<AArinaPlayerController>(OwnerArinaCharacter->GetController()) : OwnerArinaPlayerController;
		if (OwnerArinaPlayerController)
		{
			OwnerArinaPlayerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AArinaBaseWeapon::AddToAmmoCount(int32 Count)
{
	Ammo = FMath::Clamp(Ammo + Count, 0, MagCapacity);
	SetHUDAmmo();
}

void AArinaBaseWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
}

void AArinaBaseWeapon::OnRep_Ammo()
{
	OwnerArinaCharacter = OwnerArinaCharacter == nullptr ? Cast<AArinaCharacter>(GetOwner()) : OwnerArinaCharacter;
	bool bIsShotgunAndFull =
		WeaponType == EWeaponType::EWT_Shotgun &&
		OwnerArinaCharacter &&
		OwnerArinaCharacter->GetCombatComponent() &&
		MagIsFull();
	if (bIsShotgunAndFull)
	{
		OwnerArinaCharacter->GetCombatComponent()->JumpToShotgunEnd();
	}
	SetHUDAmmo();
}

void AArinaBaseWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AArinaBaseWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AArinaCasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}

	SpendRound();
}

FString AArinaBaseWeapon::GetWeaponName() const
{
	switch (WeaponType)
	{
	case EWeaponType::EWT_AssaultRifle:
		return FString("Assault Rifle");
	case EWeaponType::EWT_RocketLauncher:
		return FString("Rocket Launcher");
	case EWeaponType::EWT_Pistol:
		return FString("Pistol");
	case EWeaponType::EWT_SubmachineGun:
		return FString("Submachine Gun");
	case EWeaponType::EWT_Shotgun:
		return FString("Shotgun");
	case EWeaponType::EWT_SniperRifle:
		return FString("Sniper Rifle");
	case EWeaponType::EWT_GrenadeLauncher:
		return FString("Grenade Launcher");
	default:
		return FString("Unknown");
	}
}

void AArinaBaseWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	OwnerArinaCharacter = nullptr;
	OwnerArinaPlayerController = nullptr;
}

