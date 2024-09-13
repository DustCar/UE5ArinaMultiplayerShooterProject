// Fill out your copyright notice in the Description page of Project Settings.


#include "ArinaProjectile.h"

#include "Components/BoxComponent.h"

AArinaProjectile::AArinaProjectile()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	
}

void AArinaProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArinaProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

