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
#include "SoSUtilityAI.h"


ASoSAICharacterBase::ASoSAICharacterBase()
{

}


void ASoSAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}


void ASoSAICharacterBase::CreateDecisionObjects()
{
	for (TSubclassOf<USoSAIDecision> DecisionClass : BP_Decisions)
	{
		if (DecisionClass == nullptr)
		{
			return;
		}

		Decisions.Add(NewObject<USoSAIDecision>(DecisionClass, DecisionClass.Get()));
	}
}


TArray<USoSAIDecision*>& ASoSAICharacterBase::GetDecisions()
{
	return Decisions;
}
