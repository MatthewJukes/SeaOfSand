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
#include "Engine/DataTable.h"
#include "SoSUtilityAI.generated.h"


class ASoSCharacterBase;
class UCurveFloat;


UENUM(BlueprintType)
enum class ECurveType : uint8
{
	Linear,
	Exponential,
	Logistic
};


UENUM(BlueprintType)
enum class EConsiderationParameters : uint8
{
	MyHealth,
	MyDistanceToTarget,
	MyDirectionOffsetToTarget,
	MyLineOfSightToTarget,
	TargetHealth
};


USTRUCT(BlueprintType)
struct FDecisionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
	ASoSCharacterBase* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Utility AI")
	AActor* Target;
};


USTRUCT(BlueprintType)
struct FConsiderationParameter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	EConsiderationParameters ConsiderationParameter;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	float RangeMin;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	float RangeMax;
};


USTRUCT(BlueprintType)
struct FResponseCurveData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	UCurveFloat* Curve;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetX;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetY;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float OffsetSlope;

	//UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	//float Exponent;
};


USTRUCT(BlueprintType)
struct FDecisionConsideration : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FResponseCurveData ResponseCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FConsiderationParameter Parameter;
};


USTRUCT(BlueprintType)
struct FDecision : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FName DecisionName;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	FDecisionContext DecisionContext;

	UPROPERTY(EditDefaultsOnly, Category = "Utility AI")
	TArray<FDecisionConsideration> Considerations;

	float Score;
};


class SEAOFSAND_API SoSUtilityAI
{
public:

	static float ScoreDecision(FDecision& Decision);

	static float NormalizeParam(FConsiderationParameter& Parameter, FDecisionContext& Context);

	static float ConsiderationMyHealthPercent(FConsiderationParameter& Parameter, FDecisionContext& Context);

	static float ConsiderationDistanceToTarget(FConsiderationParameter& Parameter, FDecisionContext& Context);

	static float ComputeResponseCurve(float Score, FResponseCurveData& Curve);
};
