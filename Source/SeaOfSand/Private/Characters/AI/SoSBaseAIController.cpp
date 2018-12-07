// Fill out your copyright notice in the Description page of Project Settings.

#include "SoSBaseAIController.h"
#include "SoSUtilityAI.h"
#include "SoSAIDecision.h"
#include "SoSCharacterBase.h"
#include "SoSAICharacterBase.h"
#include "SoSCombatComponent.h"
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

	GetWorldTimerManager().SetTimer(TimerHandle_ScoreAllDecisions, this, &ASoSBaseAIController::ScoreAllDecisions, 0.5f, true);
}


void ASoSBaseAIController::ScoreAllDecisions()
{ 
	CreateDecisions();

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
			UE_LOG(LogTemp, Warning, TEXT("%s wins with a score of :%f"), *NewDecision->GetFName().ToString(), BestScore);
		}
	}
}

// TODO placeholder until system is more fully implemented
void ASoSBaseAIController::CreateDecisions()
{
	if (bCreatedDecisions)
	{
		return;
	}

	if (AICharacter)
	{
		AICharacter->CreateDecisionObjects();

		TArray<USoSAIDecision*>& Decisions = AICharacter->GetDecisions();
		for (USoSAIDecision* Decision : Decisions)
		{
			Decision->GetDecisionContext().SourceCharacter = AICharacter; //TODO this is placeholder for getting context data
			Decision->GetDecisionContext().Target = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(AICharacter, 0));
		}
	}

	bCreatedDecisions = true;
}


void ASoSBaseAIController::ExecuteDecision(USoSAIDecision* Decision)
{
	if (Decision == nullptr)
	{
		return;
	}
	

	if (Decision->GetIsAbilityDecision())
	{
		AICharacter->GetCharacterCombatComponent()->UseAbility(Decision->GetAbility()); // Start
		AICharacter->GetCharacterCombatComponent()->UseAbility(Decision->GetAbility(), true); // Release
		return;
	}


	switch (Decision->GetAction())
	{
	case EDecisionAction::MoveToActor:
		ActionMoveToActor(Decision->GetDecisionContext().Target);
		break;
	case EDecisionAction::MoveToLocation:
		break;
	default:
		break;
	} 
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

