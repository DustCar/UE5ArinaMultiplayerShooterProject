// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaProjectile.h"
#include "ArinaProjectileRocket.generated.h"

UCLASS()
class ARINA_API AArinaProjectileRocket : public AArinaProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArinaProjectileRocket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit 
	) override;

private:
	UPROPERTY(EditAnywhere)
	float MinDamage = 10.f;
	UPROPERTY(EditAnywhere)
	float InnerRadius = 200.f;
	UPROPERTY(EditAnywhere)
	float OuterRadius = 500.f;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;
	
};
