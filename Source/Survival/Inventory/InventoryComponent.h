// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
* Container for items in the inventory component.
* Holds the reference as well as slot/stack info.
*/
USTRUCT(BlueprintType)
struct FInventoryItemSlotInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	FName ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 StackSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 MaxStackSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<class UBaseItem> ItemTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	class UBaseItem *ItemTypeReference;

	bool ItemTypeClassIsValid();
	bool ItemTypeRefIsValid();

	UBaseItem *GetItemRef()
	{
		return ItemTypeReference;
	}

	FInventoryItemSlotInfo()
	{
		ItemID = FName();
		SlotIndex = INDEX_NONE;
		StackSize = 0;
		MaxStackSize = 0;
		ItemTypeClass = nullptr;
		ItemTypeReference = nullptr;
	}
};

/**
* Inventory component for any character that need an inventory.
* Handles all inventory specific functionality except pickup logic,
* which is first handled in the SurvivalCharacter class (@see SurvivalCharacter::HandlePickupItem)
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	int32 Slots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<FInventoryItemSlotInfo> Items;


public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;


	bool AddItem(const FName &ItemID, int32 NewStackSize, TSubclassOf<class UBaseItem> ItemTypeClass);

	bool DropItem(int32 Slot, int32 StackSize);

	int32 GetOpenSlotIndex() const;

	int32 GetItemInfoIndexAtSlot(int SlotIndex) const;

private:
	
	TArray<int32> _openSlots;
	
};
