// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "BaseItem.h"


UBaseItem::UBaseItem()
{
	WorldMesh = NULL;
	Icon = NULL;
	MaxStackSize = 0;
	Value = 0;
	ID = FName("NO_ID");
	Name = FText();
}

