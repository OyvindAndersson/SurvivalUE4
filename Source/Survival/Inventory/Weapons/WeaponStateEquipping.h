// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/Weapons/WeaponState.h"
#include "WeaponStateEquipping.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class SURVIVAL_API UWeaponStateEquipping : public UWeaponState
{
	GENERATED_UCLASS_BODY()
	
		UWeaponStateEquipping(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{}
	
	
};
