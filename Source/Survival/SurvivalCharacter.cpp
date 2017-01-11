// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Survival.h"
#include "SurvivalCharacter.h"
#include "SurvivalProjectile.h"
#include "Inventory/BaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemWorldActor.h"
#include "Utility/UtilityFunctionsLibrary.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASurvivalCharacter

ASurvivalCharacter::ASurvivalCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Create inventory component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Set tracedistance for player actions (i.e: the distance the player must be from the subject of action (doors, pickup items etc.))
	ActionTraceDistance = 160.0f;

}

void ASurvivalCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	Mesh1P->SetHiddenInGame(false, true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASurvivalCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);


	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASurvivalCharacter::OnFire);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &ASurvivalCharacter::OnAction);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASurvivalCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASurvivalCharacter::LookUpAtRate);
}

void ASurvivalCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ASurvivalProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ASurvivalCharacter::OnAction()
{
	if (FirstPersonCameraComponent == nullptr)
	{
		UE_LOG(SurvivalDebugLog, Error, TEXT("FirstPersonCamera component null! Cannot do trace."));
		return;
	}

	UWorld *World = GetWorld();

	// Start trace from the FP camera, and set it a bit in front of the camera location to better match the arm reach.
	const FRotator StartRotation = GetControlRotation();
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation() + (FirstPersonCameraComponent->GetForwardVector() * 20.0f);
	const FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * ActionTraceDistance);
	FHitResult HitResult;

	//--------------
	// Do the trace

	if (UUtilityFunctionsLibrary::TraceLine(World, this, Start, End, HitResult, ECollisionChannel::ECC_WorldStatic, false, true))
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("Trace hit: %s"), *HitResult.GetActor()->GetName());
		
		// #1
		// Check if we found a world item pickup actor
		AItemWorldActor *ItemPickupActor = Cast<AItemWorldActor>(HitResult.GetActor());
		if (ItemPickupActor != nullptr && ItemPickupActor->ItemTypeReference != nullptr)
		{
			// The world item pickup actor had a valid reference to an item. Handle it individually
			HandlePickupItem(ItemPickupActor);
		}

		// #2
		// Check if we instead found a weapon pickup point
		/*
		AWeaponWorldActor *WeaponPickupActor = Cast<AWeaponWorldActor>(HitResult.GetActor());
		if( WeaponPickupActor != nullptr && WeaponPickupActor->WeaponTypeReference != nullptr)
		{
			HandlePickupWeapon(WeaponPickupActor->WeaponTypeReference);
		}
		*/

		// #3
		// Is it a door?

	}
	else
	{
#if IN_DEVMODE == 1
		UE_LOG(SurvivalDebugLog, Warning, TEXT("Trace hit: NOTHING"));
#endif
	}
}

void ASurvivalCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASurvivalCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASurvivalCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASurvivalCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//////////////////////////////////////////////////////////////////////////
// Action handling

void ASurvivalCharacter::HandlePickupItem(AItemWorldActor *ItemPickup)
{
	UBaseItem *Item = ItemPickup->ItemTypeReference;

	UE_LOG(InventorySystemLog, Warning, TEXT("Handling pickup for item Name: ['%s'] with ID: ['%s'] - Class: '%s'"), 
		*Item->Name.ToString(), *Item->ID.ToString(), *Item->GetClass()->GetPathName());

	if (InventoryComponent->AddItem(Item->ID, ItemPickup->StackSize, ItemPickup->ItemTypeClass))
	{

	}
	else
	{

	}

	// TODO: Make ready item and add to inventory, serverside.
	// InventoryComponent handles all add/checks
}