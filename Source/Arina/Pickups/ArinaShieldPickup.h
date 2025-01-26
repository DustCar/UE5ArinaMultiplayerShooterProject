// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaPickup.h"
#include "ArinaShieldPickup.generated.h"

UCLASS()
class ARINA_API AArinaShieldPickup : public AArinaPickup
{
	GENERATED_BODY()

public:
	AArinaShieldPickup();

protected:
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
	float ShieldReplenishAmount = 50.f;
};
