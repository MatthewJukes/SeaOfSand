// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseAIController.h"
#include "SoSCharacterBase.h"
#include "SoSAICharacterBase.h"
#include "Public/TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


ASoSBaseAIController::ASoSBaseAIController()
{
	
}


void ASoSBaseAIController::BeginPlay()
{
	AICharacter = Cast<ASoSAICharacterBase>(GetPawn());
	
	if (AICharacter)
	{
		TArray<FDecision>& Decisions = AICharacter->GetDecisions();
		for (FDecision& Decision : Decisions)
		{
			Decision.DecisionContext.Character = AICharacter;
			Decision.DecisionContext.Target = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(AICharacter, 0));
		}
	} 

	GetWorldTimerManager().SetTimer(TimerHandle_ScoreAllDecisions, this, &ASoSBaseAIController::ScoreAllDecisions, 0.5f, true);
}


void ASoSBaseAIController::ScoreAllDecisions()
{ 
	FDecision* NewDecision = nullptr;

	if (AICharacter)
	{
		TArray<FDecision>& Decisions = AICharacter->GetDecisions();
		for (FDecision& Decision : Decisions)
		{
			Decision.Score = SoSUtilityAI::ScoreDecision(Decision);

			if (NewDecision == nullptr || NewDecision->Score < Decision.Score)
			{
				NewDecision = &Decision;
			}
		}
			
		UE_LOG(LogTemp, Warning, TEXT("%s wins with a score of :%f"), *NewDecision->DecisionName.ToString(), NewDecision->Score);
	} 
	
}

