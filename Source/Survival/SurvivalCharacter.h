// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Inventory/InventoryComponent.h"
#include "SurvivalCharacter.generated.h"


//////////////////////////////////////////////////////////////////////
// ASurvivalCharacter - The games' playable pawn

class UInputComponent;
class AItemWorldActor;

UCLASS(config=Game)
class ASurvivalCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
		
public:
	ASurvivalCharacter();

	virtual void BeginPlay();

	virtual void Tick(float Delta) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent *InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	float ActionTraceDistance;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ASurvivalProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

public:
	///////////////////////////////////////////////////////////
	// Player stats

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;


public:
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
	void ItemSlotAdded(const FItemSlotInfo &NewItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool CraftItems(int32 SlotA, int32 SlotB);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool DropItem(int32 Slot, int32 StackSize);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool DropItemSlot(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool SwapItemSlots(int32 SlotA, int32 SlotB);

	// Interaction

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool UseItem(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void Equip(int32 Slot);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void UnEquip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Inventory)
	float TakeHeal(float HealAmount, class UBaseHealingItem *HealCauser);

	void HandleEquipWeapon(class UBaseWeaponItem *WeaponItem);

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Performs an appropriate action (pick up item, open door, etc.) */
	void OnAction();

	/** Opens the inventory UI */
	void OnShowInventory();

	void OnStartReload();
	void OnEndReload();
	bool bIsReloading;
	float ReloadTime;
	float ReloadTimeAccum;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handles picking up an item from the world and puts it in inventory */
	void HandlePickupItem(AItemWorldActor *ItemPickup);

	
protected:
	// APawn / AActor interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

