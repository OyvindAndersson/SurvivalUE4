// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "UnrealEd.h"
#include "ItemCraftRecipe.generated.h"

USTRUCT(BlueprintType)
struct FItemCraftRecipeData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	TSubclassOf<class UBaseItem> ItemA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	TSubclassOf<class UBaseItem> ItemB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	TSubclassOf<class UBaseItem> YieldItem;
};

UCLASS()
class UItemCraftRecipeFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual UObject *FactoryCreateNew(UClass *Class, UObject *InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SURVIVAL_API UItemCraftRecipe : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	FName ItemAID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	FName ItemBID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	int32 YieldStackSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Craft Recipe")
	TSubclassOf<class UBaseItem> YieldTypeClass;

	// Does a quick check if items with AID and BID can be crafted here
	FORCEINLINE bool CanCraftFrom(const FName &CandidateAID, const FName &CandidateBID)
	{
		// No guarantee which "direction" the args are passed in, so we check for both combinations
		// as order does not matter.
		return (CandidateAID.IsEqual(ItemAID) && CandidateBID.IsEqual(ItemBID)) || 
			(CandidateAID.IsEqual(ItemBID) && CandidateBID.IsEqual(ItemAID));
	}
	
};
