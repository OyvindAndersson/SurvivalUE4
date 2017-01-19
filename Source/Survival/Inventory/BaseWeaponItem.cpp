// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "BaseWeaponItem.h"


UBaseWeaponItem::UBaseWeaponItem()
{
	ItemType = EItemType::IT_Weapon;
	AmmoType = EAmmoType::AT_Other;
}

