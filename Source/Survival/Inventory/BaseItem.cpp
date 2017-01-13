// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "BaseItem.h"


UBaseItem::UBaseItem()
{
	ID = FName("NO_ID");
	Name = FText();

	WorldMesh = nullptr;
	DetailMesh = nullptr;
	Icon = nullptr;

	MaxStackSize = 0;
	Value = 0;

	ItemType = EItemType::IT_Item;
}

