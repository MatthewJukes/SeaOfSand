// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSASAttributes.h"
#include "UObject/NoExportTypes.h"
#include "SoSASTasks.generated.h"

class USoSASComponent;
struct FASEffectData;

UCLASS()
class SEAOFSAND_API USoSASTasks : public UObject
{
	GENERATED_BODY()

protected:

	bool ApplyEffectToTarget(FASEffectData* EffectToApply, AActor* Target, AActor* Instigator);
	
	void AddValueToASAttribute(USoSASComponent* ASComponent, EASAttributeName AttributeToEffect, float Value);

	void AddMultiplierToASAttribute(USoSASComponent* ASComponent, EASAttributeName AttributeToEffect, float Value);

	void AddDividerToASAttribte(USoSASComponent* ASComponent, EASAttributeName AttributeToEffect, float Value);

};
