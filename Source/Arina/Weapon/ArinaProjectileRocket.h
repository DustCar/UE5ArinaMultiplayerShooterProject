// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaProjectile.h"
#include "ArinaProjectileRocket.generated.h"

class UArinaRocketMovementComponent;
class USpringArmComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class ARINA_API AArinaProjectileRocket : public AArinaProjectile
{
	GENERATED_BODY()

public:
	AArinaProjectileRocket();

protected:
	virtual void BeginPlay() override;

	virtual void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit 
	) override;

	UPROPERTY(EditAnywhere)
	USoundBase* ProjectileSoundLoop;

	UPROPERTY()
	UAudioComponent* ProjectileSoundLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* ProjectileSoundLoopAttenuation;

	UPROPERTY(VisibleAnywhere)
	UArinaRocketMovementComponent* RocketMovementComponent;

private:
	
};
