// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASEffectData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoSASTasks.generated.h"

//struct FASEffectData;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool ApplyASEffectToTarget(FASEffectData EffectToApply, AActor* Target, AActor* Instigator, float EffectDuration, float ApplicationTime);

	UFUNCTION(BlueprintCallable, Category = "ASTasks")
	static bool CheckIfTargetHasASEffectActive(EASEffectName EffectName, EASEffectType EffectType, AActor* Target, int32& OutIndex);

private:

	static void ReapplyASEffect(FASEffectData& ExistingEffect, FASEffectData& NewEffect, float ApplicationTime);
};
