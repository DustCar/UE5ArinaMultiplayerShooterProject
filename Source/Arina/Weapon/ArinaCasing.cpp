// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaCasing.h"

#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AArinaCasing::AArinaCasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	RootComponent = CasingMesh;

	CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	
	ShellEjectionImpulse = 10.f;

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotatingMovementComponent->RotationRate = FRotator(0.f, -5000.f, 0.f);

	// Cool down values
	MinCoolTime = 1.f;
	MaxCoolTime = 2.f;
	CoolTime = FMath::RandRange(MinCoolTime, MaxCoolTime);
	CoolRate = 0.1f;
}

void AArinaCasing::BeginPlay()
{
	Super::BeginPlay();

	FVector EjectVector = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(GetActorForwardVector(), 10.f);
	CasingMesh->AddImpulse(EjectVector * ShellEjectionImpulse);
	CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

	SetLifeSpan(3.f);

	// Set cool down material instance and start timer
	MaterialInstance = CasingMesh->CreateDynamicMaterialInstance(0);
	if (MaterialInstance)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				CoolDownTimerHandle,
				this,
				&ThisClass::CoolDown,
				CoolRate,
				true
			);
		}

		EmissivePower = MaterialInstance->K2_GetScalarParameterValue(FName("EmissivePower"));
		EmissivePowerDelta = (EmissivePower / CoolTime) * CoolRate;
	}
}

void AArinaCasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (CasingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, Hit.ImpactPoint);
	}

	RotatingMovementComponent->Deactivate();

	// stops further sounds after initial hit
	CasingMesh->SetNotifyRigidBodyCollision(false);
}

// function to gradually decrease emissiveness of material of casing
void AArinaCasing::CoolDown()
{
	if (MaterialInstance == nullptr) { return; }

	EmissivePower -= EmissivePowerDelta;
	MaterialInstance->SetScalarParameterValue(FName("EmissivePower"), EmissivePower);

	if (EmissivePower <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CoolDownTimerHandle);
	}
}


