// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "InventoryComponent.h"
#include "BaseItem.h"

//////////////////////////////////////////////////////////////////////////
// FInventoryItemSlotInfo

bool FInventoryItemSlotInfo::ItemTypeClassIsValid()
{
	return (ItemTypeClass == nullptr ? false : true);
}

bool FInventoryItemSlotInfo::ItemTypeRefIsValid()
{
	if (ItemTypeReference == nullptr)
		return false;

	return ItemTypeReference->IsValidLowLevel();
}


//////////////////////////////////////////////////////////////////////////
// UInventoryComponent

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Set inventory slot count
	Slots = 8;

	// Fill our array that keeps track of used slot indices
	// with the indices for these slots
	for (int i = 0; i < Slots; i++)
	{
		_openSlots.Add(i);
	}

	// Provide slack for our inventory
	Items.Empty(Slots);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}


bool UInventoryComponent::AddItem(const FName &ItemID, int32 NewStackSize, TSubclassOf<class UBaseItem> ItemTypeClass)
{
	int32 AvailableSlot = INDEX_NONE;
	for (const FInventoryItemSlotInfo &SlotInfo : Items)
	{
		// Check if we have any items with available stack-space, and have room for the new stacksize.
		if (SlotInfo.StackSize + NewStackSize <= SlotInfo.MaxStackSize )
		{
			AvailableSlot = SlotInfo.SlotIndex;
			break;
		}
	}

	// Are we adding to a slot, or creating a new?
	if (AvailableSlot != INDEX_NONE)
	{
		// We're simply adding to the stacksize of another item in the inventory. 
		// Find it, add the new stacksize and return
		FInventoryItemSlotInfo SlotInfo = Items[GetItemInfoIndexAtSlot(AvailableSlot)];
		SlotInfo.StackSize += NewStackSize;

		return true;
	}
	else if(Items.Num() < Slots)
	{
		// Only create a new instance of the item if we don't have one in inventory.
		UBaseItem *NewItem = NewObject<UBaseItem>(this, ItemTypeClass);
		if (!NewItem || !NewItem->IsValidLowLevel())
		{
			return false;
		}

		FInventoryItemSlotInfo newSlotInfo;
		newSlotInfo.SlotIndex = GetOpenSlotIndex();
		newSlotInfo.ItemID = ItemID;
		newSlotInfo.ItemTypeClass = ItemTypeClass;
		newSlotInfo.MaxStackSize = NewItem->MaxStackSize;
		newSlotInfo.ItemTypeReference = NewItem;
		// TODO: Add the rest of the stacksize as their own slot if > max?
		newSlotInfo.StackSize = (NewStackSize > NewItem->MaxStackSize ? NewItem->MaxStackSize : NewStackSize);

		// Add item to inventory
		Items.Add(newSlotInfo);

		// Make sure to remove the now occupied slot index from the open slots array
		// This will be added back when the item is removed/depleted in the inventory
		_openSlots.Remove(newSlotInfo.SlotIndex);

		return true;
	}
	else
	{
		// Cant add this. No room nor stack space.
		UE_LOG(InventorySystemLog, Warning, TEXT("No room or stack-space to add item."));
		return false;
	}
}

bool UInventoryComponent::DropItem(int32 Slot, int32 StackSize)
{
	if (Slot == INDEX_NONE || !Items.IsValidIndex(Slot))
		return false;

	int32 itemIndex = GetItemInfoIndexAtSlot(Slot);
	
	if (StackSize < Items[itemIndex].StackSize)
	{
		// We're not dropping the whole thing, only a part of the stacksize
		Items[itemIndex].StackSize -= StackSize;
		return true;
	}
	else
	{
		// Important! Make sure to "open up" the inventory slot
		_openSlots.AddUnique(Items[itemIndex].SlotIndex);
		
		// Remove from our inventory
		Items.RemoveAtSwap(itemIndex);
		return true;
	}
	return false;
	
}

int32 UInventoryComponent::GetOpenSlotIndex() const
{
	// If no slots are available, we return invalid index
	if (_openSlots.Num() == 0)
		return INDEX_NONE;

	return _openSlots[0]; // Return the first available slot
}

int32 UInventoryComponent::GetItemInfoIndexAtSlot(int SlotIndex) const
{
	return Items.IndexOfByPredicate([SlotIndex](const FInventoryItemSlotInfo &SlotInfo) {
		return SlotInfo.SlotIndex == SlotIndex;
	});
}
