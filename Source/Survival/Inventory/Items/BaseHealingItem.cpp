// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "SurvivalCharacter.h"
#include "BaseHealingItem.h"


bool UBaseHealingItem::OnUse_Implementation(class ASurvivalCharacter *Target)
{
	if (!Target || !Target->IsValidLowLevel())
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("OnUse_Imp..: Target not valid!"));
		return false;
	}

	Target->TakeHeal(this->HealAmount, this);
	return true;
}

