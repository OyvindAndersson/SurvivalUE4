// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "InventoryComponent.h"
#include "BaseItem.h"
#include "BaseWeaponItem.h"
#include "ProjectileWeapon.h"

//////////////////////////////////////////////////////////////////////////
// FInventoryItemSlotInfo

bool FItemSlotInfo::ItemTypeClassIsValid()
{
	return (ItemTypeClass == nullptr ? false : true);
}

bool FItemSlotInfo::ItemTypeRefIsValid()
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

	Rows = 4;
	Columns = 2;

	// Set inventory slot count
	Slots = Rows * Columns;

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

// Called from pawn when adding a picked-up item
bool UInventoryComponent::AddItem(const FName &ItemID, int32 NewStackSize, EItemType ItemType, TSubclassOf<class UBaseItem> ItemTypeClass)
{
	int32 StackableItemsIndex = GetStackableItemsIndex(ItemID, NewStackSize);

	// Are we adding to a slot, or creating a new?
	if (StackableItemsIndex != INDEX_NONE )
	{
		Items[StackableItemsIndex].StackSize += NewStackSize;


		return true;
	}
	else if(!IsFull())
	{
		// Only create a new instance of the item if we don't have one in inventory.
		UBaseItem *NewItem = NewObject<UBaseItem>(this, ItemTypeClass);
		if (!NewItem || !NewItem->IsValidLowLevel())
		{
			UE_LOG(InventorySystemLog, Error, TEXT("AddItem : Failed to create instance of UBaseItem!"));
			return false;
		}

		// Create new item slot info
		FItemSlotInfo newSlotInfo(ItemID, GetOpenSlotIndex(), NewStackSize, NewItem->MaxStackSize, ItemTypeClass, NewItem);
		SetInSlot(newSlotInfo);

		// Broadcast the new item addition event to listeners
		OnItemSlotAddedDelegate.Broadcast(newSlotInfo);

		return true;
	}
	else
	{
		// Cant add this. No room nor stack space.
		UE_LOG(InventorySystemLog, Warning, TEXT("No room or stack-space to add item."));
		return false;
	}
}

// Called when dropping item from UI
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


bool UInventoryComponent::ResizeInventory(int32 NewRows, int32 NewColumns)
{
	if (NewRows * NewColumns < Items.Num())
	{
		// We can't discard items that occupy slots
		UE_LOG(InventorySystemLog, Warning, TEXT("Can't downsize inventory; slots beyond new size are occupied."));
		return false;
	}
	int32 oldSlotCount = Slots;
	// Resize slots
	Rows = NewRows;
	Columns = NewColumns;
	Slots = Rows * Columns;

	// TODO: Go through _openSlots and make sure
	// we only open new slots from the resize
	TArray<int32> OccupiedSlots;
	for (const FItemSlotInfo &SlotInfo : Items)
	{
		OccupiedSlots.Add(SlotInfo.SlotIndex);
	}
	OccupiedSlots.Sort();

	// If we're just adding new slots we can start at the end
	if (Slots > oldSlotCount)
	{
		for (int i = oldSlotCount; i < Slots; i++)
		{
			_openSlots.AddUnique(i);
		}
		return true;
	}
	else
	{
		// We are downsizing, simply open all slots and set items to
		// new slots in the order they were
		_openSlots.Empty();
		for (int i = 0; i < Slots; i++)
		{
			_openSlots.Add(i);
		}
		// Sort by slotindex to get the same order in the inventory
		Items.Sort([](const FItemSlotInfo &SlotA, const FItemSlotInfo &SlotB) {
			return SlotA.SlotIndex < SlotB.SlotIndex;
		});
		// Occupy the slots necessary and remove the occupied slot.
		for (int i = 0; i < Items.Num(); i++)
		{
			Items[i].SlotIndex = _openSlots[i];
			_openSlots.RemoveAt(i);
		}

		return true;
	}
}
