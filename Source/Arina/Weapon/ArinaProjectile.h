// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaProjectile.generated.h"

class UBoxComponent;

UCLASS()
class ARINA_API AArinaProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AArinaProjectile();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

public:	
	

};
