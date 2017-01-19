// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/BaseItem.h"
#include "Inventory/Decorators/UsableInterface.h"
#include "BaseHealingItem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVAL_API UBaseHealingItem : public UBaseItem, public IUsableInterface
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Healing Item")
	float HealAmount;

	// IUsableInterface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Usable Interface")
	bool OnUse(class ASurvivalCharacter *Target);
	virtual bool OnUse_Implementation(class ASurvivalCharacter *Target) override;
	// End IUsableInterface
	
};
