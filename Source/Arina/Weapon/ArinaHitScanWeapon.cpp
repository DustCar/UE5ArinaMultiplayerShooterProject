// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaHitScanWeapon.h"

#include "Arina/Character/ArinaCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"

AArinaHitScanWeapon::AArinaHitScanWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArinaHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World == nullptr) { return; }
	
	FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
	World->LineTraceSingleByChannel(
		OutHit,
		TraceStart,
		End,
		ECC_Visibility
	);

	FVector BeamEnd = End;

	if (OutHit.bBlockingHit)
	{
		BeamEnd = OutHit.ImpactPoint;
	}
	
	if (BeamFX)
	{
		UParticleSystemComponent* BeamFXComponent = UGameplayStatics::SpawnEmitterAtLocation(
			World,
			BeamFX,
			TraceStart,
			FRotator::ZeroRotator,
			true
		);

		if (BeamFXComponent)
		{
			BeamFXComponent->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}
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

	FHitResult FireHit;
	WeaponTraceHit(Start, HitTarget, FireHit);

	if (FireHit.bBlockingHit)
	{
		AArinaCharacter* ArinaCharacter = Cast<AArinaCharacter>(FireHit.GetActor());
		if (ArinaCharacter && InstigatorController) 
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

		PlayImpactFX(FireHit);
	}
}

FVector AArinaHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandomVector =
		UKismetMathLibrary::RandomUnitVector() *
		FMath::FRandRange(0.f, OwnerArinaCharacter->IsAiming() ? AimedSphereRadius : SphereRadius);
	
	FVector EndPoint = SphereCenter + RandomVector;
	FVector ToEndPoint = EndPoint - TraceStart;

	/*DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndPoint, 4.f, 12, FColor::Yellow, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndPoint * TRACE_LENGTH / ToEndPoint.Size()),
		FColor::Cyan,
		true
	);*/

	return FVector(TraceStart + ToEndPoint * TRACE_LENGTH / ToEndPoint.Size());
}

void AArinaHitScanWeapon::PlayImpactFX(FHitResult& FireHit)
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
			FireHit.ImpactPoint
		);
	}
}

