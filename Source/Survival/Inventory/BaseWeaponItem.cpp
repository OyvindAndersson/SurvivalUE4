// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "Weapons/WeaponState.h"
#include "Weapons/WeaponStateEquipping.h"


UBaseWeaponItem::UBaseWeaponItem(const FObjectInitializer& ObjectInitializer)
	: Super()
{
	ItemType = EItemType::IT_Weapon;
	AmmoType = EAmmoType::AT_Other;

	WeaponActor = nullptr;
}

void UBaseWeaponItem::PrintItemDebug()
{
}

