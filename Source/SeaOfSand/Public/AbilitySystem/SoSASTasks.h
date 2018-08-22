// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "UObject/NoExportTypes.h"
#include "SoSASTasks.generated.h"

USTRUCT(BlueprintType)
struct FTestData
{
	GENERATED_BODY();
};

UCLASS()
class SEAOFSAND_API USoSASTasks : public UObject
{
	GENERATED_BODY()

public:

	FTestData* Test;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(FTestData Test, AActor* Target, AActor* Instigator);
};
