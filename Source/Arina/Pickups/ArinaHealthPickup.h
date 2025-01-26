// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaPickup.h"
#include "ArinaHealthPickup.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class ARINA_API AArinaHealthPickup : public AArinaPickup
{
	GENERATED_BODY()

public:
	AArinaHealthPickup();
	
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
	bool bHealing = false;
	
	UPROPERTY(EditAnywhere)
	float HealAmount = 50.f;

	UPROPERTY(EditAnywhere)
	float HealingTime = 3.f;
	
};
