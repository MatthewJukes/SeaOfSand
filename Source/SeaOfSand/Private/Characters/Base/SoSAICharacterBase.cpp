/************************************************************************************************************************************
*
*	SoSAICharacterBase.cpp
*	AI character base 
*
*	Written by Matthew Jukes (20/11/2018)
*	Owned by Matthew Jukes
*
****/


#include "SoSAICharacterBase.h"
#include "SoSAIDecision.h"
#include "SoSUtilityAI.h"
#include "SoSASTasks.h"
#include "Perception/AIPerceptionComponent.h"


ASoSAICharacterBase::ASoSAICharacterBase()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
}


void ASoSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}


void ASoSAICharacterBase::CreateDecisionObjects()
{
	for (FAIActionDecision& ActionDecisionData : ActionDecisions)
	{
		USoSAIDecision* NewDecision = NewObject<USoSAIDecision>();
		FString ReferenceString = FString("");
		NewDecision->SetActionDecisionData(ActionDecisionData.DataTable->FindRow<FActionDecisionData>(ActionDecisionData.RowName, ReferenceString));
		DecisionObjects.Add(NewDecision);
	}

	for (FAbilityDecisionPair& AbilityDecisionPair : AbilityDecisionPairs)
	{
		USoSAIDecision* NewDecision = NewObject<USoSAIDecision>();
		FString ReferenceString = FString("");
		NewDecision->SetAbilityDecisionData(AbilityDecisionPair.AbilityDecision.DataTable->FindRow<FAbilityDecisionData>(AbilityDecisionPair.AbilityDecision.RowName, ReferenceString));

		USoSAbilityBase* NewAbility = USoSASTasks::CreateAbilityInstance(AbilityDecisionPair.Ability, CombatComp);
		NewDecision->SetAbility(NewAbility);

		NewDecision->SetIsAbilityDecision(true);

		DecisionObjects.Add(NewDecision);
	}
}


TArray<USoSAIDecision*>& ASoSAICharacterBase::GetDecisions()
{
	return DecisionObjects;
}
