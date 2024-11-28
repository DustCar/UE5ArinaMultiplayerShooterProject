// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaProjectile.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class ARINA_API AArinaProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AArinaProjectile();

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTrailTimerFinished();
	void ExplodeDamage();

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

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;
	
	UPROPERTY(EditAnywhere, Category= "Projectile")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category= "Projectile|VFX")
	UParticleSystem* ImpactFX;
	
	UPROPERTY(EditAnywhere, Category= "Projectile|SFX")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, Category= "Projectile|VFX")
	UParticleSystem* SurfaceHitFX;

	UPROPERTY(EditAnywhere, Category= "Projectile|VFX")
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailSystemComponent;

	void SpawnTrailSystem();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	
private:

	UPROPERTY(EditAnywhere, Category= "Projectile|VFX")
	UParticleSystem* TracerFX;

	UPROPERTY(EditAnywhere, Category= "Projectile|SFX")
	USoundBase* SurfaceHitSound;

	UPROPERTY(EditAnywhere, Category= "Projectile|VFX")
	UParticleSystem* ActorHitFX;

	UPROPERTY(EditAnywhere, Category= "Projectile|SFX")
	USoundBase* ActorHitSound;

	FTimerHandle DestroyTrailTimer;
	UPROPERTY(EditAnywhere, Category= "Projectile")
	float TrailTimer = 3.f;

	UPROPERTY(EditAnywhere, Category= "Projectile")
	float MinDamage = 10.f;
	UPROPERTY(EditAnywhere, Category= "Projectile")
	float ExplosionInnerRadius = 100.f;
	UPROPERTY(EditAnywhere, Category= "Projectile")
	float ExplosionOuterRadius = 300.f;

public:	
	

};
