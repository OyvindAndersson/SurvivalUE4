// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "ItemCraftRecipe.h"

/////////////////////////////////////////////////////
// UMyObjectFactory

UItemCraftRecipeFactory::UItemCraftRecipeFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UItemCraftRecipe::StaticClass();
}

UObject* UItemCraftRecipeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	//UItemCraftRecipe* NewObjectAsset = NewObject(Class, InParent, Name, Flags | RF_Transactional);
	UItemCraftRecipe* NewObjectAsset = NewObject<UItemCraftRecipe>(InParent, Class, Name, Flags | RF_Transactional);
	return NewObjectAsset;
}

/////////////////////////////////////////////////////
// UItemCraftRecipe

UItemCraftRecipe::UItemCraftRecipe()
{

}

