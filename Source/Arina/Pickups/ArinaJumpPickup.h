// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaPickup.h"
#include "ArinaJumpPickup.generated.h"

UCLASS()
class ARINA_API AArinaJumpPickup : public AArinaPickup
{
	GENERATED_BODY()

public:
	AArinaJumpPickup();
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

	virtual void OnOverlap(AArinaCharacter* ArinaCharacter) override;

private:
	UPROPERTY(EditAnywhere)
	float JumpBuffMult = 1.5f;

	UPROPERTY(EditAnywhere)
	float JumpBuffTime = 20.f;
};
