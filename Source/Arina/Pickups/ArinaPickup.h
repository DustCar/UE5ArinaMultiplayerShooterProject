// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaPickup.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USphereComponent;

UCLASS()
class ARINA_API AArinaPickup : public AActor
{
	GENERATED_BODY()

public:
	AArinaPickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere, Category = "FX")
	float BaseTurnRate = 45.f;

	UPROPERTY(EditAnywhere, Category = "FX")
	float BaseFloatRate = 5.f;

	UPROPERTY(EditAnywhere, Category = "FX")
	float DistanceFromStart = 5.f;

private:
	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, Category = "FX|Sound")
	USoundBase* PickupSound;

	FVector StartLocation;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* DestroyedEffect;
	
public:
};


