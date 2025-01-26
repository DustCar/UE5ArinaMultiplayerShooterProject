// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaPickup.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Arina/Character/ArinaCharacter.h"
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

	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupEffectComponent"));
	PickupEffectComponent->SetupAttachment(RootComponent);
}

void AArinaPickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			BindOverlapTimer,
			this,
			&ThisClass::BindOverlapTimerFinished,
			BindOverlapDelay
		);
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

void AArinaPickup::OnOverlap(AArinaCharacter* ArinaCharacter)
{
}

void AArinaPickup::BindOverlapTimerFinished()
{
	// clear timer since it is no longer needed
	GetWorldTimerManager().ClearTimer(BindOverlapTimer);
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AArinaCharacter::StaticClass());

	if (OverlappingActors.Num() > 0)
	{
		AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(GetClosestActor(OverlappingActors));
		if (ArinaCharacter)
		{
			OnOverlap(ArinaCharacter);
		}
	}
}

AActor* AArinaPickup::GetClosestActor(const TArray<AActor*> OverlappingActors)
{
	AActor* ClosestActor = nullptr;
	float ClosestDistance = FLT_MAX;

	for (AActor* Player : OverlappingActors)
	{
		if (Player)
		{
			const float Distance = (GetActorLocation() - Player->GetActorLocation()).SizeSquared();
			if (Distance < ClosestDistance)
			{
				ClosestActor = Player;
				ClosestDistance = Distance;
			}
		}
	}

	return ClosestActor;
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
	// remove the delegate from the pickup
	OverlapSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnSphereOverlap);

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	
	if (DestroyedEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			DestroyedEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
	
}
