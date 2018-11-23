/************************************************************************************************************************************
*
*	SoSUtilityAI.h
*	Utility-theory AI
*
*	Written by Matthew Jukes (20/11/2018)
*	Owned by Matthew Jukes
*
****/

#pragma once

#include "CoreMinimal.h"
//#include "SoSUtilityAI.generated.h"


struct FConsiderationParameter;
struct FDecisionContext;
struct FResponseCurveData;


class SEAOFSAND_API SoSUtilityAI
{
public:

	static float NormalizeParam(FConsiderationParameter* Parameter, FDecisionContext* Context);

	static float ConsiderationMyHealthPercent(FConsiderationParameter* Parameter, FDecisionContext* Context);

	static float ConsiderationDistanceToTarget(FConsiderationParameter* Parameter, FDecisionContext* Context);

	static float ComputeResponseCurve(float Score, FResponseCurveData* Curve);
};
