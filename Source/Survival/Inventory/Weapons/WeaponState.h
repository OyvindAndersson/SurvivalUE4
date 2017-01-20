// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponState.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, CustomConstructor, EditInlineNew, Within=BaseWeaponItem)
class SURVIVAL_API UWeaponState : public UObject
{
	GENERATED_UCLASS_BODY()

	UWeaponState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	{}
	
public:
	/*ASurvivalCharacter *GetCharOwner();
	virtual UWorld *GetWorld() const override;*/

	virtual void BeginState(const UWeaponState *PrevState) { }
	virtual void EndState() { }
	virtual void Tick(float DeltaTime){ }
};
