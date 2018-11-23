// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseAIController.h"
#include "SoSUtilityAI.h"
#include "SoSAIDecision.h"
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
	Super::BeginPlay();

	AICharacter = Cast<ASoSAICharacterBase>(GetPawn());
	
	if (AICharacter)
	{
		AICharacter->CreateDecisionObjects();

		TArray<USoSAIDecision*>& Decisions = AICharacter->GetDecisions();
		for (USoSAIDecision* Decision : Decisions)
		{
			//Decision->GetDecisionContext().Character = AICharacter;
			//Decision->GetDecisionContext().Target = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(AICharacter, 0));
		}
	} 

	GetWorldTimerManager().SetTimer(TimerHandle_ScoreAllDecisions, this, &ASoSBaseAIController::ScoreAllDecisions, 0.5f, true);
}


void ASoSBaseAIController::ScoreAllDecisions()
{ 
	USoSAIDecision* NewDecision = nullptr;

	if (AICharacter)
	{
		TArray<USoSAIDecision*>& Decisions = AICharacter->GetDecisions();
		float BestScore = 0;
		for (USoSAIDecision* Decision : Decisions)
		{
			float NewScore = Decision->ScoreDecision();

			if (NewDecision == nullptr || NewScore > BestScore)
			{
				NewDecision = Decision;
				BestScore = NewScore;
			}
		}
			
		ExecuteDecision(NewDecision);
		if (NewDecision != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("%s wins with a score of :%f"), *NewDecision->GetDecisionName().ToString(), BestScore);
		}
	} 
	
}


void ASoSBaseAIController::ExecuteDecision(USoSAIDecision* Decision)
{
	if (Decision == nullptr)
	{
		return;
	}

	/*
	switch (Decision->GetAction())
	{
	case EDecisionAction::MoveToActor:
		//ActionMoveToActor(Decision->GetDecisionContext().Target);
		break;
	case EDecisionAction::MoveToLocation:
		break;
	default:
		break;
	} */
}


void ASoSBaseAIController::ActionMoveToActor(AActor* Target)
{
	if (Target == nullptr)
	{
		return;
	}

	MoveToActor(Target);
}


void ASoSBaseAIController::ActionMoveToLocation(const FVector& Location)
{
	MoveToLocation(Location);
}

