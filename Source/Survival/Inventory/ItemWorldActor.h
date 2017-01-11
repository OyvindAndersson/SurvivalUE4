// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ItemWorldActor.generated.h"

UCLASS()
class SURVIVAL_API AItemWorldActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Inventory)
	class UStaticMeshComponent *StaticMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Inventory)
	class USphereComponent *SphereComponent;

public:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TSubclassOf<class UBaseItem> ItemTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	class UBaseItem *ItemTypeReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 StackSize;
	
	// Sets default values for this actor's properties
	AItemWorldActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
};
