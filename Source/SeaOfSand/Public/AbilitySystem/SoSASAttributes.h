// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoSASAttributes.generated.h"


UENUM(BlueprintType)
enum class EASAttributeName : uint8
{
	HealthMax,
	HealthCurrent,
	ArmourMax,
	ArmourCurrent,
	EnergyMax,
	EnergyCurrent,
	Speed
};


USTRUCT(BlueprintType)
struct FASAttributeData
{
	GENERATED_BODY()

	float HealthMaxValue;

	float HealthCurrentValue;

	float ArmourMaxValue;

	float ArmourCurrentValue;

	float EnergyMaxValue;

	float EnergyCurrentValue;

	float SpeedValue;
};


UENUM(BlueprintType)
enum class EASTag : uint8
{
	Sprinting,
	SprintEnd,
	Aiming, 
	AimEnd,
	WeaponDrawn,
	HolsterWeapon,
	ImmuneToFire
};


UENUM(BlueprintType)
enum class ESoSTeam : uint8
{
	Default,
	Player,
	Enemy
};


UENUM(BlueprintType)
enum class EASDamageTypeName : uint8
{
	Default,
	Pure,
	Fire
};


USTRUCT(BlueprintType)
struct FASDamageType : public FTableRowBase
{
	GENERATED_BODY()
		
	// Percent of damage that bypasses armour
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "100.0", UIMax = "100.0"))
	float ArmourPenetration;

	// Percent of negated damage armour takes instead 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ArmourDamage;

};
