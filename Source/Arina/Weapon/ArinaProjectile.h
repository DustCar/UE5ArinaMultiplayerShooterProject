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
	virtual void Destroyed() override;

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

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* TracerFX;
	
	UParticleSystemComponent* TracerParticleComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactFX;
	
	UPROPERTY(EditAnywhere)
	USoundBase* ImpactSound;

public:	
	

};
