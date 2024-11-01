// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaProjectileRocket.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
AArinaProjectileRocket::AArinaProjectileRocket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AArinaProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArinaProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage,
				MinDamage,
				GetActorLocation(),
				InnerRadius,
				OuterRadius,
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				FiringController
			);
		}
	}
	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

// Called every frame
void AArinaProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

