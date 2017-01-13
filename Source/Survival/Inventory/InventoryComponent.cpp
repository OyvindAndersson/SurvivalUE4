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
