// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "ArinaBaseWeapon.generated.h"

class AArinaPlayerController;
class AArinaCharacter;
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();

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

	/**
	*	Weapon FX
	*/
	UPROPERTY(EditAnywhere, Category = "ArinaWeaponProps")
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, Category = "ArinaWeaponProps")
	USoundBase* DropSound;

	FString GetWeaponName() const;

	/**
	*	Enable or disable custom depth
	*/
	void EnableCustomDepth(bool bEnable);
	
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

	UPROPERTY()
	AArinaCharacter* OwnerArinaCharacter;

private:
	UPROPERTY(VisibleAnywhere, Category = "ArinaWeaponProps")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "ArinaWeaponProps")
	USphereComponent* PickupArea;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "ArinaWeaponProps", ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "ArinaWeaponProps")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category="ArinaWeaponProps")
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

	/**
	*	Ammo
	*/
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();
	
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY()
	AArinaPlayerController* OwnerArinaPlayerController;

	UPROPERTY(EditAnywhere, Category = "ArinaWeaponProps")
	EWeaponType WeaponType;

public:
	void SetWeaponState(EWeaponState State);
	void SetHUDAmmo();
	void AddToAmmoCount(int32 Count);
	FORCEINLINE USphereComponent* GetPickupArea() const { return PickupArea; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bAutomatic; }
	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetWeaponAmmoAmount() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE bool MagIsFull() const { return Ammo == MagCapacity; }
};
