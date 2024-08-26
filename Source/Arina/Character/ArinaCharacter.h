// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ArinaCharacter.generated.h"

class UArinaCombatComponent;
class AArinaBaseWeapon;
class UWidgetComponent;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ARINA_API AArinaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AArinaCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
	class UArinaInputConfigData* InputActions;

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

	UFUNCTION()
	void OnRep_OverlappingWeapon(AArinaBaseWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipItem();

	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void LookUp(const FInputActionValue& Value);
	void LookRight(const FInputActionValue& Value);
	void EquipItem();
	void CrouchPlayer();
	void AimIn(const FInputActionValue& Value);

public:	
	void SetOverlappingWeapon(AArinaBaseWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

};
