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

	// ...
	Slots = 8;
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
		// Only create a new instance of the item if we don't have one in inventory.
		UBaseItem *NewItem = NewObject<UBaseItem>(this, ItemTypeClass);
		if (!NewItem || !NewItem->IsValidLowLevel())
		{
			return false;
		}
	}
	else if(Items.Num() < Slots)
	{
		// We have room
	}
	else
	{
		// Cant add this. No room nor stack space.
	}
	

	FInventoryItemSlotInfo NewSlot;
	NewSlot.ItemID = ItemID;
	NewSlot.ItemTypeClass = ItemTypeClass;
	NewSlot.ItemTypeReference = nullptr;
	NewSlot.StackSize = NewStackSize;
	NewSlot.SlotIndex = 0;


	return true;
}

