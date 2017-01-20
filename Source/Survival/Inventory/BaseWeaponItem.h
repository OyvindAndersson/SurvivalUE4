// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/BaseItem.h"
#include "Tickable.h"
#include "BaseWeaponItem.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, Config = Game)
class SURVIVAL_API UBaseWeaponItem : public UBaseItem, public FTickableGameObject
{
	GENERATED_UCLASS_BODY()

	friend class UWeaponState;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileWeapon)
	int32 ClipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileWeapon)
	int32 MaxClipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseWeapon)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseWeapon)
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseWeapon)
	TSubclassOf<class ABaseWeaponActor> WeaponActor;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = ProjectileWeapon)
	TArray<TSubclassOf<class ASurvivalProjectile>> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TArray<class USoundBase*> FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TArray<class UAnimMontage*> FireAnimation;

	//////////////////////////////////////////////////////////
	// FTickableGameObject

	virtual void Tick(float fDeltaTime) override;
	virtual bool IsTickable() const override { return true;  }
	virtual TStatId GetStatId() const override { return this->GetStatID(); }

	virtual void BeginPlay();


	//////////////////////////////////////////////////////////
	// States
	
public:

	virtual void GotoState(class UWeaponState *NewState);
	virtual void StateChanged(){}

	// 0: Inactive state
	// 1: Equipping state [Equip -> Idle]
	// 2: Idle state [Idle -> Fire | Idle -> Reload]
	// 3: Firing state [dryfire / fireprojectile]
	// 4: Reloading state

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Weapon States")
	UWeaponState *CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon States")
	UWeaponState *InactiveState;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon States")
	UWeaponState *EquippingState;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon States")
	UWeaponState *IdleState;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon States")
	UWeaponState *FiringState;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon States")
	UWeaponState *ReloadingState;

	//////////////////////////////////////////////////////////
	// State variables

	float fRefireDelay;
	float fReloadTime;

public:
	///////////////////////////////////////////////////////////
	// Other

	UFUNCTION(BlueprintCallable, Category = Debug)
		void PrintItemDebug();
};