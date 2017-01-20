// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "InventoryComponent.h"
#include "InventorySystemManager.h"
#include "SurvivalCharacter.h"
#include "ItemCraftRecipe.h"
#include "BaseItem.h"
#include "BaseWeaponItem.h"
#include "Items/BaseAmmoItem.h"

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

const FItemSlotInfo FItemSlotInfo::InvalidSlot = FItemSlotInfo(FName(TEXT("INVALID")), INDEX_NONE, 0, 0, nullptr, nullptr);

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
	EquippedWeapon = nullptr;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	CharOwner = Cast<ASurvivalCharacter>(GetOuter());
	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

// Called when an item in the slot is used
bool UInventoryComponent::UseItem(int32 Slot, ASurvivalCharacter *Target)
{
	FItemSlotInfo *SlotInfo = GetItemInSlot(Slot);
	if ( SlotInfo == NULL || !IsSlotValidLowLevel(*SlotInfo) )
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Not valid item in slot %d"), Slot);
		return false;
	}

	UBaseItem *Item = SlotInfo->ItemTypeReference;

	// Check if this item implements the Usable interface. 
	if (Item->GetClass()->ImplementsInterface(UUsableInterface::StaticClass()) )
	{
		if (IUsableInterface::Execute_OnUse(Item, Target))
		{
			SlotInfo->StackSize -= 1;
			// Not all items can be dropped if stacksize depletes
			if (SlotInfo->StackSize <= 0 && SlotInfo->ItemTypeReference->CanDrop)
			{
				DropItem(SlotInfo->SlotIndex, INDEX_NONE); // Drop entire item - stack is empty
			}
			return true;
		}
		else
		{
			UE_LOG(InventorySystemLog, Verbose, TEXT("HandleItemUsed[IUsableInterface::OnUse] : Item was not used!"));
			return false;
		}
	}
	else
	{
		UE_LOG(InventorySystemLog, Warning, TEXT("Item ['%s'] does not implement UsableInterface."), *Item->ID.ToString());
		return false;
	}
}

// Called when an item in the slot is being equipped
void UInventoryComponent::EquipItem(int32 Slot, ASurvivalCharacter *Target)
{
	
	FItemSlotInfo *SlotInfo = GetItemInSlot(Slot);
	if (SlotInfo == NULL || !IsSlotValidLowLevel(*SlotInfo))
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Not valid item in slot %d"), Slot);
		return;
	}

	UBaseItem *Item = SlotInfo->ItemTypeReference;

	EItemType ItemType = Item->GetItemType();
	if (ItemType == EItemType::IT_Item)
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("EquipItem : Trying to equip item type : IT_Item. Not equipable."));
		return;
	}
	else if (ItemType == EItemType::IT_Weapon && Target != nullptr && Target->IsValidLowLevel())
	{
		// We have a weapon
		UBaseWeaponItem *Weapon = Cast<UBaseWeaponItem>(Item);
		if (!Weapon || !Weapon->IsValidLowLevel())
		{
			UE_LOG(SurvivalDebugLog, Warning, TEXT("EquipItem : Not subclass of UBaseWeaponItem!."));
		}

		// SET EQUIPPED WEAPON
		EquippedWeapon = Weapon;

		// Handle equip on character
		Target->HandleEquipWeapon(EquippedWeapon);
	}
}

// Unequips the equipped item, if any
void UInventoryComponent::UnEquipItem()
{
	EquippedWeapon = nullptr;
}

// Called when equipped weapon must reload
int32 UInventoryComponent::FindAmmoItemInSlot(EAmmoType AmmoType)
{
	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i].ItemTypeRefIsValid())
		{
			UBaseAmmoItem *AmmoItem = Cast<UBaseAmmoItem>(Items[i].ItemTypeReference);
			if (AmmoItem != nullptr && AmmoItem->AmmoType == AmmoType)
			{
				// We found our ammo type. Return the slot index
				return Items[i].SlotIndex;
			}
		}
		
	}

	return INDEX_NONE;
}

// Called to reload the equipped weapon, if we have the ammo for it
bool UInventoryComponent::ReloadEquippedWeapon()
{
	if (EquippedWeapon == nullptr || !EquippedWeapon->IsValidLowLevel())
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("No weapons equipped. Failed to reload."));
		return false;
	}

	int32 AmmoSlot = FindAmmoItemInSlot(EquippedWeapon->AmmoType);
	FItemSlotInfo *AmmoSlotInfo = GetItemInSlot(AmmoSlot);
	if (AmmoSlotInfo != nullptr)
	{
		// If we have less of the ammo stack than the max clip size,
		// everything will be spent and we must drop the ammo item
		if (AmmoSlotInfo->StackSize + EquippedWeapon->ClipSize <= EquippedWeapon->MaxClipSize)
		{
			EquippedWeapon->ClipSize += AmmoSlotInfo->StackSize;
			DropItem(AmmoSlotInfo->SlotIndex, INDEX_NONE);

			return true;
		}
		else
		{
			while (EquippedWeapon->ClipSize != EquippedWeapon->MaxClipSize)
			{
				AmmoSlotInfo->StackSize--;
				EquippedWeapon->ClipSize++;
			}
			return true;
		}
	}
	else
	{
		UE_LOG(SurvivalDebugLog, Log, TEXT("ReloadEquippedWeapon : Ammo slot info invalid. Can't reload."));
		return false;
	}
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
		// Give item ownership to character
		NewItem->GivenTo(CharOwner);

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

// Called to add an item to a specific slot, if open.
bool UInventoryComponent::AddItemToSlot(int32 SlotIndex, const FName &ItemID, int32 NewStackSize, EItemType ItemType, TSubclassOf<class UBaseItem> ItemTypeClass)
{
	int32 StackableItemsIndex = GetStackableItemsIndex(ItemID, NewStackSize);

	// Are we adding to a slot, or creating a new?
	if (StackableItemsIndex != INDEX_NONE)
	{
		Items[StackableItemsIndex].StackSize += NewStackSize;

		return true;
	}
	else if (!IsFull())
	{
		// Only create a new instance of the item if we don't have one in inventory.
		UBaseItem *NewItem = NewObject<UBaseItem>(this, ItemTypeClass);
		if (!NewItem || !NewItem->IsValidLowLevel())
		{
			UE_LOG(InventorySystemLog, Error, TEXT("AddItem : Failed to create instance of UBaseItem!"));
			return false;
		}

		if (!IsSlotOpen(SlotIndex))
		{
			UE_LOG(InventorySystemLog, Warning, TEXT("Failed to add item to specific slot '%d'; Now trying to add to an open slot..."),
				SlotIndex);
			SlotIndex = GetOpenSlotIndex();
		}

		// Create new item slot info
		FItemSlotInfo newSlotInfo(ItemID, SlotIndex, NewStackSize, NewItem->MaxStackSize, ItemTypeClass, NewItem);
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
	if (Slot == INDEX_NONE)
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot drop item at slot index '%d' ; Invalid index!"), Slot);
		return false;
	}

	int32 itemIndex = GetItemInfoIndexAtSlot(Slot);

	if (!Items.IsValidIndex(itemIndex))
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot drop item at item index '%d' ; Invalid index!"), itemIndex);
		return false;
	}
	
	// If stacksize == index_none, we're removing the entire item.
	if (StackSize < Items[itemIndex].StackSize && StackSize != INDEX_NONE)
	{
		// We're not dropping the whole thing, only a part of the stacksize
		Items[itemIndex].StackSize -= StackSize;
		return true;
	}
	else
	{
		// Important! Make sure to "open up" the inventory slot
		_openSlots.AddUnique(Items[itemIndex].SlotIndex);

		// Make sure we dont drop an equipped weapon. "unequip" it first.
		// TODO: Handle differently? Perhaps don't allow dropping equipped items..
		if (EquippedWeapon != nullptr && Items[itemIndex].ItemTypeReference != nullptr
			&& Items[itemIndex].ItemTypeReference->GetUniqueID() == EquippedWeapon->GetUniqueID())
		{
			UE_LOG(SurvivalDebugLog, Log, TEXT("Dropping equipped weapon. Removing reference."));
			
			ASurvivalCharacter *Owner = Cast<ASurvivalCharacter>(GetOwner());
			if (Owner)
			{
				Owner->UnEquip();
			}
		}
		
		// Remove from our inventory
		Items.RemoveAtSwap(itemIndex);
		return true;
	}
	return false;
	
}

// Try to craft an item
bool UInventoryComponent::CraftItem(int32 SlotA, int32 SlotB, UInventorySystemManager *InventorySystemManager)
{
	if (InventorySystemManager == nullptr || !InventorySystemManager->IsValidLowLevel())
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Inventory system manager NULL. Cannot craft item."));
		return false;
	}

	int32 ItemAIndex = GetItemInfoIndexAtSlot(SlotA);
	int32 ItemBIndex = GetItemInfoIndexAtSlot(SlotB);

	if (!Items.IsValidIndex(ItemAIndex) || !Items.IsValidIndex(ItemBIndex))
	{
		UE_LOG(InventorySystemLog, Warning, TEXT("Failed to craft item. Either invalid index: [A:%d] - [B:%d]"), ItemAIndex, ItemBIndex);
		return false;
	}

	// Get item infos for crafting
	FItemSlotInfo &ItemA = Items[ItemAIndex];
	FItemSlotInfo &ItemB = Items[ItemBIndex];

	// Can we craft them?
	const UItemCraftRecipe *NewItemCraftInfo = InventorySystemManager->CraftItem(ItemA.ItemID, ItemB.ItemID);
	if (NewItemCraftInfo == nullptr)
	{
		UE_LOG(InventorySystemLog, Warning, TEXT("Failed to craft item [%s] and [%s]. Not a valid recipe."),
			*ItemA.ItemID.ToString(), *ItemB.ItemID.ToString());
		return false;
	}
	else
	{
		// Ok, we have a valid craft. Lets:
		// 1: Remove the recipe items we used
		// 1A: Are we dropping the entire stack?
		// 1B: Are only using part of the stack?
		// 2: Instantiate the newly crafted item
		// 3: Set the crafted item in inventory if:
		// 3A: We have room for it, as we might have crafter using part of a stack only
		// 3B: We used the entire stack, so we know we have room.

		int32 OldASlot = ItemA.SlotIndex; // We will place the new item in the first item slot. Does not matter really.
		int32 OldBSlot = ItemB.SlotIndex;
		// For now, drop the entire stack. We'll add stack-specific crafting later...
		DropItem(ItemA.SlotIndex, INDEX_NONE);
		DropItem(ItemB.SlotIndex, INDEX_NONE);

		// Let blueprints in on the action for UI updates
		DroppedCraftItems(OldASlot, OldBSlot);
		

		return AddItemToSlot(OldASlot, 
			NewItemCraftInfo->YieldItemID,
			NewItemCraftInfo->YieldStackSize,
			NewItemCraftInfo->YieldItemType,
			NewItemCraftInfo->YieldTypeClass);
	}
}

// Swap slot positions
bool UInventoryComponent::SwapSlot(int32 SlotA, int32 SlotB)
{
	// No need for a swap
	if (IsSlotOpen(SlotA) && IsSlotOpen(SlotB))
	{
		return true;
	}
	else if (!IsValidSlot(SlotA) || !IsValidSlot(SlotB))
	{
		UE_LOG(InventorySystemLog, Warning, TEXT("SwapSlot : Either or both slot indices are out of range."));
		return false;
	}

	int32 ItemAIndex = INDEX_NONE, ItemBIndex = INDEX_NONE;

	// TODO: Too much?
	if (IsSlotOpen(SlotA) || IsSlotOpen(SlotB))
	{
		// One is occupied, but the other is open - so we must do some additional work
		if (IsSlotOpen(SlotA))
		{
			ItemBIndex = GetItemInfoIndexAtSlot(SlotB); // Get index for the item for the occupied slot
			if (Items.IsValidIndex(ItemBIndex))
			{
				Items[ItemBIndex].SlotIndex = SlotA; // Slot A is open, so move this item there
				_openSlots.Remove(SlotA); // "Close" the moved-to slot
				_openSlots.AddUnique(SlotB); // Open the previously occupied slot

				return true;
			}
			else
			{
				UE_LOG(InventorySystemLog, Warning, TEXT("SwapSlot : Item B index is invalid."));
				return false;
			}
		}
		else
		{
			ItemAIndex = GetItemInfoIndexAtSlot(SlotA); // Get index for the item for the occupied slot
			if (Items.IsValidIndex(ItemAIndex))
			{
				Items[ItemAIndex].SlotIndex = SlotB; // Slot B is open, move item over
				_openSlots.Remove(SlotB); // Remove from open slots list
				_openSlots.AddUnique(SlotA); // Add the previously occupied slot to open slots list

				return true;
			}
			else
			{
				UE_LOG(InventorySystemLog, Warning, TEXT("SwapSlot : Item A index is invalid."));
				return false;
			}
		}
	}
	else
	{
		// Both are occupied, simply swap index
		ItemAIndex = GetItemInfoIndexAtSlot(SlotA);
		ItemBIndex = GetItemInfoIndexAtSlot(SlotB);

		if (!Items.IsValidIndex(ItemAIndex) || !Items.IsValidIndex(ItemBIndex))
		{
			UE_LOG(InventorySystemLog, Warning, TEXT("SwapSlot : Either itemA/B index is invalid, cannot swap existing items."));
			return false;
		}

		// Swap slots
		Items[ItemAIndex].SlotIndex = SlotB;
		Items[ItemBIndex].SlotIndex = SlotA;

		return true;
	}
}

// Called to resize the inventory
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
