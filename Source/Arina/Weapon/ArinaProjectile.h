// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaProjectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class ARINA_API AArinaProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AArinaProjectile();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit 
	);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(bool bCharacterHit, const FVector_NetQuantize& ImpactLocation, const FVector_NetQuantizeNormal& ImpactNormal);

	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;
	
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactFX;
	
	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;
	
private:

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* TracerFX;

	UPROPERTY(EditAnywhere)
	UParticleSystem* SurfaceHitFX;

	UPROPERTY(EditAnywhere)
	USoundBase* SurfaceHitSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ActorHitFX;

	UPROPERTY(EditAnywhere)
	USoundBase* ActorHitSound;

public:	
	

};
