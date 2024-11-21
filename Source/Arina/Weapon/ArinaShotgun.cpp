// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaShotgun.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"


AArinaShotgun::AArinaShotgun()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArinaShotgun::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArinaShotgun::Fire(const FVector& HitTarget)
{
	AArinaBaseWeapon::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) { return; }

	AController* InstigatorController = OwnerPawn->GetController();
	if (InstigatorController == nullptr) { return; }

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(TEXT("MuzzleFlash"));
	if (MuzzleFlashSocket == nullptr) { return; }
	
	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector Start = SocketTransform.GetLocation();

	TMap<AArinaCharacter*, uint32> HitMap;
	for (uint32 i = 0; i < PelletCount; i++)
	{
		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		if (FireHit.bBlockingHit)
		{
			AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(FireHit.GetActor());
			if (ArinaCharacter)
			{
				if (HitMap.Contains(ArinaCharacter))
				{
					HitMap[ArinaCharacter]++;
				}
				else
				{
					HitMap.Emplace(ArinaCharacter, 1);
				}
				ImpactSound = BodyHitSound == nullptr ? ImpactSound : BodyHitSound;
				ImpactFX = BodyHitFX == nullptr ? ImpactFX : BodyHitFX;
			}

			PlayImpactFX(FireHit);
		}
	}

	for (auto HitPair : HitMap)
	{
		if (HitPair.Key && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				HitPair.Key,
				BulletDamage * HitPair.Value,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void AArinaShotgun::PlayImpactFX(FHitResult& FireHit)
{
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactFX,
			FireHit.ImpactPoint,
			FireHit.ImpactNormal.Rotation()
		);
	}

	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ImpactSound,
			FireHit.ImpactPoint,
			.5f,
			FMath::FRandRange(-0.5f, 0.5f)
		);
	}
}
