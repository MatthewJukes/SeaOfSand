// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "GameFramework/Actor.h"
#include "SoSASEffect.generated.h"


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
struct FASEffectData
{
	GENERATED_BODY()

	// Effect properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASAttributeName AttributeToEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASEffectValueType EffectValueType;

	// Magnitude of the effect. Multiplicative and Subtractive should be given in percentage values. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float EffectValue;

	// Length of the effect in seconds, 0 means the duration is infinite
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = 0))
	float EffectDuration; 

	// The rate in seconds at which the value is reapplied over the duration of the effect, 0 means the effect will tick only once
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = 0))
	float TickRate; 

	// Delay the first value application of the effect, e.g. an effect that ticks once will have the value applied when the duration has ended instead of at the start.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bDelayFirstTick; 

	// Maximum number of times the effect can be applied to the same target at the same time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = 1))
	int MaxStacks; 

	// How many times the effect is applied to the target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = 1))
	int StacksPerApplication;

	// Is the effect a temporary stat modifier? Generally should be true when effect things like max health or speed and you want these to be restored when the effect end. False for an effect like draining current health or energy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bTemporaryModifier; 

	// Effect status trackers

	float EffectStartTime;

	float TimeSinceLastTick;

	float TotalValue;

	int CurrentStacks;
};

UCLASS()
class SEAOFSAND_API ASoSASEffect : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FASEffectData Effect;

};