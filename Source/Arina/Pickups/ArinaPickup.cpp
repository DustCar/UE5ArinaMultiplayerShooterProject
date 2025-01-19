// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPickup.h"

#include "Arina/Weapon/WeaponTypes.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


AArinaPickup::AArinaPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(RootComponent);
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetSphereRadius(150.f);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 50.f));

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(OverlapSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupMesh->SetRelativeScale3D(FVector(2.5f));
}

void AArinaPickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}

	if (PickupMesh)
	{
		StartLocation = PickupMesh->GetComponentLocation();
	}
}

void AArinaPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AArinaPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupMesh)
	{
		PickupMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
		if (PickupMesh->GetComponentLocation().Z > StartLocation.Z + DistanceFromStart || PickupMesh->GetComponentLocation().Z < StartLocation.Z - DistanceFromStart)
		{
			BaseFloatRate *= -1.f;
		}
		PickupMesh->AddWorldOffset(FVector(0.f, 0.f, BaseFloatRate * DeltaTime));
	}
}

void AArinaPickup::Destroyed()
{
	Super::Destroyed();

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}
