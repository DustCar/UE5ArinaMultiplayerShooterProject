// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaProjectileRocket.h"

#include "ArinaRocketMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AArinaProjectileRocket::AArinaProjectileRocket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<UArinaRocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->SetIsReplicated(true);
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AArinaProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}

	if (ProjectileSoundLoop && ProjectileSoundLoopAttenuation)
	{
		ProjectileSoundLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileSoundLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			ProjectileSoundLoopAttenuation,
			nullptr,
			false
		);
	}

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AArinaProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
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

	GetWorldTimerManager().SetTimer(
		DestroyTrailTimer,
		this,
		&ThisClass::DestroyTrailTimerFinished,
		TrailTimer
	);

	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (RocketMesh)
	{
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->GetSystemInstanceController()->Deactivate();
	}

	if (ProjectileSoundLoopComponent && ProjectileSoundLoopComponent->IsPlaying())
	{
		ProjectileSoundLoopComponent->Stop();
	}
}

// Called every frame
void AArinaProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArinaProjectileRocket::DestroyTrailTimerFinished()
{
	Destroy();
}

