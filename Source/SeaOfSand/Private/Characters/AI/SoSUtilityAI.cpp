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
#include "SoSASTasks.h"
#include "SoSAIDecision.h"
#include "SoSCharacterBase.h"
#include "SoSCombatComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/World.h"


float SoSUtilityAI::NormalizeParam(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	float NormalizedParamValue = 0.0f;

	switch (Parameter->ConsiderationParameter)
	{
	default:
		break;
	case EConsiderationParameters::MyHealth:
		NormalizedParamValue = ConsiderationMyHealthPercent(Parameter, Context);
		break;
	case EConsiderationParameters::MyDistanceToTarget:
		NormalizedParamValue = ConsiderationMyDistanceToTarget(Parameter, Context);
		break;
	case EConsiderationParameters::MyDirectionOffsetToTarget:
		NormalizedParamValue = ConsiderationMyDirectionOffsetToTarget(Parameter, Context);
		break;
	case EConsiderationParameters::MyLineOfSightToTarget:
		NormalizedParamValue = ConsiderationMyLineOfSightToTarget(Parameter, Context);
		break;
	case EConsiderationParameters::TargetHealth:
		NormalizedParamValue = ConsiderationTargetHealthPercent(Parameter, Context);
		break;
	case EConsiderationParameters::TargetDirectionOffsetToMe:
		NormalizedParamValue = ConsiderationTargetDirectionOffsetToMe(Parameter, Context);
		break;
	}

	return NormalizedParamValue;
}


float SoSUtilityAI::ConsiderationMyHealthPercent(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	if (Context->SourceCharacter == nullptr)
	{
		return 0.0f;
	}

	return Context->SourceCharacter->GetCharacterCombatComponent()->GetAttributeTotalValue(EAttributeName::HealthCurrent) / Context->SourceCharacter->GetCharacterCombatComponent()->GetAttributeTotalValue(EAttributeName::HealthMax);
}

float SoSUtilityAI::ConsiderationMyDistanceToTarget(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	if (Context->SourceCharacter == nullptr || Context->Target == nullptr)
	{
		return 0.0f;
	}

	FVector StartLocation = Context->SourceCharacter->GetActorLocation();
	FVector TargetLocation = Context->Target->GetActorLocation();
	float Distance = (TargetLocation - StartLocation).Size();

	return FMath::Clamp((Distance - Parameter->RangeMin) / (Parameter->RangeMax - Parameter->RangeMin), 0.0f, 1.0f);
}


float SoSUtilityAI::ConsiderationMyDirectionOffsetToTarget(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	if (Context->SourceCharacter == nullptr || Context->Target == nullptr)
	{
		return 0.0f;
	}

	FVector DirectionToTarget = Context->Target->GetActorLocation() - Context->SourceCharacter->GetActorLocation();
	DirectionToTarget.Z = 0;
	DirectionToTarget.Normalize();

	FVector2D InputRange = FVector2D(0.0f, 3.14159f);
	FVector2D OutputRange = FVector2D(0.0f, 1.0f);

	return 1 - FMath::GetMappedRangeValueClamped(InputRange, OutputRange, FMath::Acos(FVector::DotProduct(DirectionToTarget, Context->SourceCharacter->GetActorForwardVector())));
}


float SoSUtilityAI::ConsiderationMyLineOfSightToTarget(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	if (Context->SourceCharacter == nullptr || Context->Target == nullptr)
	{
		return false;
	}

	UWorld* World = USoSASTasks::AbilityGetWorldFromContextObject(Context->SourceCharacter);
	if (World == nullptr)
	{
		return false;
	}

	const FName TraceTag("LoSTraceTag");
	World->DebugDrawTraceTag = TraceTag;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, Context->SourceCharacter);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.TraceTag = TraceTag;

	FHitResult OutHit;
	if (World->LineTraceSingleByChannel(OutHit, Context->SourceCharacter->GetActorLocation() + FVector(0,0,50), Context->Target->GetActorLocation() + FVector(0, 0, 50), ECC_Visibility, TraceParams))
	{
		return OutHit.Actor == Context->Target ? 1 : 0;
	}
	return 1; // Line-trace didn't hit anything so we assume we can see target
}


float SoSUtilityAI::ConsiderationTargetHealthPercent(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	if (Context->Target == nullptr)
	{
		return 0.0f;
	}

	ASoSCharacterBase* TargetCharacter = Cast<ASoSCharacterBase>(Context->Target);
	if (TargetCharacter == nullptr)
	{
		return 0.0f;
	}

	return TargetCharacter->GetCharacterCombatComponent()->GetAttributeTotalValue(EAttributeName::HealthCurrent) / TargetCharacter->GetCharacterCombatComponent()->GetAttributeTotalValue(EAttributeName::HealthMax);
}


float SoSUtilityAI::ConsiderationTargetDirectionOffsetToMe(FConsiderationParameter* Parameter, FDecisionContext* Context)
{
	if (Context->SourceCharacter == nullptr || Context->Target == nullptr)
	{
		return 0.0f;
	}

	FVector DirectionToMe = Context->SourceCharacter->GetActorLocation() - Context->Target->GetActorLocation();
	DirectionToMe.Z = 0;
	DirectionToMe.Normalize();

	FVector2D InputRange = FVector2D(0.0f, 3.14159f);
	FVector2D OutputRange = FVector2D(0.0f, 1.0f);

	return 1 - FMath::GetMappedRangeValueClamped(InputRange, OutputRange, FMath::Acos(FVector::DotProduct(DirectionToMe, Context->Target->GetActorForwardVector())));
}


float SoSUtilityAI::ComputeResponseCurve(float Score, FResponseCurveData* Curve)
{
	if (Curve == nullptr)
	{
		return Score;
	}

	float Response = 1.0f;
	Response = Curve->Curve->GetFloatValue(Score - Curve->OffsetX) + Curve->OffsetY;
	return FMath::Clamp(Response, 0.0f, 1.0f);
}
