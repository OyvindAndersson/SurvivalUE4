// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "SurvivalGameMode.generated.h"

UCLASS(minimalapi)
class ASurvivalGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASurvivalGameMode();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory System Manager")
	class UInventorySystemManager *InventorySystemManager;


public:

	virtual void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;
};



