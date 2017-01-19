// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "ItemCraftRecipe.h"
#include "BaseItem.h"
#include "Engine/ObjectLibrary.h"
#include "InventorySystemManager.h"


UInventorySystemManager::UInventorySystemManager()
{
	LoadedCraftRecipes = 0;
	CraftRecipeLibrary = NULL;
}

void UInventorySystemManager::PrintAssets()
{
	TArray<FAssetData> AssetDatas;
	CraftRecipeLibrary->GetAssetDataList(AssetDatas);

	UE_LOG(InventorySystemLog, Warning, TEXT("ASSETS LOADED: %d"), LoadedCraftRecipes);

	for(int i=0; i < AssetDatas.Num(); i++)
	{
		FAssetData &AssetData = AssetDatas[i];

		UItemCraftRecipe *Recipe = Cast<UItemCraftRecipe>(AssetData.GetAsset());
		if (Recipe)
		{
			UE_LOG(InventorySystemLog, Warning, TEXT("Item A: %s, Item B: %s"), 
				*Recipe->ItemAID.ToString(), *Recipe->ItemBID.ToString());
		}

		FString OutFullName = AssetData.GetClass()->GetName();
		FString isLoadedSz = (AssetData.IsAssetLoaded() ? FString(TEXT("IS LOADED")) : FString(TEXT("NOT LOADED")));
		UE_LOG(InventorySystemLog, Warning, TEXT("Asset | Name: '%s', Asset Class: '%s' - [%s]"),
			*AssetData.AssetName.ToString(), *OutFullName, *isLoadedSz);
	}
}

void UInventorySystemManager::LoadAllRecipeAssets()
{
	if (CraftRecipeLibrary == NULL)
	{
		CraftRecipeLibrary = UObjectLibrary::CreateLibrary(UItemCraftRecipe::StaticClass(), true, true);
		CraftRecipeLibrary->AddToRoot();
	}
	
	CraftRecipeLibrary->ClearLoaded();
	CraftRecipeLibrary->LoadAssetDataFromPath(TEXT("/Game/Inventory/CraftingRecipes"));
	CraftRecipeLibrary->LoadAssetsFromAssetData(); // Fully load

	LoadedCraftRecipes = CraftRecipeLibrary->GetAssetDataCount();

	// Load all recipe instances.

	TArray<FAssetData> AssetDatas;
	CraftRecipeLibrary->GetAssetDataList(AssetDatas);
	for (int i = 0; i < AssetDatas.Num(); i++)
	{
		FAssetData &AssetData = AssetDatas[i];

		UItemCraftRecipe *Recipe = Cast<UItemCraftRecipe>(AssetData.GetAsset());
		if (Recipe)
		{
			UBaseItem *Temp = NewObject<UBaseItem>(this, Recipe->YieldTypeClass);
			if (Temp && Temp->IsValidLowLevel())
			{
				Recipe->YieldItemID = Temp->ID;
				Temp->MarkPendingKill();
			}
			CraftRecipes.AddUnique(Recipe);
		}
	}
}

/*
const FCraftedItemInfo UInventorySystemManager::CraftItem(const FName &ItemAID, const FName &ItemBID)
{
	
	// Go through our recipes and see if we can craft anything out of these
	UItemCraftRecipe *CurRecipe = NULL;
	for (int i = 0; i < CraftRecipes.Num(); i++)
	{
		CurRecipe = CraftRecipes[i];
		if (!CurRecipe || !CurRecipe->IsValidLowLevel())
			continue;

		if (CurRecipe->CanCraftFrom(ItemAID, ItemBID))
		{
			FCraftedItemInfo NewItem(CurRecipe->YieldTypeClass, CurRecipe->YieldStackSize);
			return NewItem;
		}
	}

	return InvalidCraftedItemInfo;
}*/

const UItemCraftRecipe *UInventorySystemManager::CraftItem(const FName &ItemAID, const FName &ItemBID)
{
	// Go through our recipes and see if we can craft anything out of these
	UItemCraftRecipe *CurRecipe = NULL;
	for (int i = 0; i < CraftRecipes.Num(); i++)
	{
		CurRecipe = CraftRecipes[i];
		if (!CurRecipe || !CurRecipe->IsValidLowLevel())
			continue;

		if (CurRecipe->CanCraftFrom(ItemAID, ItemBID))
		{
			return CurRecipe;
		}
	}

	return nullptr;
}
