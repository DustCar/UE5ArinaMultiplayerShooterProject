// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaHitScanWeapon.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AArinaHitScanWeapon::AArinaHitScanWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArinaHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) { return; }

	AController* InstigatorController = OwnerPawn->GetController();
	if (InstigatorController == nullptr) { return; }

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
	if (MuzzleFlashSocket == nullptr) { return; }
	
	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector Start = SocketTransform.GetLocation();
	FVector End = Start + (HitTarget - Start) * 1.25f;

	FHitResult FireHit;
	UWorld* World = GetWorld();
	if (World == nullptr) { return; }
	
	World->LineTraceSingleByChannel(
		FireHit,
		Start,
		End,
		ECC_Visibility
	);

	FVector BeamEnd = End;

	if (FireHit.bBlockingHit)
	{
		BeamEnd = FireHit.ImpactPoint;
		AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(FireHit.GetActor());
		if (ArinaCharacter) 
		{
			if (HasAuthority())
			{
				UGameplayStatics::ApplyDamage(
					ArinaCharacter,
					BulletDamage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			ImpactFX = BodyHitFX == nullptr ? ImpactFX : BodyHitFX;
			ImpactSound = BodyHitSound == nullptr ? ImpactSound : BodyHitSound;
		}

		if (ImpactFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				World,
				ImpactFX,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
		}

		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				World,
				ImpactSound,
				FireHit.ImpactPoint
			);
		}
	}

	if (BeamFX)
	{
		UParticleSystemComponent* BeamFXComponent = UGameplayStatics::SpawnEmitterAtLocation(
			World,
			BeamFX,
			SocketTransform
		);

		if (BeamFXComponent)
		{
			BeamFXComponent->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
}

void AArinaHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArinaHitScanWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

