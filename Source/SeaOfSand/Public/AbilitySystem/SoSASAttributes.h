// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.generated.h"

UENUM(BlueprintType)
enum class EASAttributeName : uint8
{
	HealthMax,
	HealthCurrent,
	ArmourMax,
	ArmourCurrent,
	Speed,
	EnergyMax,
	EnergyCurrent,
	EASATTRIBUTENAME_NR_ITEMS UMETA(Hidden)
};
