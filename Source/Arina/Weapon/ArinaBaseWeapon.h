// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArinaBaseWeapon.generated.h"

class AArinaCasing;
class UArinaCombatComponent;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class ARINA_API AArinaBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AArinaBaseWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Fire(const FVector& HitTarget);

	/**
	 *	Textures for weapon crosshairs
	 */

	UPROPERTY(EditAnywhere, Category="Crosshairs")
	UTexture2D* CrosshairCenter;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category="Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category="Crosshairs")
	UTexture2D* CrosshairTop;
	
	UPROPERTY(EditAnywhere, Category="Crosshairs")
	UTexture2D* CrosshairBottom;
	
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

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	

private:
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USphereComponent* PickupArea;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties", ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category="WeaponProperties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArinaCasing> CasingClass;

	/**
	*  Zoomed FOV while aiming
	*/
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/**
	*  Automatic fire check and rates
	*/
	UPROPERTY(EditAnywhere, Category="Combat")
	bool bAutomatic = false;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float FireDelay = 0.1f;

public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetPickupArea() const { return PickupArea; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }
};
