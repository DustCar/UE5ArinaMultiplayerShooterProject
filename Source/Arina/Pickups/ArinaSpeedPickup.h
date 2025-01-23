// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaPickup.h"
#include "ArinaSpeedPickup.generated.h"

UCLASS()
class ARINA_API AArinaSpeedPickup : public AArinaPickup
{
	GENERATED_BODY()

public:
	AArinaSpeedPickup();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

private:
	UPROPERTY(EditAnywhere)
	float SpeedBuffMult = 1.25f;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 20.f;
};
