// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Components/SphereComponent.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupArea = CreateDefaultSubobject<USphereComponent>(TEXT("PickupArea"));
	PickupArea->SetupAttachment(RootComponent);

	PickupArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	// if the local machine has authority, then the collision will be enabled and Pawns will overlap on weapon
	if (HasAuthority())
	{
		PickupArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

