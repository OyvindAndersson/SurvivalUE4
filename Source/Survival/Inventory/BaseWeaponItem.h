// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/BaseItem.h"
#include "BaseWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVAL_API UBaseWeaponItem : public UBaseItem
{
	GENERATED_BODY()

public:
	UBaseWeaponItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileWeapon)
	int32 ClipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileWeapon)
	int32 MaxClipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseWeapon)
	EWeaponType WeaponType;
};