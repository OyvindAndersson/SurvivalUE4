// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "Weapons/WeaponState.h"
#include "Weapons/WeaponStateEquipping.h"


UBaseWeaponItem::UBaseWeaponItem(const FObjectInitializer& ObjectInitializer)
	: Super()
{
	ItemType = EItemType::IT_Weapon;
	AmmoType = EAmmoType::AT_Other;

	InactiveState = ObjectInitializer.CreateDefaultSubobject<UWeaponState>(this, TEXT("StateInactive"));
	EquippingState = ObjectInitializer.CreateDefaultSubobject<UWeaponState>(this, TEXT("StateEquipping"));
	IdleState = ObjectInitializer.CreateDefaultSubobject<UWeaponState>(this, TEXT("StateIdle"));
	FiringState = ObjectInitializer.CreateDefaultSubobject<UWeaponState>(this, TEXT("FiringState"));
	ReloadingState = ObjectInitializer.CreateDefaultSubobject<UWeaponState>(this, TEXT("ReloadingState"));
	CurrentState = InactiveState;
	WeaponActor = nullptr;
}

void UBaseWeaponItem::BeginPlay()
{
	if (CurrentState == InactiveState)
	{
		GotoState(InactiveState);
	}
	checkSlow(CurrentState != NULL);
}


void UBaseWeaponItem::Tick(float DeltaTime)
{
	// This weapon should not be active without a owner
	if (CurrentState != InactiveState && (CharOwner == NULL || CharOwner->IsPendingKill()) && CurrentState != NULL)
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("%s lost Owner while active (state %s)"), *GetName(), *GetNameSafe(CurrentState));
		GotoState(InactiveState);
	}

	if (CurrentState != InactiveState)
	{
		CurrentState->Tick(DeltaTime);
	}
}

void UBaseWeaponItem::GotoState(class UWeaponState *NewState)
{
	// Only call StateChanged when the state type actually changes
	// End current state
	// begin new state

	if (NewState == NULL || !NewState->IsIn(this))
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("Attempt to send %s to invalid state %s"), *GetName(), *GetFullNameSafe(NewState));
	}
	else
	{
		// We only do something if the state changes entirely
		if (CurrentState != NewState)
		{
			UWeaponState *PrevState = CurrentState;
			// Current state might be NULL if this is initial Goto
			if (CurrentState != NULL)
			{
				CurrentState->EndState(); // May trigger another GotoState call
			}
			// Make sure the EndState did not trigger a new state change (Will be equal if not)
			if (CurrentState == PrevState)
			{
				CurrentState = NewState;
				CurrentState->BeginState(PrevState);
				StateChanged();
			}
		}
	}
}

void UBaseWeaponItem::PrintItemDebug()
{
}

