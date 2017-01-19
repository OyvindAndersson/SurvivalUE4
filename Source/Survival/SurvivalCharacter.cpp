// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Survival.h"
#include "SurvivalCharacter.h"
#include "SurvivalProjectile.h"
#include "SurvivalGameMode.h"
#include "Inventory/InventorySystemManager.h"
#include "Inventory/BaseItem.h"
#include "Inventory/Items/BaseHealingItem.h"
#include "Inventory/BaseWeaponItem.h"
#include "Inventory/ProjectileWeapon.h"

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
	PrimaryActorTick.bCanEverTick = true;

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

	// Player stats
	Health = 100.0f;
	MaxHealth = 100.0f;
	Stamina = 100.0f;

	bIsReloading = false;
	ReloadTime = 2.0f;
	ReloadTimeAccum = 0.0f;

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

void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsReloading)
	{
		ReloadTimeAccum += DeltaTime;
		if (ReloadTimeAccum >= ReloadTime)
		{
			OnEndReload();
		}
	}
}

float ASurvivalCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;

		// If the damage depletes our health set our lifespan to zero - which will destroy the actor  
		if (Health <= 0.f)
		{
			SetLifeSpan(0.001f);
		}
	}

	return ActualDamage;
}

float ASurvivalCharacter::TakeHeal_Implementation(float HealAmount, class UBaseHealingItem *HealCauser)
{
	if (HealAmount >= MaxHealth)
	{
		Health = MaxHealth;
		return HealAmount;
	}

	// TODO: Future idea: Perhaps some heal items will negatively/positively affect the player when he
	// has some form of status ailments. Do these checks here and alter the heal amount respectively.

	Health += HealAmount;
	return HealAmount;
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
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASurvivalCharacter::OnStartReload);
	PlayerInputComponent->BindAction("ShowInventory", IE_Pressed, this, &ASurvivalCharacter::OnShowInventory);

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
	if (bIsReloading)
	{
		UE_LOG(SurvivalDebugLog, Log, TEXT("Weapon is reloading. Cant fire yet..."));
		return;
	}

	if (InventoryComponent && InventoryComponent->EquippedWeapon && InventoryComponent->EquippedWeapon->IsValidLowLevel())
	{
		if (InventoryComponent->EquippedWeapon->ClipSize > 0)
		{
			InventoryComponent->EquippedWeapon->ClipSize--; // shot fired
		}
		else
		{
			UE_LOG(SurvivalDebugLog, Log, TEXT("OnFire : Out of ammo!"));
			return;
		}
	}
	else
	{
		UE_LOG(SurvivalDebugLog, Log, TEXT("OnFire : No weapon equipped! Cannot fire.."));
		return;
	}
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

void ASurvivalCharacter::OnShowInventory()
{
	
}

void ASurvivalCharacter::OnStartReload()
{
	InventoryComponent->PrintInventory();
	bIsReloading = InventoryComponent->ReloadEquippedWeapon();
	UE_LOG(SurvivalDebugLog, Error, TEXT("RELOAD START"));
}

void ASurvivalCharacter::OnEndReload()
{
	bIsReloading = false;
	UE_LOG(SurvivalDebugLog, Error, TEXT("RELOAD END"));
	ReloadTimeAccum = 0.0f;
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
	if (Role != ROLE_Authority)
	{
		UE_LOG(InventorySystemLog, Warning, TEXT("Not authority! Not adding item..."));
		return;
	}

	UE_LOG(InventorySystemLog, Warning, TEXT("Handling pickup for item Name: ['%s'] with ID: ['%s'] - Class: '%s'"), 
		*ItemPickup->ItemTypeReference->Name.ToString(), 
		*ItemPickup->ItemTypeReference->ID.ToString(), 
		*ItemPickup->ItemTypeReference->GetClass()->GetPathName());

	if (InventoryComponent->AddItem(
		ItemPickup->ItemTypeReference->ID, 
		ItemPickup->StackSize, 
		ItemPickup->ItemTypeReference->GetItemType(), 
		ItemPickup->ItemTypeClass))
	{
		ItemPickup->Destroy(true);

		// DEBUG print inventory
		for (int i = 0; i < InventoryComponent->Items.Num(); i++)
		{
			const FItemSlotInfo *ItemSlot = &InventoryComponent->Items[i];

			if (ItemSlot->ItemTypeReference->GetItemType() == EItemType::IT_Item)
			{
				UE_LOG(InventorySystemLog, Warning, TEXT("Item: [%s] - Slot: %d | Stack: [%d] | MaxStack: [%d] | Class: [%s]"),
					*ItemSlot->ItemTypeReference->Name.ToString(),
					ItemSlot->SlotIndex,
					ItemSlot->StackSize,
					ItemSlot->MaxStackSize,
					*ItemSlot->ItemTypeClass->GetSuperClass()->GetName());
			}
			else
			{
				FString type = FString(TEXT("Base Weapon"));
				UBaseWeaponItem *Wep = Cast<UBaseWeaponItem>(ItemSlot->ItemTypeReference);
				if(Wep && Wep->IsValidLowLevel())
				{
					if (Wep->WeaponType == EWeaponType::WT_Projectile)
					{
						type = FString(TEXT("WT_Projectile"));
					}
					else if( Wep->WeaponType == EWeaponType::WT_Bludgeon)
					{
						type = FString(TEXT("WT_Bludgeon"));
					}
				}

				UE_LOG(InventorySystemLog, Warning, TEXT("Item: [%s] - Slot: %d | Stack: [%d] | MaxStack: [%d] | Clip: [%d] | MaxClip: [%d] | Type: [%s] | Class: [%s]"),
					*ItemSlot->ItemTypeReference->Name.ToString(),
					ItemSlot->SlotIndex,
					ItemSlot->StackSize,
					ItemSlot->MaxStackSize,
					Wep->ClipSize,
					Wep->MaxClipSize,
					*type,
					*ItemSlot->ItemTypeClass->GetSuperClass()->GetName());
			}
			
		}
	}
	else
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Failed to add item to inventory."));
	}

	// TODO: Make ready item and add to inventory, serverside.
	// InventoryComponent handles all add/checks
}

void ASurvivalCharacter::HandleEquipWeapon(UBaseWeaponItem *WeaponItem)
{
	if (WeaponItem)
	{
		if (WeaponItem->DetailMesh && WeaponItem->DetailMesh->IsValidLowLevel())
		{
			FP_Gun->SetSkeletalMesh(WeaponItem->DetailMesh);
		}
		else
		{
			UE_LOG(SurvivalDebugLog, Warning, TEXT("HandleEquipWeapon : Detailmesh not valid!"));
		}
	}
	else
	{
		UE_LOG(SurvivalDebugLog, Warning, TEXT("HandleEquipWeapon : Weapon item not valid!"));
	}
}

bool ASurvivalCharacter::CraftItems(int32 SlotA, int32 SlotB)
{
	if (!InventoryComponent)
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Inventory System is null. Cannot craft."));
		return false;
	}

	UWorld *World = GetWorld();
	if (World)
	{
		// Only on Server. This is singleplayer, so it will not matter.
		ASurvivalGameMode *GM = World->GetAuthGameMode<ASurvivalGameMode>();
		if (!GM)
		{
			UE_LOG(SurvivalDebugLog, Error, TEXT("ASurvivalCharacter::CraftItems : Could not retrieve gamemode!"));
			return false;
		}

		// Try to craft the items.
		return InventoryComponent->CraftItem(SlotA, SlotB, GM->InventorySystemManager);
	}
	else
	{
		UE_LOG(SurvivalDebugLog, Error, TEXT("ASurvivalCharacter::CraftItems : World not initialized!"));
		return false;
	}
}

bool ASurvivalCharacter::DropItem(int32 SlotIndex, int32 StackSize)
{
	if (InventoryComponent)
	{
		return InventoryComponent->DropItem(SlotIndex, StackSize);
	}
	else
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot drop item - InventoryComponent is NULL!"));
		return false;
	}
}

bool ASurvivalCharacter::DropItemSlot(int32 SlotIndex)
{
	if (InventoryComponent)
	{
		return InventoryComponent->DropItem(SlotIndex, INDEX_NONE);
	}
	else
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot drop item - InventoryComponent is NULL!"));
		return false;
	}
}

bool ASurvivalCharacter::SwapItemSlots(int32 SlotA, int32 SlotB)
{
	if (InventoryComponent)
	{
		return InventoryComponent->SwapSlot(SlotA, SlotB);
	}
	else
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot swap item slots - InventoryComponent is NULL!"));
		return false;
	}
}

void ASurvivalCharacter::Equip(int32 Slot)
{
	if (InventoryComponent)
	{
		InventoryComponent->EquipItem(Slot, this);
	}
	else
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot equip item - InventoryComponent is NULL!"));
	}
}

void ASurvivalCharacter::UnEquip()
{
	if (FP_Gun && FP_Gun->IsValidLowLevel())
	{
		FP_Gun->SetSkeletalMesh(NULL);
		if (InventoryComponent)
		{
			InventoryComponent->EquippedWeapon = nullptr;
		}
	}
}

bool ASurvivalCharacter::UseItem(int32 Slot)
{
	if (InventoryComponent)
	{
		bool WasUsed = InventoryComponent->UseItem(Slot, this);
		if (WasUsed)
		{
			// TODO: Do we need event for UI updates???
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		UE_LOG(InventorySystemLog, Error, TEXT("Cannot use item - InventoryComponent is NULL!"));
		return false;
	}
}