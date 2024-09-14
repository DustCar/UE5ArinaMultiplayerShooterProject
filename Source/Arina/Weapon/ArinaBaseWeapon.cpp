// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaBaseWeapon.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AArinaBaseWeapon::AArinaBaseWeapon()
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

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
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

// notifies client when WeaponState changes, i.e. when a weapon gets equipped
void AArinaBaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		// Do not need to call the function since server disables weapons pickup collision when character overlap ends
		// hiding the widget
		/*ShowPickupWidget(false);*/
		break;
	}
}

void AArinaBaseWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		// Same reasoning as in the OnRep version
		/*ShowPickupWidget(false);*/
		PickupArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
	
}

void AArinaBaseWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AArinaBaseWeapon::Fire()
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
}

