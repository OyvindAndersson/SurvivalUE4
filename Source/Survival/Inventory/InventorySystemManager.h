// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "InventorySystemManager.generated.h"

USTRUCT(BlueprintType)
struct FCraftedItemInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	TSubclassOf<class UBaseItem> ItemTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	int32 StackSize;

	FCraftedItemInfo()
	{
		ItemTypeClass = nullptr;
		StackSize = 0;
	}

	FCraftedItemInfo(TSubclassOf<class UBaseItem> InItemTypeClass, int32 InStackSize)
	{
		ItemTypeClass = InItemTypeClass;
		StackSize = InStackSize;
	}
};

static FCraftedItemInfo InvalidType;

/**
 * 
 */
UCLASS()
class SURVIVAL_API UInventorySystemManager : public UObject
{
	GENERATED_BODY()
	
public:
	UInventorySystemManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	TArray<class UItemCraftRecipe*> CraftRecipes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	class UObjectLibrary *CraftRecipeLibrary;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
	int32 LoadedCraftRecipes;

	
	void LoadAllRecipeAssets();

	void PrintAssets();

	// Create iteminfo for a crafted item that can be used to create the actual item object
	const FCraftedItemInfo CraftItem(const FName &ItemAID, const FName &ItemBID);
};
