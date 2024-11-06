// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaProjectile.h"
#include "ArinaProjectileRocket.generated.h"

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
	virtual void Tick(float DeltaTime) override;
	void DestroyTrailTimerFinished();

	virtual void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit 
	) override;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere)
	USoundBase* ProjectileSoundLoop;

	UPROPERTY()
	UAudioComponent* ProjectileSoundLoopComponent;

	UPROPERTY(EditAnywhere)
	USoundAttenuation* ProjectileSoundLoopAttenuation;

private:
	UPROPERTY(EditAnywhere)
	float MinDamage = 10.f;
	UPROPERTY(EditAnywhere)
	float InnerRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float OuterRadius = 500.f;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTrailTimer;

	UPROPERTY(EditAnywhere)
	float TrailTimer = 3.f;
	
};
