/************************************************************************************************************************************
*
*	SoSAIDecision.cpp
*	Utility-theory AI decision
*
*	Written by Matthew Jukes (21/11/2018)
*	Owned by Matthew Jukes
*
****/


#include "SoSAIDecision.h"
#include "SoSUtilityAI.h"


USoSAIDecision::USoSAIDecision()
{

}


float USoSAIDecision::ScoreDecision()
{
	float FinalScore = 1;
	
	TArray<FConsiderationPreset>* Considerations = bAbilityDecision ? &AbilityDecisionData->Considerations : &ActionDecisionData->Considerations;
	FDecisionContext* DecisionContext = bAbilityDecision ? &AbilityDecisionData->DecisionContext : &ActionDecisionData->DecisionContext;

	if (Considerations->Num() == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s contains no considerations"), *GetDecisionName().ToString());
		return 0;
	}

	float ModificationFactor = 1 - (1.0f / Considerations->Num());
	FString ReferenceString = FString("");

	for (FConsiderationPreset& ConsiderationPreset : *Considerations)
	{
		FDecisionConsideration* Consideration = &ConsiderationPreset.Consideration;
		
		if (Consideration == nullptr) { return 0; }

		float ParamScore = SoSUtilityAI::NormalizeParam(&Consideration->Parameter, DecisionContext);
		float Response = SoSUtilityAI::ComputeResponseCurve(ParamScore, &Consideration->ResponseCurve);

		float MakeUpValue = (1 - Response) * ModificationFactor;
		Response = Response + (MakeUpValue * Response);

		FinalScore *= FMath::Clamp(Response, 0.0f, 1.0f);
	} 

	return FinalScore;
}


bool USoSAIDecision::GetIsAbilityDecision() const
{
	return bAbilityDecision;
}


EDecisionAction USoSAIDecision::GetAction() const
{
	if (ActionDecisionData == nullptr)
	{
		return EDecisionAction::NoAction;
	}

	return  ActionDecisionData->Action;
}


USoSAbilityBase* USoSAIDecision::GetAbility() const
{
	return AbilityDecisionData->Ability;
}


FDecisionContext* USoSAIDecision::GetDecisionContext() const
{
	if (bAbilityDecision)
	{
		if (AbilityDecisionData == nullptr)
		{
			return nullptr;
		}

		return &AbilityDecisionData->DecisionContext;
	}

	if (ActionDecisionData == nullptr)
	{
		return nullptr;
	}

	return &ActionDecisionData->DecisionContext;
}


TArray<FConsiderationPreset>* USoSAIDecision::GetConsiderations() const
{
	if (bAbilityDecision)
	{

		if (AbilityDecisionData == nullptr)
		{
			return nullptr;
		}

		return &AbilityDecisionData->Considerations;
	}

	if (ActionDecisionData == nullptr)
	{
		return nullptr;
	}

	return &ActionDecisionData->Considerations;
} 


void USoSAIDecision::SetIsAbilityDecision(bool NewValue)
{
	bAbilityDecision = NewValue;
}


void USoSAIDecision::SetActionDecisionData(FActionDecisionData* NewDecisionData)
{
	if (ActionDecisionData == nullptr)
	{
		return;
	}

	ActionDecisionData = NewDecisionData;
}


void USoSAIDecision::SetAbilityDecisionData(FAbilityDecisionData* NewDecisionData)
{
	if (AbilityDecisionData == nullptr)
	{
		return;
	}

	AbilityDecisionData = NewDecisionData;
}


void USoSAIDecision::SetAbility(USoSAbilityBase* NewAbility)
{
	if (AbilityDecisionData == nullptr)
	{
		return;
	}

	AbilityDecisionData->Ability = NewAbility;
}

