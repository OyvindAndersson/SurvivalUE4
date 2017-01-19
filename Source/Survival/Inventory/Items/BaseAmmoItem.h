// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/BaseItem.h"
#include "BaseAmmoItem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVAL_API UBaseAmmoItem : public UBaseItem
{
	GENERATED_BODY()
	
public:
	UBaseAmmoItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseWeapon)
	EAmmoType AmmoType;
	
};
