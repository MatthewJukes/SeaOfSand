// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoSASEffectData.h"
#include "SoSASEffectBase.generated.h"

UCLASS(Blueprintable)
class SEAOFSAND_API USoSASEffectBase : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FASEffectData EffectData;
	
};
