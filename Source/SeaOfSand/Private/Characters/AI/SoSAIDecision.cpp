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
	
	if (DecisionData.Considerations.Num() == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s contains no considerations"), *GetDecisionName().ToString());
		return 0;
	}

	float ModificationFactor = 1 - (1.0f / DecisionData.Considerations.Num());
	FString ReferenceString = FString("");

	for (FDataTableRowHandle& DataTableRowConsideration : DecisionData.Considerations)
	{
		FDecisionConsideration* Consideration = DataTableRowConsideration.DataTable->FindRow<FDecisionConsideration>(DataTableRowConsideration.RowName, ReferenceString);
		float ParamScore = SoSUtilityAI::NormalizeParam(&Consideration->Parameter, &DecisionData.DecisionContext);
		float Response = SoSUtilityAI::ComputeResponseCurve(ParamScore, &Consideration->ResponseCurve);

		float MakeUpValue = (1 - Response) * ModificationFactor;
		Response = Response + (MakeUpValue * Response);

		FinalScore *= FMath::Clamp(Response, 0.0f, 1.0f);
	} 

	return FinalScore;
}



EDecisionAction USoSAIDecision::GetAction() const
{
	return  DecisionData.Action;
}


FDecisionContext& USoSAIDecision::GetDecisionContext() 
{
	return DecisionData.DecisionContext;
}


TArray<FDataTableRowHandle>& USoSAIDecision::GetConsiderations() 
{
	return DecisionData.Considerations;
} 
