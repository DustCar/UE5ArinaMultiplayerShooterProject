// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaCasing.generated.h"

class URotatingMovementComponent;

UCLASS()
class ARINA_API AArinaCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	AArinaCasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit 
	);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* RotatingMovementComponent;

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse;

	UPROPERTY(EditAnywhere)
	USoundBase* CasingSound;

	// member vars for "cooling down" effect; courtesy of Rob Meade
	UPROPERTY(EditAnywhere)
	float MinCoolTime;
	
	UPROPERTY(EditAnywhere)
	float MaxCoolTime;

	UFUNCTION()
	void CoolDown();

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	float CoolTime;
	float EmissivePower;
	float EmissivePowerDelta;
	float CoolRate;

	FTimerHandle CoolDownTimerHandle;
	
};
