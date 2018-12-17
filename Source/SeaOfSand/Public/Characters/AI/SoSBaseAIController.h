// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SoSBaseAIController.generated.h"


class ASoSAICharacterBase;
class USoSAIDecision;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;


UCLASS()
class SEAOFSAND_API ASoSBaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASoSBaseAIController();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UAIPerceptionComponent* PerceptionComp;

public:

protected:

	FVector SpawnLocation;

	bool bCreatedDecisions;

	ASoSAICharacterBase * AICharacter;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void ScoreAllDecisions();

	void CreateDecisions();

	void ExecuteDecision(USoSAIDecision* Decision);

	void ActionMoveToActor(AActor* Target);

	void ActionMoveToLocation(const FVector& Location);

	FTimerHandle TimerHandle_ScoreAllDecisions;
};
