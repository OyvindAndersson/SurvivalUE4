// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Survival.h"
#include "SurvivalGameMode.h"
#include "SurvivalGameStateBase.h"
#include "SurvivalPlayerState.h"
#include "SurvivalHUD.h"
#include "SurvivalCharacter.h"

ASurvivalGameMode::ASurvivalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASurvivalHUD::StaticClass();
	GameStateClass = ASurvivalGameStateBase::StaticClass();
	PlayerStateClass = ASurvivalPlayerState::StaticClass();
}
