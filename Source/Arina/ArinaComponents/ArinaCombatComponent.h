// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArinaCombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AArinaBaseWeapon* WeaponToEquip);

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void FireButtonPressed(bool bPressed);
	
	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	
private:

	AArinaCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AArinaBaseWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bFireButtonPressed;

public:	

	
};
