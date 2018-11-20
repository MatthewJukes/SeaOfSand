/************************************************************************************************************************************
*
*	SoSUtilityAI.cpp
*	Utility-theory AI
*
*	Written by Matthew Jukes (20/11/2018)
*	Owned by Matthew Jukes
*
****/

#include "SoSUtilityAI.h"
#include "SoSCharacterBase.h"
#include "SoSCombatComponent.h"
#include "Curves/CurveFloat.h"


float SoSUtilityAI::ScoreDecision(FDecision& Decision)
{
	float FinalScore = 1;
	float ModificationFactor = 1 - (1 / Decision.Considerations.Num());
	
	for (FDecisionConsideration& Consideration : Decision.Considerations)
	{
		float Score = NormalizeParam(Consideration.Parameter, Decision.DecisionContext);
		float Response = ComputeResponseCurve(Score, Consideration.ResponseCurve);

		float MakeUpValue = (1 - Response) * ModificationFactor;
		Response = Response + (MakeUpValue * Response);

		FinalScore *= FMath::Clamp(Response, 0.0f, 1.0f);
	}
	
	return FinalScore; // TODO adjust for multiple considerations
}


float SoSUtilityAI::NormalizeParam(FConsiderationParameter& Parameter, FDecisionContext& Context)
{
	float NormalizedParamValue = 0.0f;

	switch (Parameter.ConsiderationParameter)
	{
	default:
		break;
	case EConsiderationParameters::MyHealth:
		NormalizedParamValue = ConsiderationMyHealthPercent(Parameter, Context);
		break;
	case EConsiderationParameters::MyDistanceToTarget:
		NormalizedParamValue = ConsiderationDistanceToTarget(Parameter, Context);
		break;
	case EConsiderationParameters::MyDirectionOffsetToTarget:
		break;
	case EConsiderationParameters::MyLineOfSightToTarget:
		break;
	case EConsiderationParameters::TargetHealth:
		break;
	}

	return NormalizedParamValue;
}


float SoSUtilityAI::ConsiderationMyHealthPercent(FConsiderationParameter& Parameter, FDecisionContext& Context)
{
	if (Context.Character == nullptr)
	{
		return 0.0f;
	}

	return Context.Character->GetCharacterCombatComponent()->GetAttributeTotalValue(EAttributeName::HealthCurrent) / Context.Character->GetCharacterCombatComponent()->GetAttributeTotalValue(EAttributeName::HealthMax);
}


float SoSUtilityAI::ConsiderationDistanceToTarget(FConsiderationParameter& Parameter, FDecisionContext& Context)
{
	if (Context.Character == nullptr || Context.Target == nullptr)
	{
		return 0.0f;
	}

	FVector StartLocation = Context.Character->GetActorLocation();
	FVector TargetLocation = Context.Target->GetActorLocation();
	float Distance = (TargetLocation - StartLocation).Size();

	return FMath::Clamp((Distance - Parameter.RangeMin) / (Parameter.RangeMax - Parameter.RangeMin), 0.0f, 1.0f);
}


float SoSUtilityAI::ComputeResponseCurve(float Score, FResponseCurveData& Curve)
{
	float Response = 1.0f;
	Response = Curve.Curve->GetFloatValue(Score); //TODO expand functionality 
	return Response;
}
