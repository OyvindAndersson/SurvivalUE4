// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableInterface.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class SURVIVAL_API UUsableInterface : public UInterface
{
	GENERATED_BODY()
};

class IUsableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Usable Item Interface")
	bool OnUse(class ASurvivalCharacter *Target);

};
