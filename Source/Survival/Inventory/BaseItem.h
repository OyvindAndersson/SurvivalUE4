// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "BaseItem.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Item			UMETA(DisplayName = "9mm Pistol"),
	IT_Weapon		UMETA(DisplayName = "Rifle")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	AT_Pistol		UMETA(DisplayName = "9mm Pistol"),
	AT_Rifle		UMETA(DisplayName = "Rifle"),
	AT_Shotgun		UMETA(DisplayName = "Shotgun")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_Projectile		UMETA(DisplayName = "Firearm"),
	WT_Bludgeon			UMETA(DisplayName = "Bludgeon")
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

	FORCEINLINE EItemType GetItemType()
	{
		return ItemType;
	}

	UBaseItem();
};
