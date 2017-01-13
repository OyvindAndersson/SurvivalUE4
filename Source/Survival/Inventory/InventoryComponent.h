// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
* Container for items in the inventory component.
* Holds the reference as well as slot/stack info.
*/
USTRUCT(BlueprintType)
struct FItemSlotInfo
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

	FItemSlotInfo()
	{
		ItemID = FName();
		SlotIndex = INDEX_NONE;
		StackSize = 0;
		MaxStackSize = 0;
		ItemTypeClass = nullptr;
		ItemTypeReference = nullptr;
	}

	FItemSlotInfo(const FName &ItemID, int32 Slot, int32 StackSize, int32 MaxStackSize,
		TSubclassOf<class UBaseItem> ItemTypeClass, UBaseItem *ItemTypeReference)
	{
		this->ItemID = ItemID;
		this->SlotIndex = Slot;
		this->StackSize = StackSize;
		this->MaxStackSize = MaxStackSize;
		this->ItemTypeClass = ItemTypeClass;
		this->ItemTypeReference = ItemTypeReference;
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemSlotAddedSignature, const FItemSlotInfo&, NewSlotInfo);

/**
* Inventory component for any character that need an inventory.
* Handles all inventory specific functionality except pickup logic,
* which is first handled in the SurvivalCharacter class (@see SurvivalCharacter::HandlePickupItem)
*
* Remember, inventory slots and item indices are different. The items are stored as a simple
* TArray, which has ordered and shuffeling indices. The slot indices however are used
* to swap places in the UI and such. Therefore SlotIndex and ItemIndex must not be
* confused.
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 Slots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	int32 Rows;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	int32 Columns;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<FItemSlotInfo> Items;


public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	///////////////////////////////////////////////////////////////
	// Events

	UPROPERTY(BlueprintAssignable, Category = "Inventory Events")
	FItemSlotAddedSignature OnItemSlotAddedDelegate;

	///////////////////////////////////////////////////////////////
	// Inventory handling

	// Adds a regular item to the inventory or to the stack of a similar item
	bool AddItem(const FName &ItemID, int32 NewStackSize, EItemType ItemType, TSubclassOf<class UBaseItem> ItemTypeClass);

	// Drop an item or the stacksize of an item
	bool DropItem(int32 Slot, int32 StackSize);

	// Utility to set a new info in a slot. Important step includes closing the slot index, which is vital
	FORCEINLINE void SetInSlot(const FItemSlotInfo &SlotInfo)
	{
		// Add item to inventory
		Items.Add(SlotInfo);

		// Make sure to remove the now occupied slot index from the open slots array
		// This will be added back when the item is removed/depleted in the inventory
		_openSlots.Remove(SlotInfo.SlotIndex);
	}

	// Utility to get an open inventory slot index
	FORCEINLINE int32 GetOpenSlotIndex()
	{
		// If no slots are available, we return invalid index
		if (_openSlots.Num() == 0)
			return INDEX_NONE;

		return _openSlots[0]; // Return the first available slot
	}

	// Utility to get the FInventoryItemSlotInfo at the specified Inventory slot index.
	FORCEINLINE int32 GetItemInfoIndexAtSlot(int SlotIndex)
	{
		return Items.IndexOfByPredicate([SlotIndex](const FItemSlotInfo &SlotInfo) {
			return SlotInfo.SlotIndex == SlotIndex;
		});
	}

	// Utility to get the ItemIndex of a stackable slot
	FORCEINLINE int32 GetStackableSlotIndex(const FName &ItemID, int32 StackSize)
	{
		for (const FItemSlotInfo &SlotInfo : Items)
		{
			if (SlotInfo.ItemID.IsEqual(ItemID) && SlotInfo.StackSize + StackSize <= SlotInfo.MaxStackSize)
			{
				return SlotInfo.SlotIndex;
			}
		}
		return INDEX_NONE;
	}

	// Utility to get the ItemIndex of an item that matches ItemID and has room for StackSize
	FORCEINLINE int32 GetStackableItemsIndex(const FName &ItemID, int32 StackSize)
	{
		return Items.IndexOfByPredicate([ItemID, StackSize](const FItemSlotInfo &SlotInfo) {
			return (SlotInfo.ItemID.IsEqual(ItemID) && SlotInfo.StackSize + StackSize <= SlotInfo.MaxStackSize);
		});
	}

	// Utility to check if the inventory is full
	FORCEINLINE bool IsFull()
	{
		return Items.Num() == Slots;
	}

	// Utility to resize the inventory slot count
	bool ResizeInventory(int32 NewRows, int32 NewColumns);

private:
	
	TArray<int32> _openSlots;
	
};
