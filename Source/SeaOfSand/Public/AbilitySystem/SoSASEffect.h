// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSASEffect.generated.h"

UENUM(BlueprintType)
enum class EASEffectType : uint8
{
	Positive,
	Neutral,
	Negative
};

UENUM(BlueprintType)
enum class EASEffectValueType : uint8
{
	Additive,
	Multiplicative,
	Subtractive
};

USTRUCT(BlueprintType)
struct FASEffectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASAttributeName AttributeToEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASEffectValueType EffectValueType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float EffectValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float EffectDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int MaxStacks;
};
