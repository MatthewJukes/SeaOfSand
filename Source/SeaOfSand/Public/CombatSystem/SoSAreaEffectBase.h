// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoSAbilityActor.h"
#include "SoSAreaEffectBase.generated.h"

UCLASS()
class SEAOFSAND_API ASoSAreaEffectBase : public ASoSAbilityActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoSAreaEffectBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

protected:

	float TickRate;
	
	virtual void EndAbilityActor_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "AreaEffect")
	void AreaEffectTick();

	FTimerHandle TimerHandle_AreaEffectTick;
};
