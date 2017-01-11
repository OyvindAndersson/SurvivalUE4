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


//////////////////////////////////////////////////////////////////////////
// UInventoryComponent

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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


bool UInventoryComponent::AddItem(const FName &ItemID, int32 StackSize, TSubclassOf<class UBaseItem> ItemTypeClass)
{
	FInventoryItemSlotInfo NewSlot;
	NewSlot.ItemID = ItemID;
	NewSlot.ItemTypeClass = ItemTypeClass;
	NewSlot.StackSize = StackSize;
	NewSlot.SlotIndex = 0;

	for (const FInventoryItemSlotInfo &SlotInfo : Items)
	{

	}


	return true;
}

