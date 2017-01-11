// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "DrawDebugHelpers.h"
#include "UtilityFunctionsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVAL_API UUtilityFunctionsLibrary : public UObject
{
	GENERATED_BODY()
	
public:

	/** 
	* Does a single line trace with one possible ignore actor.
	* Wrapper function to utilize the standard line trace by channel
	*/
	static FORCEINLINE bool TraceLine(UWorld *World,
		AActor *ActorToIgnore,
		const FVector &Start, const FVector &End,
		FHitResult &HitOut, ECollisionChannel CollisionChannel = ECollisionChannel::ECC_Pawn,
		bool ReturnPhysMat = false,
		bool DrawDebugLines = false)
	{
		if (!World)
		{
		#if IN_DEVMODE==1
			UE_LOG(UtilityLib, Warning, TEXT("World NULL in TraceLine"));
		#endif
			return false;
		}

		FCollisionQueryParams TraceParams;

		const FName TraceTag("SurvivalTraceTag");
		if (DrawDebugLines)
		{
			World->DebugDrawTraceTag = TraceTag;
			TraceParams.TraceTag = TraceTag;
		}

		TraceParams.AddIgnoredActor(ActorToIgnore);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;

		World->LineTraceSingleByChannel(HitOut, Start, End, CollisionChannel, TraceParams);


		return HitOut.GetActor() != NULL;
	}
	
	
};
