// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arina/ArinaTypesHeaders/CombatState.h"
#include "Components/ActorComponent.h"
#include "Arina/ArinaTypesHeaders/HUDPackageStruct.h"
#include "Arina/Weapon/WeaponTypes.h"
#include "ArinaCombatComponent.generated.h"

class AArinaProjectile;
class AArinaHUD;
class AArinaPlayerController;
class AArinaBaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARINA_API UArinaCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UArinaCombatComponent();
	// "friend class" allows any class of type AArinaCharacter to access all functions of CombatComponent
	friend class AArinaCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetHUDCarriedAmmo();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AArinaBaseWeapon* WeaponToEquip);
	void FireButtonPressed(bool bPressed);
	void AimButtonPressed(bool bPressed);
	void ReloadWeapon();
	void FinishedReloading();

	void ShotgunShellReload();
	void JumpToShotgunEnd();

	void ThrowGrenadeFinished();
	void TossGrenade();

	UFUNCTION(Server, Reliable)
	void ServerTossGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoToPickup);

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void UpdateWalkSpeed();
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void CalculateCrosshairFactors(float DeltaTime);
	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload();
	int32 AmountToReload();

	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AArinaProjectile> GrenadeClass;

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);

private:
	UPROPERTY()
	AArinaCharacter* ArinaCharacter;
	UPROPERTY()
	AArinaPlayerController* ArinaController;
	UPROPERTY()
	AArinaHUD* ArinaHUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AArinaBaseWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming = false;
	bool bAimButtonPressed = false;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed = 350.f;

	bool bFireButtonPressed = false;

	/**
	*	HUD and Crosshair
	*/

	// amount for crosshair spread
	UPROPERTY(EditAnywhere)
	float CrosshairBaseSpread = 0.48f;
	float CrosshairVelocityFactor;
	float CrosshairAirborneFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;
	float CrosshairEnemyFactor;

	FHUDPackage HUDPackage;
	FVector HitTarget;

	bool bEnemyInSight = false;
	
	/**
	*  Aiming and FOV
	*/
	// FOV when not aiming; set to camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
    float UnZoomInterpSpeed = 15.f;

	void InterpFOV(float DeltaTime);
	bool CanAim();

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* ZoomInScopeCue;
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* ZoomOutScopeCue;

	bool bScopeDisabled = false;

	/**
	 *	Sensitivity variables
	 */
	UPROPERTY(EditAnywhere, Category = "Sensitivity", meta=(UIMin = 0.f, UIMax = 1.f))
	float AimSensitivityMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Sensitivity", meta=(UIMin = 0.f, UIMax = 1.f))
	float BaseSensitivity = 0.5f;

	/**
	*	Fire for automatic weapons
	*/
	FTimerHandle FireTimerHandle;

	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();
	void Fire();
	bool CanFire();

	/**
	*	Ammo and Ammo types (future)
	*/
	// Carried ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	
	void UpdateCarriedAmmo();
	
	UFUNCTION()
	void OnRep_CarriedAmmo();

	UPROPERTY(EditAnywhere, Category = "Ammo")
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	/**
	*	Reload variables
	*/
	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();
	
	void UpdateHUDWeaponType();

	void ShowGrenadeMesh(bool bShow);

	/**
	*	Grenades
	*/
	UPROPERTY(EditAnywhere, Category = "Grenades")
	float GrenadeThrowAdjustment = 20.f;

	UPROPERTY(EditAnywhere, Category = "Grenades", ReplicatedUsing = OnRep_GrenadesHeld)
	int32 GrenadesHeld = 0;

	UFUNCTION()
	void OnRep_GrenadesHeld();

	UPROPERTY(EditAnywhere, Category = "Grenades")
	int32 MaxGrenadesHeld = 5;

	void UpdateHUDGrenadesHeld();
public:	
	FORCEINLINE int32 GetGrenadesHeld() const { return GrenadesHeld; }
	
};


