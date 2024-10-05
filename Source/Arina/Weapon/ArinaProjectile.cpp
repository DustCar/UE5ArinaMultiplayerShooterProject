// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaProjectile.h"

#include "Arina/Arina.h"
#include "Arina/Character/ArinaCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AArinaProjectile::AArinaProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 20000.f;
	ProjectileMovementComponent->MaxSpeed = 20000.f;
}

void AArinaProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (TracerFX)
	{
		UGameplayStatics::SpawnEmitterAttached(
			TracerFX,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	// affects world, bind to server
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AArinaProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(OtherActor);
	if (ArinaCharacter)
	{
		ArinaCharacter->MulticastHit();
	}

	bool bCharacterHit = Cast<AArinaCharacter>(OtherActor) ? true : false;

	if (HasAuthority())
	{
		MulticastOnHit(bCharacterHit, Hit.ImpactPoint, Hit.ImpactNormal);
	}

	Destroy();
}

void AArinaProjectile::MulticastOnHit_Implementation(bool bCharacterHit, const FVector_NetQuantize& ImpactLocation, const FVector_NetQuantizeNormal& ImpactNormal)
{
	if (ActorHitFX && SurfaceHitFX)
	{
		ImpactFX = bCharacterHit ? ActorHitFX : SurfaceHitFX;
	}

	if (ActorHitSound && SurfaceHitSound)
	{
		ImpactSound = bCharacterHit ? ActorHitSound : SurfaceHitSound;
	}
	
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFX, ImpactLocation, ImpactNormal.Rotation());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}

void AArinaProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


