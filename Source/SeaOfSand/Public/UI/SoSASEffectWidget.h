// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBlueprint.h"
#include "SoSEffectData.h"
#include "SoSASEffectWidget.generated.h"


UCLASS(BlueprintType, Blueprintable)
class SEAOFSAND_API USoSASEffectWidget : public UWidgetBlueprint
{
	GENERATED_BODY()
	
protected:

	USoSASEffectWidget();

	FEffectData* EffectReference;	

public:

	FEffectData* GetEffect();

	//UFUNCTION(BlueprintCallable, "EffectWidget")
	//void SetEffectReference(FASEffectData& Effect);
};
