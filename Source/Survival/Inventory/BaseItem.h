// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "BaseItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SURVIVAL_API UBaseItem : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FName ID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	class UStaticMesh *WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	class UTexture2D *Icon;


	UBaseItem();
};
