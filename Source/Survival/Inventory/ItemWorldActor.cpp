// Fill out your copyright notice in the Description page of Project Settings.

#include "Survival.h"
#include "BaseItem.h"
#include "ItemWorldActor.h"


// Sets default values
AItemWorldActor::AItemWorldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetSphereRadius(100.f);

	ItemTypeReference = NULL;
	ItemTypeClass = NULL;

}

// Called when the game starts or when spawned
void AItemWorldActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemWorldActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


void AItemWorldActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (ItemTypeClass != NULL)
	{
		if (ItemTypeReference != NULL)
		{
			StaticMesh->SetStaticMesh(0);
			ItemTypeReference->DestroyNonNativeProperties();
			ItemTypeReference->MarkPendingKill();

			GetWorld()->ForceGarbageCollection(true);
		}

		// Create the item associated with this pickup actor
		ItemTypeReference = NewObject<UBaseItem>(this, ItemTypeClass);
		
		// Set mesh to whatever is referenced in the base item def
		StaticMesh->SetStaticMesh(ItemTypeReference->WorldMesh);
		if (StaticMesh)
		{
			// For now, just set this actors' pickup radius to the bounds of the mesh
			// and update location of it to the mesh.
			SphereComponent->SetSphereRadius(StaticMesh->Bounds.GetSphere().W);
			SphereComponent->SetWorldLocation(StaticMesh->GetComponentLocation());
		}
		
	}
}