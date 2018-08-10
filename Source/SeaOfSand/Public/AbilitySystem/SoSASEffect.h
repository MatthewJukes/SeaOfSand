// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "SoSASEffect.generated.h"

USTRUCT(BlueprintType)
struct FASEffectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	EASAttributeName AttributeToEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int MaxStacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bIsNegative;
};
