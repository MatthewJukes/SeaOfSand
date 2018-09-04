// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SoSASAttributes.h"
#include "SoSASEffectData.generated.h"


UENUM(BlueprintType)
enum class EASEffectName : uint8
{ 
	//Positive

	// Neutral
	PlayerSprint,
	PlayerPistolAiming,

	//Negative
	Burning                    
	
};


UENUM(BlueprintType)
enum class EASEffectType : uint8
{
	Positive,
	Neutral,
	Negative
};


UENUM(BlueprintType)
enum class EASEffectValueType : uint8 // Attribute modification formula type
{
	Additive,
	Multiplicative,
	Subtractive
};


USTRUCT(BlueprintType)
struct FASEffectData : public FTableRowBase
{
	GENERATED_BODY()

	// Effect properties

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASEffectName EffectName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASAttributeName AttributeToEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASEffectType EffectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	EASEffectValueType EffectValueType;

	// Magnitude of the effect. Multiplicative and Subtractive should be given in percentage values. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float EffectValue;

	// The rate in seconds at which the value is reapplied over the duration of the effect, 0 means the effect will tick only once
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float TickRate; 

	// Delay the first value application of the effect, e.g. an effect that ticks once will have the value applied when the duration has ended instead of at the start.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bDelayFirstTick; 

	// Maximum number of times the effect can be applied to the same target at the same time
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "1"))
	int32 MaxStacks = 1;

	// How many times the effect is applied to the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "1"))
	int32 StacksPerApplication = 1;

	// Is the effect a temporary stat modifier? Generally should be true when effect things like max health or speed and you want these to be restored when the effect end. False for an effect like draining current health or energy.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bTemporaryModifier; 

	// If reapplied, duration added to remaining duration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	bool bAdditiveDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TArray<EASEffectName> EffectBlockedBy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TArray<EASEffectName> EffectRemoves;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TArray<EASEffectName> EffectRequires;

	// Effect status trackers

	float EffectDuration; 

	float EffectStartTime;

	float LastTickTime;

	float TotalValue;

	int32 CurrentStacks;

	bool bExpired = false;
};