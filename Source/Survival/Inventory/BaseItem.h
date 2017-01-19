// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Decorators/UsableInterface.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Item			UMETA(DisplayName = "Item"),
	IT_Weapon		UMETA(DisplayName = "Weapon")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	AT_Other		UMETA(DisplayName = "Other"),
	AT_Pistol		UMETA(DisplayName = "9mm Pistol"),
	AT_Rifle		UMETA(DisplayName = "Rifle"),
	AT_Shotgun		UMETA(DisplayName = "Shotgun")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_Projectile		UMETA(DisplayName = "Firearm"),
	WT_Bludgeon			UMETA(DisplayName = "Bludgeon"),
	WT_Other			UMETA(DisplayName = "Other")
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SURVIVAL_API UBaseItem : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseItem)
	EItemType ItemType;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	FName ID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	int32 Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	class UStaticMesh *WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	class USkeletalMesh *DetailMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseItem)
	class UTexture2D *Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BaseItem)
	bool CanDrop;

	FORCEINLINE EItemType GetItemType()
	{
		return ItemType;
	}

	UBaseItem();
};
