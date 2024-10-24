// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arina/ArinaComponents/ArinaCombatComponent.h"
#include "Arina/ArinaTypesHeaders/CombatState.h"
#include "Arina/ArinaTypesHeaders/TurningInPlace.h"
#include "Arina/Interfaces/ArinaCrosshairInteractionInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "ArinaCharacter.generated.h"

class AArinaPlayerController;
class UArinaCombatComponent;
class AArinaBaseWeapon;
class UWidgetComponent;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARINA_API AArinaCharacter : public ACharacter, public IArinaCrosshairInteractionInterface
{
	GENERATED_BODY()

public:
	AArinaCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	float PlayReloadMontage();
	void PlayEliminatedMontage();
	void Eliminated();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminated();

	virtual void Destroyed() override;
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	class UArinaInputConfigData* InputActions;

	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void LookUp(const FInputActionValue& Value);
	void LookRight(const FInputActionValue& Value);
	void EquipItem();
	void CrouchPlayer();
	void AimIn(const FInputActionValue& Value);
	void AimOffset(float DeltaTime);
	virtual void Jump() override;
	void Fire(const FInputActionValue& Value);
	void Reload();
	void PlayHitReactMontage();
	void UpdateHUDHealth();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorBy, AActor* DamageCauser);

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UWidgetComponent* OverHeadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AArinaBaseWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere)
	UArinaCombatComponent* CombatComp;

	UPROPERTY()
	AArinaPlayerController* ArinaPlayerController;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AArinaBaseWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipItem();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/**
	*	Animation Montages
	*/
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* EliminatedMontage;

	void HideCharacterIfCameraClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bSpaceBarUncrouch = false;
	
	// Rotation speed based on aiming
	float RotationLookSpeedMultiplier = 1.f;

	/**
	*	Player Health
	*/
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, Category = "PlayerStats")
	float CurrentHealth = MaxHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	bool bEliminated = false;

	FTimerHandle EliminatedTimer;

	UPROPERTY(EditDefaultsOnly)
	float EliminatedDelay = 3.f;

	void EliminatedTimerFinished();

	/**
	*	Dissolve Effect
	*/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;
	
	UPROPERTY(EditAnywhere, Category="Elimination")
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	// Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category="Elimination")
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// Material instance set on the Blueprint, used with the dynamic material instance
	UPROPERTY(EditAnywhere, Category = "Elimination")
	UMaterialInstance* DissolveMaterialInstance;

	/**
	*	Elim Bot
	*/
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundBase* ElimBotSound;

	
	
public:	
	void SetOverlappingWeapon(AArinaBaseWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AArinaBaseWeapon* GetEquippedWeapon() const;
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget();
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetRotationLookSpeedMultiplier(float Multiplier) { RotationLookSpeedMultiplier = Multiplier; }
	FORCEINLINE bool IsEliminated() const { return bEliminated; }
	ECombatState GetCombatState() const;
};
