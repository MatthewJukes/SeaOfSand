// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SoSBaseAIController.generated.h"


class ASoSAICharacterBase;
class USoSAIDecision;


UCLASS()
class SEAOFSAND_API ASoSBaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASoSBaseAIController();

protected:

	virtual void BeginPlay() override;

public:

protected:

	ASoSAICharacterBase * AICharacter;

	UFUNCTION()
	void ScoreAllDecisions();

	void ExecuteDecision(USoSAIDecision* Decision);

	void ActionMoveToActor(AActor* Target);

	void ActionMoveToLocation(const FVector& Location);

	FTimerHandle TimerHandle_ScoreAllDecisions;
};
