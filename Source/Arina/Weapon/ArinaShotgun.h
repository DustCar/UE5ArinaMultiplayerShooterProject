// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArinaHitScanWeapon.h"
#include "ArinaShotgun.generated.h"

UCLASS()
class ARINA_API AArinaShotgun : public AArinaHitScanWeapon
{
	GENERATED_BODY()

public:
	AArinaShotgun();

protected:
	virtual void BeginPlay() override;

	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere, Category= "WeaponScatter")
	uint32 PelletCount = 12;


public:
};
