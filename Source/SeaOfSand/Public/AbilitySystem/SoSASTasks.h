// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoSASTasks.generated.h"

struct FASEffectData;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//UFUNCTION(BlueprintCallable)
	static bool ApplyEffectToTarget(FASEffectData* EffectToApply, AActor* Target, AActor* Instigator, float ApplicationTime);
};
