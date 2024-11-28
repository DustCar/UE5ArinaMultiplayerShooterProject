// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaProjectileRocket.h"

#include "ArinaRocketMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"


AArinaProjectileRocket::AArinaProjectileRocket()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<UArinaRocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->SetIsReplicated(true);
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->ProjectileGravityScale = 0.f;
}

void AArinaProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	SpawnTrailSystem();

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
	if (OtherActor == GetOwner()) { return; }
	
	ExplodeDamage();
	StartDestroyTimer();

	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
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



