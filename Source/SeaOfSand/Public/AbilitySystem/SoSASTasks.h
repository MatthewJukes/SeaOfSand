// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASTasks.generated.h"

struct FASEffectData;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UObject
{
	GENERATED_BODY()

protected:

	bool ApplyEffectToTarget(FASEffectData* EffectToApply, AActor* Target, AActor* Instigator);
	
};
